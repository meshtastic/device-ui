#ifdef MUI_RUNTIME_ROTATION

#include "graphics/ScreenRotation.h"
#include "screens.h"
#include <doctest/doctest.h>

// The secondary tree is linked under renamed symbols. Referencing one here
// forces the linker to extract its objects: without this a static-library
// build links green while never pulling in the secondary layout at all.
extern "C" {
extern objects_t MUI2_objects;
void mui_bridge_publish_secondary(void);
}

TEST_CASE("both generated layouts are linked")
{
    CHECK(&objects != &MUI2_objects);
    CHECK(mui_bridge_publish_secondary != nullptr);
}

TEST_CASE("panel rotation runs opposite to the panel's own numbering")
{
    CHECK(ScreenRotation::panelRotation(ScreenRotation::Rotation0) == 0);
    CHECK(ScreenRotation::panelRotation(ScreenRotation::Rotation90) == 3);
    CHECK(ScreenRotation::panelRotation(ScreenRotation::Rotation180) == 2);
    CHECK(ScreenRotation::panelRotation(ScreenRotation::Rotation270) == 1);

    // parity must survive, or the tree selection would disagree with the size
    for (uint8_t v = 0; v <= 3; v++)
        CHECK((ScreenRotation::panelRotation((ScreenRotation::Value)v) & 1) == (v & 1));
}

TEST_CASE("odd quarter turns select the perpendicular tree")
{
    CHECK_FALSE(ScreenRotation::usesSecondaryTree(ScreenRotation::Rotation0));
    CHECK(ScreenRotation::usesSecondaryTree(ScreenRotation::Rotation90));
    CHECK_FALSE(ScreenRotation::usesSecondaryTree(ScreenRotation::Rotation180));
    CHECK(ScreenRotation::usesSecondaryTree(ScreenRotation::Rotation270));
}

TEST_CASE("odd quarter turns swap the layout dimensions")
{
    const uint16_t w = ScreenRotation::width(ScreenRotation::Rotation0);
    const uint16_t h = ScreenRotation::height(ScreenRotation::Rotation0);
    CHECK(w != h);

    // 180 keeps the aspect, 90/270 transpose it
    CHECK(ScreenRotation::width(ScreenRotation::Rotation180) == w);
    CHECK(ScreenRotation::height(ScreenRotation::Rotation180) == h);
    CHECK(ScreenRotation::width(ScreenRotation::Rotation90) == h);
    CHECK(ScreenRotation::height(ScreenRotation::Rotation90) == w);
    CHECK(ScreenRotation::width(ScreenRotation::Rotation270) == h);
    CHECK(ScreenRotation::height(ScreenRotation::Rotation270) == w);
}

// The tests above use the pure mapping helpers and never touch the loaded
// state; this is the only one that does, and setLoaded() is deliberately
// one-shot.
TEST_CASE("an out-of-range stored value falls back to the build default")
{
    const ScreenRotation::Value before = ScreenRotation::get();
    ScreenRotation::setLoaded(9);
    CHECK(ScreenRotation::get() == before);

    ScreenRotation::setLoaded(ScreenRotation::Rotation90); // ignored, one-shot
    CHECK(ScreenRotation::get() == before);
}

#endif // MUI_RUNTIME_ROTATION
