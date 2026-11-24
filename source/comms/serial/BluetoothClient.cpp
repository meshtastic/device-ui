#include "comms/BluetoothClient.h"

#if defined(ARCH_ESP32)

#include "comms/MeshEnvelope.h"
#include "util/ILog.h"
#include <cstring>

// GATT table of the firmware server, see src/BluetoothCommon.h
static const char *PHONEAPI_SERVICE_UUID = "6ba1b218-15a8-461f-9fa8-5dcae273eafd";
static const char *TORADIO_UUID = "f75c76d2-129e-4dad-a1dd-7866124401e7";
static const char *FROMRADIO_UUID = "2c55e69e-4993-11ed-b878-0242ac120002";
static const char *FROMNUM_UUID = "ed9da18c-a800-4f66-a670-aa7547e34453";

static const uint32_t SCAN_CHUNK_MS = 500;
static const uint32_t CONNECT_TIMEOUT_MS = 8000;
static const uint32_t POLL_MS = 50;
static const uint32_t RETRY_MS = 5000;
static const uint32_t RETRY_FAIL_MS = 30000;
static const uint8_t MAX_CONSECUTIVE_FAILS = 3;
static const size_t RX_LIMIT = 8192;
static const size_t TX_LIMIT = 32;

BluetoothClient::BluetoothClient(void) : SerialClient("ble") {}

BluetoothClient::~BluetoothClient()
{
    m_shutdown = true;
    if (m_taskHandle != nullptr) {
        while (!m_taskDone) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void BluetoothClient::start_stack(void)
{
    if (BLE.isInitialized())
        return;
    if (!BLE.begin("device-ui")) {
        ILOG_ERROR("BLE stack init failed");
        phase(eError, "BLE init failed");
        return;
    }
    ILOG_INFO("BLE stack started");
    BLE.setPower(9);

    BLESecurity security = BLE.getSecurity();
    security.setAuthenticationMode(true, true, true);
    security.setIOCapability(BLEIOCapability::KeyboardOnly);
    security.onPassKeyRequest([this](const BLEConnInfo &) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_passkey;
    });

    BLEScan scan = BLE.getScan();
    scan.setActiveScan(true);
    scan.setInterval(100);
    scan.setWindow(99);
}

void BluetoothClient::stop_stack(void)
{
    if (BLE.isInitialized()) {
        BLE.end();
        ILOG_INFO("BLE stack stopped");
    }
}

void BluetoothClient::init(void)
{
    SerialClient::init();

    start_stack();
    xTaskCreate(task_entry, "blelink", 8192, this, 1, &m_taskHandle);
}

void BluetoothClient::setDeviceName(const std::string &name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_name = name;
}

void BluetoothClient::setPasskey(uint32_t passkey)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_passkey = passkey;
}

bool BluetoothClient::connect(void)
{
    m_retryAt = 0;
    m_consecutiveFails = 0;
    m_want = true;
    phase(eConnecting, "connecting");
    return true;
}

bool BluetoothClient::disconnect(void)
{
    m_want = false;
    return true;
}

bool BluetoothClient::isConnected(void)
{
    return m_linkUp;
}

bool BluetoothClient::send(const uint8_t *buf, size_t len)
{
    if (len < MT_HEADER_SIZE + 1 || buf[0] != 0x94 || buf[1] != 0xC3) {
        ILOG_WARN("BLE send: bad envelope header");
        return false;
    }
    size_t flen = ((size_t)buf[2] << 8) | buf[3];
    if (len < MT_HEADER_SIZE + flen)
        return false;

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_tx.size() >= TX_LIMIT) {
        ILOG_WARN("BLE send: tx queue full, dropping %d bytes", flen);
        return false;
    }
    m_tx.emplace_back(buf + MT_HEADER_SIZE, buf + MT_HEADER_SIZE + flen);
    return true;
}

size_t BluetoothClient::receive(uint8_t *buf, size_t space_left)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (space_left == 0) {
        // The base buffer filled up mid-frame: the frame is larger than
        // PB_BUFSIZE and can never validate. Drop it whole; frame boundaries
        // in the ring keep the stream in sync for the next packet.
        if (!m_rx.empty() && m_rxPartial > 0) {
            ILOG_WARN("BLE receive: fromRadio frame too large (%d), dropped", (int)m_rx.front().size());
            m_rx.pop_front();
            m_rxPartial = 0;
            pb_size = 0;
        }
        return 0;
    }

    size_t copied = 0;
    while (space_left > 0 && !m_rx.empty()) {
        const std::vector<uint8_t> &frame = m_rx.front();
        size_t remaining = frame.size() - m_rxPartial;
        size_t n = remaining < space_left ? remaining : space_left;
        memcpy(buf + copied, frame.data() + m_rxPartial, n);
        copied += n;
        m_rxPartial += n;
        space_left -= n;
        if (m_rxPartial == frame.size()) {
            m_rx.pop_front();
            m_rxPartial = 0;
        }
    }
    return copied;
}

