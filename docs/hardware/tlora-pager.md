# LilyGO T-LoRa Pager

The Pager uses the shared `VIEW_320x240` landscape interface at its actual
480 × 222 display resolution. Select `DISPLAY_SET_RESOLUTION` with the existing
`LGFX_TLORA_PAGER` driver; `VIEW_480x222` does not provide the generated screens
required for the complete interface.

The Pager layout adjustment fits the six navigation buttons, initial setup and
signal scanner into the shorter display. Focus and edit outlines are drawn
inside controls, including dynamically created rows, so parent clipping does
not cut off the outline after LVGL scrolls a control into view.

## Inputs

`T_LORA_PAGER` selects the TCA8418 keyboard at I²C address `0x34` on `Wire`
(SDA 3, SCL 2). The firmware must initialize the bus and board power before MUI.
The driver polls the hardware event FIFO and preserves press/release state for
LVGL; it does not require the keyboard interrupt on GPIO 6.

- Shift and Sym can be held or tapped before a character. A tapped modifier
  expires after 1.5 seconds.
- Enter submits; Backspace deletes.
- Sym + Enter moves focus forward; Shift + Sym + Enter moves it backward.
- Sym + Backspace sends Escape.
- Sym + Space cycles the keyboard backlight through off, low and high when
  `KB_BL_PIN=46` is defined.

The rotary encoder uses GPIO 40/41, with its active-low center switch on GPIO 7:

```ini
-D INPUTDRIVER_ENCODER_TYPE=1
-D INPUTDRIVER_ENCODER_UP=40
-D INPUTDRIVER_ENCODER_DOWN=41
-D INPUTDRIVER_ENCODER_BTN=7
-D ENABLE_ROTARY_PULLUP
-D ENABLE_BUTTON_PULLUP
-D ROTARY_BUXTRONICS
```

The board target must provide
[`mverch67/RotaryEncoder` at `da958a2`](https://github.com/mverch67/RotaryEncoder/tree/da958a21389cbcd485989705df602a33e092dd88),
already supplied by Meshtastic's Pager firmware variant. This is the library
whose header is `RotaryEncoder.h` and whose decoder method is `process()`.
No additional keyboard library is required.

Both encoder edges feed that decoder; detents accumulate until LVGL consumes
them. The switch uses a 20 ms debounce and retains its held state for long-press
and editing behavior. `CONFIG_ARDUINO_ISR_IRAM` must remain disabled: the decoder
and its table reside in flash, so the implementation uses ordinary Arduino GPIO
interrupts. Enabling IRAM interrupts is rejected at compile time.

## Validation scope

An integrated firmware build booted MUI on one physical Pager, where rotary
navigation was exercised. That does not establish complete keyboard,
radio-variant, SD-card, sleep/wake or peripheral compatibility. No automated
tests were run. The existing display driver, display backlight and shared SPI
configuration are unchanged.
