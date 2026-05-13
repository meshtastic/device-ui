#ifdef INPUTDRIVER_MATRIX_TYPE

#include "input/KeyMatrixInputDriver.h"
#include "Arduino.h"
#include "input/policy/DefaultInputPolicyFactory.h"
#include "input/policy/IUICommandDispatcher.h"
#include "input/policy/InputContextState.h"
#include "input/policy/InputPipeline.h"
#include "input/policy/InputSourceRegistry.h"
#include "util/ILog.h"
#include <memory>
#include <vector>

// PICOmputer S3 keyboard matrix
#if INPUTDRIVER_MATRIX_TYPE == 1

constexpr byte keys_cols[] = {44, 47, 17, 15, 13, 41};
constexpr byte keys_rows[] = {12, 16, 42, 18, 14, 7};

constexpr unsigned char KeyMap[3][sizeof(keys_rows)][sizeof(keys_cols)] = {
    {{' ', '.', 'm', 'n', 'b', 0x12},  // down
     {0x0a, 'l', 'k', 'j', 'h', 0x14}, // left
     {'p', 'o', 'i', 'u', 'y', 0x11},  // up
     {0x08, 'z', 'x', 'c', 'v', 0x13}, // right
     {'a', 's', 'd', 'f', 'g', 0x1b},
     {'q', 'w', 'e', 'r', 't', 0x1a}},
    {                                  // SHIFT
     {'_', ',', 'M', 'N', 'B', 0x22},  // pgdown (shift+down)
     {0x0d, 'L', 'K', 'J', 'H', 0x03}, // end (shift+left)
     {'P', 'O', 'I', 'U', 'Y', 0x21},  // pgup (shift+up)
     {0x7f, 'Z', 'X', 'C', 'V', 0x02}, // home (shift+right)
     {'A', 'S', 'D', 'F', 'G', 0x09},
     {'Q', 'W', 'E', 'R', 'T', 0x1a}},
    {// SHIFT-SHIFT
     {':', ';', '>', '<', '"', '{'},
     {'~', '-', '*', '&', '+', '['},
     {'0', '9', '8', '7', '6', '}'},
     {'=', '(', ')', '?', '/', ']'},
     {'!', '@', '#', '$', '%', '\\'},
     {'1', '2', '3', '4', '5', 0x1a}}};
#endif

namespace
{

class NullUICommandDispatcher : public input_policy::IUICommandDispatcher
{
  public:
    void dispatch(input_policy::UICommand, const input_policy::CommandPayload &) override {}
};

std::shared_ptr<input_policy::InputPipeline> matrixPipeline;
std::shared_ptr<input_policy::IInputContextProvider> contextProvider;
std::shared_ptr<input_policy::IUICommandDispatcher> commandDispatcher;

const input_policy::InputCapabilities matrixCapabilities = {
    true,  // hasArrowKeys
    false, // hasTabKey
    true,  // hasHomeKey
    true,  // hasEndKey
    true,  // hasPgUpPgDownKeys
    true,  // hasCancelKey
    true,  // hasEnterKey
    true,  // hasModifiers
    true,  // supportsLongPress
    true,  // supportsRepeat
    true   // supportsTextEntry
};

} // namespace

KeyMatrixInputDriver::KeyMatrixInputDriver(void) {}

void KeyMatrixInputDriver::init(void)
{
    for (byte i = 0; i < sizeof(keys_rows); i++) {
        pinMode(keys_rows[i], OUTPUT);
        digitalWrite(keys_rows[i], HIGH);
    }
    for (byte i = 0; i < sizeof(keys_cols); i++) {
        pinMode(keys_cols[i], INPUT_PULLUP);
    }

    keyboard = lv_indev_create();
    lv_indev_set_type(keyboard, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keyboard, keyboard_read);

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
        commandDispatcher = std::make_shared<NullUICommandDispatcher>();
    }
    if (!matrixPipeline) {
        // Use factory to build pipeline with capability-driven policy composition
        input_policy::InputSourceRegistry registry;
        input_policy::DefaultInputPolicyFactory factory;
        auto result = factory.build(registry, contextProvider, commandDispatcher);

        matrixPipeline =
            std::make_shared<input_policy::InputPipeline>(result.bindingResolver, contextProvider, commandDispatcher);
        matrixPipeline->setPolicyChain(std::move(result.chain));
    }
}