void BluetoothClient::push_rx(std::vector<uint8_t> &&frame)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t total = 0;
    for (const auto &f : m_rx)
        total += f.size();
    if (total + frame.size() > RX_LIMIT) {
        ILOG_WARN("BLE rx ring full, dropping %d bytes", (int)frame.size());
        return;
    }
    m_rx.push_back(std::move(frame));
}

void BluetoothClient::task_entry(void *arg)
{
    static_cast<BluetoothClient *>(arg)->worker_loop();
}

void BluetoothClient::phase(ConnectionStatus status, const char *detail)
{
    if (m_lastPhase != detail) {
        m_lastPhase = detail;
        ILOG_INFO("BLE state: %s", detail);
    }
    setConnectionStatus(status, detail);
}

void BluetoothClient::worker_loop(void)
{
    while (!m_shutdown) {
        if (m_sleeping) {
            if (m_linkUp)
                teardown_link(false);
            if (!m_workerIdle.exchange(true))
                ILOG_INFO("BLE worker parked");
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }
        m_workerIdle = false;
        if (!m_want) {
            if (m_linkUp)
                teardown_link(false);
            phase(eDisconnected, "disconnected");
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        if (!m_linkUp) {
            if ((int32_t)(millis() - m_retryAt) >= 0)
                attempt_link();
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }
        serve_link();
        if (m_linkUp)
            teardown_link(m_want && !m_shutdown && !m_sleeping);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (m_linkUp)
        teardown_link(false);
    m_taskDone = true;
    vTaskDelete(nullptr);
}

void BluetoothClient::attempt_link(void)
{
    std::string name;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        name = m_name;
    }

    phase(eConnecting, "scanning");
    BLEScan::Results results = BLE.getScan().startBlocking(SCAN_CHUNK_MS);
    const BLEAdvertisedDevice *match = nullptr;
    for (const BLEAdvertisedDevice &d : results) {
        String dn = d.getName();
        bool hit = name.empty() ? dn.startsWith("Meshtastic_") : dn.equals(name.c_str());
        if (hit) {
            match = &d;
            break;
        }
    }
    if (!match) {
        m_retryAt = millis() + SCAN_CHUNK_MS;
        return;
    }

    BLEAdvertisedDevice dev = *match;
    ILOG_INFO("BLE peer %s %s found", dev.getName().c_str(), dev.getAddress().toString().c_str());

    phase(eConnecting, "connecting");
    BLEClient client = BLE.createClient();
    if (!client) {
        m_retryAt = millis() + RETRY_MS;
        return;
    }
    m_dropReason = -1;
    client.onDisconnect([this](BLEClient &, const BLEConnInfo &, uint8_t reason) { m_dropReason = (int)reason; });

    bool paired = false;
    if (!client.connect(dev, CONNECT_TIMEOUT_MS)) {
        ILOG_WARN("BLE connect failed");
    } else {
        phase(eConnecting, "pairing");
        BLE.getSecurity().setStaticPassKey([this]() {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_passkey;
        }());
        if (!client.secureConnection()) {
            ILOG_WARN("BLE pairing failed");
        } else {
            paired = true;
        }
    }

    if (paired && client.discoverServices()) {
        BLERemoteService svc = client.getService(BLEUUID(PHONEAPI_SERVICE_UUID));
        if (svc) {
            m_toRadio = svc.getCharacteristic(BLEUUID(TORADIO_UUID));
            m_fromRadio = svc.getCharacteristic(BLEUUID(FROMRADIO_UUID));
            m_fromNum = svc.getCharacteristic(BLEUUID(FROMNUM_UUID));
        }
        if (!m_toRadio || !m_fromRadio) {
            ILOG_WARN("BLE PhoneAPI characteristics not found");
            paired = false;
        }
    } else if (paired) {
        ILOG_WARN("BLE service discovery failed");
        paired = false;
    }

    if (paired) {
        if (m_fromNum) {
            m_fromNum.subscribe(BLESubscription::Notify, [this](BLERemoteCharacteristic &, const uint8_t *, size_t, BLESubscription) {
                m_doorbell = true;
            });
        }
        m_client = client;
        m_linkUp = true;
        m_consecutiveFails = 0;
        ILOG_INFO("BLE GATT ready: mtu=%d fromNum=%s", client.getMTU(), m_fromNum ? "subscribed" : "unavailable");
        phase(eConnected, "connected");
        return;
    }

    if (client.isConnected())
        client.disconnect();
    m_toRadio = BLERemoteCharacteristic();
    m_fromRadio = BLERemoteCharacteristic();
    m_fromNum = BLERemoteCharacteristic();

    m_consecutiveFails = m_consecutiveFails < MAX_CONSECUTIVE_FAILS ? m_consecutiveFails + 1 : MAX_CONSECUTIVE_FAILS;
    m_retryAt = millis() + (m_consecutiveFails >= MAX_CONSECUTIVE_FAILS ? RETRY_FAIL_MS : RETRY_MS);
    phase(eConnecting, "connecting");
}

void BluetoothClient::serve_link(void)
{
    uint32_t lastPoll = 0;
    uint8_t rbuf[517];

    while (!m_shutdown && !m_sleeping && m_want && m_linkUp && m_client.isConnected()) {
        if (m_doorbell.exchange(false) || millis() - lastPoll >= POLL_MS) {
            lastPoll = millis();
            for (int i = 0; i < 16; i++) {
                size_t n = m_fromRadio.readValue(rbuf, sizeof(rbuf));
                if (n == 0)
                    break;
                std::vector<uint8_t> frame;
                frame.reserve(MT_HEADER_SIZE + n);
                frame.push_back(0x94);
                frame.push_back(0xC3);
                frame.push_back((uint8_t)(n >> 8));
                frame.push_back((uint8_t)(n & 0xFF));
                frame.insert(frame.end(), rbuf, rbuf + n);
                push_rx(std::move(frame));
            }
        }

        std::vector<uint8_t> tx;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_tx.empty()) {
                tx = std::move(m_tx.front());
                m_tx.pop_front();
            }
        }
        if (!tx.empty()) {
            BTStatus st = m_toRadio.writeValue(tx.data(), tx.size(), true);
            if (!st)
                ILOG_WARN("BLE toRadio write failed (%s)", st.toString());
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void BluetoothClient::teardown_link(bool wanted)
{
    m_doorbell = false;
    if (m_client) {
        if (m_client.isConnected())
            m_client.disconnect();
        m_client.resetCallbacks();
        m_client = BLEClient();
    }
    m_toRadio = BLERemoteCharacteristic();
    m_fromRadio = BLERemoteCharacteristic();
    m_fromNum = BLERemoteCharacteristic();
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_rx.clear();
        m_tx.clear();
        m_rxPartial = 0;
    }
    m_linkUp = false;

    int reason = m_dropReason.exchange(-1);
    ILOG_INFO("BLE link down (reason %d)", reason);

    if (wanted) {
        m_consecutiveFails = m_consecutiveFails < MAX_CONSECUTIVE_FAILS ? m_consecutiveFails + 1 : MAX_CONSECUTIVE_FAILS;
        m_retryAt = millis() + (m_consecutiveFails >= MAX_CONSECUTIVE_FAILS ? RETRY_FAIL_MS : RETRY_MS);
        phase(eConnecting, "reconnecting");
    } else {
        m_consecutiveFails = 0;
        m_retryAt = 0;
        phase(eDisconnected, "disconnected");
    }
}

bool BluetoothClient::sleep(int16_t pin)
{
    ILOG_INFO("BLE sleep request: parking worker");
    m_sleeping = true;
    // let the worker drop the link first; a scan chunk takes up to SCAN_CHUNK_MS
    uint32_t deadline = millis() + 1500;
    while (!m_workerIdle && (int32_t)(millis() - deadline) < 0)
        vTaskDelay(pdMS_TO_TICKS(20));
    if (!m_workerIdle)
        ILOG_WARN("BLE worker busy at sleep request");

    // the BLE controller does not survive light sleep - tear it down and restart below
    stop_stack();
    bool wokeByPin = SerialClient::sleep(pin);
    start_stack();

    m_retryAt = 0;
    m_consecutiveFails = 0;
    m_sleeping = false;
    ILOG_INFO("BLE resumed after light sleep (wakeByPin=%d), reconnecting", wokeByPin);
    return wokeByPin;
}

#endif // ARCH_ESP32
