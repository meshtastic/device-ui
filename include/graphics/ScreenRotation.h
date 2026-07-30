#pragma once

#ifdef MUI_RUNTIME_ROTATION

// Only these two layouts are perpendicular counterparts of each other.
#if !defined(VIEW_320x240) && !defined(VIEW_240x320)
#error "MUI_RUNTIME_ROTATION requires VIEW_320x240 or VIEW_240x320"
#endif

// Those drivers apply a fixed rotation to touch coordinates and would
// desynchronize from a rotated panel.
#ifdef CUSTOM_TOUCH_DRIVER
#error "MUI_RUNTIME_ROTATION is not supported with CUSTOM_TOUCH_DRIVER"
#endif

#include <cstdint>

/**
 * Runtime screen rotation for builds that link both generated UI layouts.
 *
 * The stored value is a quarter-turn count applied on top of the board's
 * compile-time LGFX_ROTATION offset, so value 0 reproduces the board's default.
 * Even values keep the compile-time VIEW_* layout, odd values select the
 * perpendicular one. Applied at boot only: an LVGL layout tree cannot be
 * rebuilt in place once the view has cached its object pointers.
 */
class ScreenRotation
{
  public:
    enum Value : uint8_t {
        Rotation0 = 0,
        Rotation90 = 1,
        Rotation180 = 2,
        Rotation270 = 3,
    };

    /// Supplied by the integrator before the display is created. Out-of-range
    /// values fall back to the build default. Ignored after the first call.
    static void setLoaded(uint8_t raw);
    static void load(void);
    static Value get(void) { return current; }

    /// Odd quarter turns swap the aspect, so they need the perpendicular tree.
    static bool usesSecondaryTree(Value v) { return ((uint8_t)v & 1) != 0; }

    /// Nominal size of the layout, for seeding DisplayDriver. NOT the panel
    /// resolution -- the driver reads that from the rotated panel.
    static uint16_t width(Value v);
    static uint16_t height(Value v);

    /// Value passed to setRotation(); LovyanGFX composes it with the board's
    /// offset_rotation. The quarter turns run opposite to the panel's own
    /// numbering so that a rotation reads upright to the user: hardware-checked
    /// on a 320x240 panel with offset_rotation 3, where upright portrait is
    /// panel rotation 2, i.e. one step back from upright landscape, not one
    /// step on. Parity is preserved, so tree selection is unaffected.
    static uint8_t panelRotation(Value v) { return (uint8_t)((4 - (uint8_t)v) & 3); }

    static bool usesSecondaryTree(void) { return usesSecondaryTree(current); }
    static uint16_t width(void) { return width(current); }
    static uint16_t height(void) { return height(current); }
    static uint8_t panelRotation(void) { return panelRotation(current); }

  private:
    static Value current;
    static bool loaded;
};

#endif // MUI_RUNTIME_ROTATION
