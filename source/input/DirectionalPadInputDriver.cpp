#ifdef INPUTDRIVER_DIRECTIONALPAD_TYPE

#include "input/DirectionalPadInputDriver.h"
#include "Arduino.h"
#include "indev/lv_indev_private.h"
#include "input/policy/DefaultInputPolicyFactory.h"
#include "input/policy/InputContextState.h"
#include "input/policy/InputPipeline.h"
#include "input/policy/InputSourceRegistry.h"
#include "input/policy/UICommandDispatcher.h"
#include "util/ILog.h"

// ---------------------------------------------------------------------------
// Build-flag defaults
// ---------------------------------------------------------------------------
#ifndef INPUTDRIVER_DIRECTIONALPAD_I2C_ADDR
#define INPUTDRIVER_DIRECTIONALPAD_I2C_ADDR 0x22
#endif

#ifndef INPUTDRIVER_DIRECTIONALPAD_WIRE
#define INPUTDRIVER_DIRECTIONALPAD_WIRE Wire
#endif

// Duration (ms) after which holding RETURN emits UICommand 0x101 (go home)
// instead of forwarding LV_KEY_ESC to LVGL.  Must be set before LVGL fires
// its own LV_EVENT_LONG_PRESSED for the ESC key (which is deferred anyway).
#ifndef DIRECTIONALPAD_RETURN_LONG_PRESS_MS
#define DIRECTIONALPAD_RETURN_LONG_PRESS_MS 500
#endif

// ---------------------------------------------------------------------------
// File-scope TCA6424A reader instance
// ---------------------------------------------------------------------------
static Tca6424Pad tca(INPUTDRIVER_DIRECTIONALPAD_I2C_ADDR, INPUTDRIVER_DIRECTIONALPAD_WIRE);

// ---------------------------------------------------------------------------
// Static member definitions
// ---------------------------------------------------------------------------
volatile bool DirectionalPadInputDriver::inputPending = false;
QueueHandle_t DirectionalPadInputDriver::eventQueue = nullptr;
uint8_t DirectionalPadInputDriver::lastPortState = 0xFF; // all bits high = all buttons released
uint32_t DirectionalPadInputDriver::prevKey = 0;

// ---------------------------------------------------------------------------
// for use with input policy processing
// ---------------------------------------------------------------------------
static std::shared_ptr<input_policy::InputPipeline> padPipeline;
static std::shared_ptr<input_policy::IInputContextProvider> contextProvider;
static std::shared_ptr<input_policy::IUICommandDispatcher> commandDispatcher;

const input_policy::InputCapabilities PadCapabilities = {
    true,  // hasArrowKeys
    false, // hasTabKey
    false, // hasHomeKey
    false, // hasEndKey
    false, // hasPgUpPgDownKeys
    true,  // hasCancelKey
    true,  // hasEnterKey
    false, // hasModifiers
    true,  // supportsLongPress
    true,  // supportsRepeat
    false  // supportsTextEntry
};

// ---------------------------------------------------------------------------
// PORT 0 bit index → LV_KEY mapping
// ---------------------------------------------------------------------------
uint32_t DirectionalPadInputDriver::portBitToLvKey(uint8_t bit)
{
    switch (bit) {
    case 0:
        return LV_KEY_UP;
    case 1:
        return LV_KEY_DOWN;
    case 2:
        return LV_KEY_LEFT;
    case 3:
        return LV_KEY_RIGHT;
    case 4:
        return LV_KEY_ENTER; // CONFIRM – LVGL fires long-press events automatically
    case 5:
        return LV_KEY_ESC; // RETURN – long-press event interpreted as UICommand
    default:
        return 0;
    }
}

// ---------------------------------------------------------------------------
// ISR – runs in interrupt context; no I2C access, flag only.
// Placed in IRAM so it is available even when the flash cache is inactive.
// ---------------------------------------------------------------------------
void IRAM_ATTR DirectionalPadInputDriver::intHandler(void)
{
    inputPending = true;
}

// ---------------------------------------------------------------------------
// Constructor / init
// ---------------------------------------------------------------------------
DirectionalPadInputDriver::DirectionalPadInputDriver(void) {}

void DirectionalPadInputDriver::prepareSleep(void)
{
    // detachInterrupt(digitalPinToInterrupt(INPUTDRIVER_DIRECTIONALPAD_INT));
}

void DirectionalPadInputDriver::wakeUp(void)
{
    // attachInterrupt(digitalPinToInterrupt(INPUTDRIVER_DIRECTIONALPAD_INT), intHandler, FALLING);
}

