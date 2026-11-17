// LGFXDriver::touchpad_read in graphics/driver/LGFXDriver.h must report pointer
// coordinates independently of LVGL's gesture metadata. LVGL 9.6 stopped copying
// the primary gesture point, leaving single touches at the previous position.
// These cases also pin centroid panning and release suppression after a pinch.
#include "graphics/driver/LGFXDriver.h"

#include <doctest/doctest.h>

#if LV_USE_GESTURE_RECOGNITION && !defined(CUSTOM_TOUCH_DRIVER)
#include <algorithm>
#include <initializer_list>
#include <vector>

namespace
{
struct FakeTouchDevice {
    std::vector<lgfx::touch_point_t> points;

    uint8_t getTouch(lgfx::touch_point_t *out, uint8_t capacity)
    {
        auto count = std::min<size_t>(points.size(), capacity);
        std::copy_n(points.begin(), count, out);
        return count;
    }
};

FakeTouchDevice fakeTouch;

struct TouchReader : LGFXDriver<FakeTouchDevice> {
    using LGFXDriver<FakeTouchDevice>::touchpad_read;
};
} // namespace

template <> FakeTouchDevice *LGFXDriver<FakeTouchDevice>::lgfx = &fakeTouch;

TEST_CASE("gesture-enabled touch callback reports single touches and centroid panning")
{
    if (!lv_is_initialized())
        lv_init();
    auto *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_pinch_up_threshold(indev, 1.5f);
    lv_indev_set_pinch_down_threshold(indev, 0.5f);
    lv_indev_set_rotation_rad_threshold(indev, 0.8f);
    indev->recognizers[LV_INDEV_GESTURE_TWO_FINGERS_SWIPE].recog_fn = nullptr;

    auto read = [&](std::initializer_list<lgfx::touch_point_t> points) {
        fakeTouch.points = points;
        lv_indev_data_t data = {};
        TouchReader::touchpad_read(indev, &data);
        return data;
    };
    auto check = [&](std::initializer_list<lgfx::touch_point_t> points, lv_indev_state_t state, int x, int y) {
        auto data = read(points);
        CHECK(data.state == state);
        CHECK(data.point.x == x);
        CHECK(data.point.y == y);
    };
    auto point = [](int x, int y, int id) {
        lgfx::touch_point_t p = {};
        p.x = x;
        p.y = y;
        p.id = id;
        return p;
    };

    read({});
    check({point(30, 40, 0)}, LV_INDEV_STATE_PRESSED, 30, 40);
    check({point(45, 55, 0)}, LV_INDEV_STATE_PRESSED, 45, 55);
    check({}, LV_INDEV_STATE_RELEASED, 45, 55);

    check({point(50, 60, 0)}, LV_INDEV_STATE_PRESSED, 50, 60);
    check({point(50, 60, 0), point(150, 60, 1)}, LV_INDEV_STATE_PRESSED, 50, 60);
    check({point(60, 70, 0), point(160, 70, 1)}, LV_INDEV_STATE_PRESSED, 60, 70);
    auto pinch = read({point(10, 70, 0), point(210, 70, 1)});
    CHECK(pinch.gesture_type[LV_INDEV_GESTURE_PINCH] == LV_INDEV_GESTURE_PINCH);
    CHECK(pinch.gesture_data[LV_INDEV_GESTURE_PINCH] != nullptr);
    CHECK(pinch.point.x == 60);
    CHECK(pinch.point.y == 70);
    check({point(210, 70, 1)}, LV_INDEV_STATE_RELEASED, 60, 70);
    check({point(220, 80, 1)}, LV_INDEV_STATE_RELEASED, 60, 70);
    check({}, LV_INDEV_STATE_RELEASED, 60, 70);
    check({point(100, 110, 1)}, LV_INDEV_STATE_PRESSED, 100, 110);
    check({}, LV_INDEV_STATE_RELEASED, 100, 110);

    check({point(40, 50, 0), point(140, 50, 1)}, LV_INDEV_STATE_PRESSED, 90, 50);
    check({}, LV_INDEV_STATE_RELEASED, 90, 50);
    lv_indev_delete(indev);
}
#endif
