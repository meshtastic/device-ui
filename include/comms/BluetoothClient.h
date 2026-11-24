#pragma once

#include "comms/SerialClient.h"
#include <atomic>
#include <time.h>

#if defined(ARCH_ESP32)
#include <deque>
#include <mutex>
#include <string>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <BLE.h>
#endif

/**
 * @brief PhoneAPI over BLE, built on the SerialClient framing/queue machinery.
 *
 * Uses the arduino-esp32 >= 3.x core BLE API (BLE.h, backend-agnostic: NimBLE
 * on C5/S3, Bluedroid on ESP32). An internal worker task scans, connects,
 * pairs (Keyboard/PassKey, default PIN 123456) and drains the fromRadio
 * characteristic into a byte ring; the base class task_loop picks the frames
 * up here: receive() re-attaches the 4-byte envelope header the base
 * validates, send() strips it again before the raw ToRadio protobuf is handed
 * to the worker. BLE carries raw protobuf, so this class is the translation
 * point between stream framing and GATT.
 */
class BluetoothClient : public SerialClient
{
  public:
    BluetoothClient(void);
    void init(void) override;
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    // drop the BLE link around light sleep; the peer queues FromRadio packets while we are away
    bool sleep(int16_t pin) override;
    virtual ~BluetoothClient();

#if defined(ARCH_ESP32)
    // advertise name to connect to; empty selects the first Meshtastic_* device found
    void setDeviceName(const std::string &name);
    // peer pairing PIN, default 123456
    void setPasskey(uint32_t passkey);
#endif

  protected:
    // low-level send: strips the envelope header and queues the raw ToRadio for the worker
    bool send(const uint8_t *buf, size_t len) override;

    // low-level receive: hands out fromRadio reads re-framed with the envelope header
    size_t receive(uint8_t *buf, size_t space_left) override;

  private:
#if defined(ARCH_ESP32)
    static void task_entry(void *arg);
    void worker_loop(void);
    void attempt_link(void);
    void serve_link(void);
    void teardown_link(bool wanted);
    void push_rx(std::vector<uint8_t> &&frame);
    void start_stack(void);
    void stop_stack(void);
    // setConnectionStatus + one INFO line per phase change
    void phase(ConnectionStatus status, const char *detail);

    BLEClient m_client;
    BLERemoteCharacteristic m_toRadio;
    BLERemoteCharacteristic m_fromRadio;
    BLERemoteCharacteristic m_fromNum;

    std::mutex m_mutex; // guards m_rx, m_tx, m_name, m_passkey
    std::deque<std::vector<uint8_t>> m_rx;
    std::deque<std::vector<uint8_t>> m_tx;
    size_t m_rxPartial = 0; // bytes of m_rx.front() already handed to the base class

    std::string m_name;
    uint32_t m_passkey = 123456;

    std::atomic<bool> m_shutdown{false};
    std::atomic<bool> m_taskDone{false};
    std::atomic<bool> m_want{false};   // user intent, set by connect()/disconnect()
    std::atomic<bool> m_linkUp{false}; // actual GATT link state
    std::atomic<bool> m_doorbell{false};
    std::atomic<bool> m_sleeping{false};  // set while the core is in light sleep
    std::atomic<bool> m_workerIdle{false}; // worker parked, no scan/connection in flight
    std::atomic<int> m_dropReason{-1};

    uint32_t m_retryAt = 0;
    uint8_t m_consecutiveFails = 0;
    std::string m_lastPhase;
    TaskHandle_t m_taskHandle = nullptr;
#endif
};