void DirectionalPadInputDriver::init(void)
{
    eventQueue = xQueueCreate(16, sizeof(PadEvent));

#ifdef TCA6424_REQUIRES_INIT
    if (!tca.begin()) {
        ILOG_WARN("DirectionalPadInputDriver: TCA6424 init failed");
    }
#endif

    // Establish a known baseline before any interrupt fires.
    lastPortState = tca.readPort(0);

    // The TCA6424A INT output is active-low open-drain; enable the internal
    // pull-up so the line idles high and only falls when an input changes.
    pinMode(INPUTDRIVER_DIRECTIONALPAD_INT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUTDRIVER_DIRECTIONALPAD_INT), intHandler, FALLING);

    keyboard = lv_indev_create();
    lv_indev_set_type(keyboard, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keyboard, button_read);

    if (!inputGroup) {
        inputGroup = lv_group_create();
        lv_group_set_default(inputGroup);
    }
    lv_indev_set_group(keyboard, inputGroup);

    if (!contextProvider) {
        contextProvider = std::shared_ptr<input_policy::IInputContextProvider>(&input_policy::InputContextState::instance(),
                                                                               [](input_policy::IInputContextProvider *) {});
    }

    if (!commandDispatcher) {
        commandDispatcher = std::shared_ptr<input_policy::IUICommandDispatcher>(&input_policy::UICommandDispatcher::instance(),
                                                                                [](input_policy::IUICommandDispatcher *) {});
    }

    if (!padPipeline) {
        input_policy::InputSourceRegistry registry;
        input_policy::DefaultInputPolicyFactory factory;
        auto result = factory.build(registry, contextProvider, commandDispatcher);

        padPipeline = std::make_shared<input_policy::InputPipeline>(result.bindingResolver, contextProvider, commandDispatcher);
        padPipeline->setPolicyChain(std::move(result.chain));
    }

    ILOG_DEBUG("DirectionalPadInputDriver: initialised on INT pin %d, I2C addr 0x%02x", INPUTDRIVER_DIRECTIONALPAD_INT,
               INPUTDRIVER_DIRECTIONALPAD_I2C_ADDR);
}

