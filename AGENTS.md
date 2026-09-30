
# AI Coding Instructions: device-ui

## Project and Build Context

This repository is the Meshtastic device UI C++17 library. It supports embedded Arduino-based devices and Linux/Portduino; this checkout does not define PlatformIO environments (`platformio.ini`/`variants`). For firmware builds, use the consuming firmware repository's target and build instructions rather than assuming a local PlatformIO target here.

The standalone native build uses CMake, Portduino, LVGL, and doctest. From this repository root:

```sh
cmake -B build -S .
cmake --build build -j
ctest --test-dir build --output-on-failure
```

`ENABLE_DOCTESTS` defaults on when this repository is configured as the top-level CMake project. The native build is useful for host-side behavior and regression tests, but it does not replace compiling or exercising the relevant hardware target.

## Repository Layout

- `include/` contains public interfaces and headers.
- `source/` contains the UI library implementations, including graphics, input, communications, filesystem, and utility code.
- `src/` contains Meshtastic protobuf/constants integration and the library entry point.
- `portduino/` contains native-platform support.
- `drivers/` contains device-specific support code.
- `generated/` contains generated LVGL/EEZ Studio view sources, organized by view dimensions.
- `tests/` contains the CMake/doctest test executable's test cases.
- `cmake/` defines external dependency setup; CMake fetches several dependencies with `FetchContent`.

## Architecture and Platform Boundaries

- C++17 is required (`CMAKE_CXX_STANDARD` is 17 and extensions are disabled).
- Follow existing subsystem boundaries: public contracts generally live under `include/`, with implementations under `source/`. Reuse established interfaces and factories, such as `IClientBase`, `IFileSystem`, `DisplayDriverFactory`, and `ViewFactory`; do not add a parallel abstraction without a concrete need.
- New shared UI behavior should work across all supported devices. Do not scatter `#ifdef DEVICE_X` checks through shared logic to handle individual boards.
- Put device-specific behavior behind the narrowest suitable existing abstraction. When a new behavior genuinely differs by device, prefer an abstract interface with device-specific implementations selected by a factory, following patterns such as `DisplayDriver` and `DisplayDriverFactory`.
- Keep device-specific compile-time checks at platform or factory registration boundaries when required by the build or hardware APIs. Follow the existing conditional-registration patterns; do not add an interface or factory when the existing abstraction already handles the variation.
- Use RAII and the ownership style already used by the surrounding code. Consider embedded memory and lifetime costs when adding allocations, but do not replace appropriate dynamic collections with fixed-size storage by default.
- Native tests compile against the library and Portduino/LVGL support. They are host tests, not hardware tests; avoid requiring attached peripherals or assuming native execution proves hardware behavior. Use the existing doctest conventions (`TEST_CASE`, `SUBCASE`, and `CHECK`) and test the narrow behavior changed.

## Generated UI Sources

Do not hand-edit files under `generated/`; changes there are overwritten by UI generation. Update the corresponding UI authoring source and regenerate the affected view using the project's established workflow. Keep view-specific references scoped to the matching `generated/ui_<dimensions>/` directory.

The CMake build currently selects `ui_320x240`. In the PlatformIO library integration, `extra_script.py` selects a generated view from the consuming build's `VIEW_*` definition and adds `portduino/` when `ARCH_PORTDUINO` is defined. When changing generated-view or platform integration, preserve both build paths.

`CMakeLists.txt` uses configure-time source globs. Rerun `cmake -B build -S .` after adding or removing source files so the build picks up the changes.

## Adding a TFT Device

Device-specific UI settings are compiler flags in the device's `platformio.ini`, either in the consuming firmware repository's variant or in a standalone PlatformIO integration. Keep pin and panel configuration with that target instead of hard-coding it into shared UI code.

1. If LovyanGFX supports both the TFT panel and touch controller, use the generic driver. Define `LGFX_DRIVER_TEMPLATE`, set `LGFX_DRIVER=LGFX_GENERIC`, and set `GFX_DRIVER_INC=\"graphics/LGFX/LGFX_GENERIC.h\"`. Add the panel, touch, bus, GPIO, rotation, and dimensions flags required by that board. See `crowpanel_small_esp32s3_base` in `firmware/variants/esp32s3/elecrow_panel/platformio.ini`.
2. If the generic LovyanGFX configuration cannot represent the hardware, add a board-specific LovyanGFX device definition under `include/graphics/LGFX/`, then set `LGFX_DRIVER` and `GFX_DRIVER_INC` to that class and header. Include the header in `source/graphics/driver/DisplayDriverFactory.cpp` under the corresponding board define so the driver class is available to the factory. See `LGFX_ELECROW70.h` and `crowpanel_large_esp32s3_base` in the same firmware variant.
3. Select a UI view independently from the panel's physical resolution. `320x240` is the CMake default and the common TFT layout, but the library may also have specific view implementations for other dimensions; check `source/graphics/common/ViewFactory.cpp` and the matching `generated/ui_<dimensions>/` assets before choosing a `VIEW_*` define. For a panel whose native dimensions differ from the selected layout, `DISPLAY_SET_RESOLUTION` makes the LVGL display use the resolution reported by the LovyanGFX driver; it does not select or create a different UI view.