/******************************************************************
    LV_KEY_NEXT: Focus on the next object
    LV_KEY_PREV: Focus on the previous object
    LV_KEY_ENTER: Triggers LV_EVENT_PRESSED, LV_EVENT_CLICKED, or LV_EVENT_LONG_PRESSED etc. events
    LV_KEY_UP: Increase value or move upwards
    LV_KEY_DOWN: Decrease value or move downwards
    LV_KEY_RIGHT: Increase value or move to the right
    LV_KEY_LEFT: Decrease value or move to the left
    LV_KEY_ESC: Close or exit (E.g. close a Drop down list)
    LV_KEY_DEL: Delete (E.g. a character on the right in a Text area)
    LV_KEY_BACKSPACE: Delete a character on the left (E.g. in a Text area)
    LV_KEY_HOME: Go to the beginning/top (E.g. in a Text area)
    LV_KEY_END: Go to the end (E.g. in a Text area)

    LV_KEY_UP        = 17,  // 0x11
    LV_KEY_DOWN      = 18,  // 0x12
    LV_KEY_RIGHT     = 19,  // 0x13
    LV_KEY_LEFT      = 20,  // 0x14
    LV_KEY_ESC       = 27,  // 0x1B
    LV_KEY_DEL       = 127, // 0x7F
    LV_KEY_BACKSPACE = 8,   // 0x08
    LV_KEY_ENTER     = 10,  // 0x0A, '\n'
    LV_KEY_HOME      = 2,   // 0x02, STX
    LV_KEY_END       = 3,   // 0x03, ETX
    KEY_PAGE_UP      = 33,  // 0x21
    KEY_PAGE_DOWN    = 34,  // 0x22
*******************************************************************/

void KeyMatrixInputDriver::keyboard_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static int shift = 0;
    static uint32_t prevkey = 0;

    data->key = 0;
    data->state = LV_INDEV_STATE_RELEASED;

    if (INPUTDRIVER_MATRIX_TYPE == 1) {
        // scan for keypresses
        for (byte i = 0; i < sizeof(keys_rows); i++) {
            digitalWrite(keys_rows[i], LOW);
            for (byte j = 0; j < sizeof(keys_cols); j++) {
                if (digitalRead(keys_cols[j]) == LOW) {
                    data->key = (uint32_t)KeyMap[shift][i][j];
                    break;
                }
            }
            digitalWrite(keys_rows[i], HIGH);
        }

        // suppress repeating key, only repeat five times/s
        // the enter key is an exception for LONG_PRESSED monitoring
        static uint32_t lastPressed = millis();
        if (data->key != 0 && (data->key == LV_KEY_ENTER || (millis() > lastPressed + 200))) {
            lastPressed = millis();
            prevkey = data->key;

            switch (data->key) {
            case 0x1a: // Shift
                if (++shift > 2) {
                    shift = 0;
                }
                data->key = 0;
                return; // don't process shift as key input
            default:
                break;
            }
            data->state = LV_INDEV_STATE_PRESSED;
            ILOG_DEBUG("Key 0x%x pressed", data->key);
        } else {
            if (prevkey != 0) {
                data->state = LV_INDEV_STATE_RELEASED;
                data->key = prevkey; // must provide released key here!
                // ILOG_DEBUG("Key 0x%x released", prevkey);
                prevkey = 0;
            }
        }
    }

    if (data->key != 0 && matrixPipeline) {
        input_policy::InputEvent event{};
        event.sourceId = "matrix";
        event.rawKeyCode = data->key;
        event.resolvedKeyCode = data->key;
        event.pressKind =
            (data->state == LV_INDEV_STATE_PRESSED) ? input_policy::PressKind::Press : input_policy::PressKind::Release;
        event.timestampMs = millis();

        ILOG_DEBUG("[KeyMatrix] Raw event: key=0x%x state=%s", data->key,
                   data->state == LV_INDEV_STATE_PRESSED ? "PRESSED" : "RELEASED");

        std::vector<input_policy::InputEvent> output;
        bool forward = matrixPipeline->process(event, matrixCapabilities, output);
        if (!forward || output.empty()) {
            ILOG_DEBUG("[KeyMatrix] Pipeline consumed event, not forwarding");
            data->key = 0;
            data->state = LV_INDEV_STATE_RELEASED;
            return;
        }

        const auto &outEvent = output.front();
        uint32_t outKey = outEvent.resolvedKeyCode != 0 ? outEvent.resolvedKeyCode : outEvent.rawKeyCode;
        if (outKey == 0) {
            ILOG_DEBUG("[KeyMatrix] No output key from pipeline");
            data->key = 0;
            data->state = LV_INDEV_STATE_RELEASED;
            return;
        }

        ILOG_DEBUG("[KeyMatrix] Pipeline output: key=0x%x state=%s (remapped from 0x%x)", outKey,
                   outEvent.pressKind == input_policy::PressKind::Press ? "PRESSED" : "RELEASED", data->key);
        data->key = outKey;
    }
}

#endif