// ---------------------------------------------------------------------------
// LVGL read callback – called by LVGL's input timer on every poll cycle.
//
// Design:
//  1. When the TCA6424A INT fires, the ISR sets inputPending.  Here we do the
//     actual I2C read, diff against lastPortState, and enqueue one PadEvent
//     per changed bit.
//  2. Each poll cycle we drain at most one event from the queue so LVGL
//     processes transitions sequentially.
//  3. If no queued event but a key is still physically held (reflected in
//     lastPortState), we report PRESSED so LVGL's long-press timer keeps
//     running.  No INT fires while the key state is unchanged.
//  4. When the queue is empty and no key is held, we send a final RELEASED for
//     the last active key so LVGL can finalise the click/release event chain.
// ---------------------------------------------------------------------------
void DirectionalPadInputDriver::button_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    // Driver-side state for RETURN long-press detection.
    // RETURN (LV_KEY_ESC) is intercepted here and never forwarded to LVGL until
    // we know whether it is a short or long press:
    //   short press  → ESC PRESSED + RELEASED injected after release
    //   long press   → UICommand 0x101 dispatched via pipeline, ESC suppressed
    static uint32_t returnPressTime = 0;
    static bool returnActive = false;
    static bool returnLongSent = false;

    // --- Step 1: process pending interrupt -----------------------------------
    if (inputPending) {
        inputPending = false; // clear before reading to avoid losing a second edge
        uint8_t current = tca.readPort(0);
        if (current != 0xFF) { // 0xFF is the I2C-error sentinel; skip if bus failed
            uint8_t changed = current ^ lastPortState;
            for (uint8_t bit = 0; bit < 6; bit++) {
                if (changed & (1u << bit)) {
                    uint32_t key = portBitToLvKey(bit);
                    if (key != 0) {
                        // Active-low: bit = 0 means pressed, bit = 1 means released.
                        bool pressed = !(current & (1u << bit));
                        PadEvent ev{key, pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED};
                        xQueueSend(eventQueue, &ev, 0);
                    }
                }
            }
            lastPortState = current;
        }
    }

    // --- Step 2: deliver one queued transition event through the pipeline ----
    PadEvent e;
    if (xQueueReceive(eventQueue, &e, 0) == pdTRUE) {
        // RETURN key: intercept for driver-side long-press detection.
        // We defer forwarding to LVGL so the initial ESC cancel does not fire
        // during the hold window; the decision is made on release or at threshold.
        if (e.key == LV_KEY_ESC) {
            if (e.state == LV_INDEV_STATE_PRESSED) {
                returnActive = true;
                returnPressTime = millis();
                returnLongSent = false;
                data->key = 0;
                data->state = LV_INDEV_STATE_RELEASED;
                prevKey = 0;
                return;
            }
            // Released
            returnActive = false;
            if (returnLongSent) {
                // Long press was already dispatched; send Release for 0x101.
                if (padPipeline) {
                    input_policy::InputEvent ev{};
                    ev.sourceId = "directionalPad";
                    ev.rawKeyCode = 0x101;
                    ev.resolvedKeyCode = 0x101;
                    ev.pressKind = input_policy::PressKind::Release;
                    ev.timestampMs = millis();
                    std::vector<input_policy::InputEvent> out;
                    padPipeline->process(ev, PadCapabilities, out);
                }
                returnLongSent = false;
                data->key = 0;
                data->state = LV_INDEV_STATE_RELEASED;
                prevKey = 0;
                return;
            }
            // Short press: inject ESC PRESSED now; prevKey mechanism
            // delivers ESC RELEASED on the next poll cycle (step 4).
            e.state = LV_INDEV_STATE_PRESSED;
            // fall through to pipeline processing with ESC PRESSED
        }

        // Normal pipeline processing
        data->key = e.key;
        data->state = e.state;

        if (e.key != 0 && padPipeline) {
            input_policy::InputEvent event{};
            event.sourceId = "directionalPad";
            event.rawKeyCode = e.key;
            event.resolvedKeyCode = e.key;
            event.pressKind =
                (e.state == LV_INDEV_STATE_PRESSED) ? input_policy::PressKind::Press : input_policy::PressKind::Release;
            event.timestampMs = millis();

            std::vector<input_policy::InputEvent> output;
            bool forward = padPipeline->process(event, PadCapabilities, output);
            if (!forward || output.empty()) {
                ILOG_DEBUG("[DirectionalPad] Pipeline consumed event, not forwarding");
                data->key = 0;
                data->state = LV_INDEV_STATE_RELEASED;
                prevKey = 0;
                return;
            }

            const auto &outEvent = output.front();
            uint32_t outKey = outEvent.resolvedKeyCode != 0 ? outEvent.resolvedKeyCode : outEvent.rawKeyCode;
            if (outKey == 0) {
                ILOG_DEBUG("[DirectionalPad] No output key from pipeline");
                data->key = 0;
                data->state = LV_INDEV_STATE_RELEASED;
                prevKey = 0;
                return;
            }
            data->state =
                (outEvent.pressKind == input_policy::PressKind::Release) ? LV_INDEV_STATE_RELEASED : LV_INDEV_STATE_PRESSED;
            data->key = outKey;
        }

        // Track the pipeline-resolved key so steps 3 and 4 report it consistently.
        prevKey = (data->state == LV_INDEV_STATE_PRESSED) ? data->key : 0;
        return;
    }

    // Long press detection for the RETURN key.
    // Fires while the key is physically held (step 1 updated lastPortState).
    // Since LVGL never received an ESC PRESSED event, there is no race with
    // LVGL's own long-press timer for ESC.
    if (returnActive && !returnLongSent && (millis() - returnPressTime) >= DIRECTIONALPAD_RETURN_LONG_PRESS_MS) {
        returnLongSent = true;
        ILOG_DEBUG("[DirectionalPad] RETURN long press UICommand 0x101");
        if (padPipeline) {
            input_policy::InputEvent ev{};
            ev.sourceId = "directionalPad";
            ev.rawKeyCode = 0x101;
            ev.resolvedKeyCode = 0x101;
            ev.pressKind = input_policy::PressKind::LongPress;
            ev.timestampMs = millis();
            std::vector<input_policy::InputEvent> out;
            padPipeline->process(ev, PadCapabilities, out);
        }
        data->key = 0;
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    // --- Step 3: sustain PRESSED while a key is physically held -------------
    // The INT only fires on *changes*, not while a key is held steady, so we
    // keep reporting PRESSED so LVGL's long-press timer accumulates time.
    // Use prevKey (already pipeline-resolved in step 2) to avoid re-processing
    // the event through the pipeline on every poll cycle.
    uint8_t heldBits = ~lastPortState & 0x3Fu; // bits 0-5 only
    if (heldBits != 0 && prevKey != 0) {
        data->key = prevKey;
        data->state = LV_INDEV_STATE_PRESSED;
        return;
    }

    // --- Step 4: deliver final RELEASED for the last active key -------------
    if (prevKey != 0) {
        data->key = prevKey;
        data->state = LV_INDEV_STATE_RELEASED;
        prevKey = 0;
        return;
    }

    data->state = LV_INDEV_STATE_RELEASED;
}

#endif // INPUTDRIVER_DIRECTIONALPAD_INT
