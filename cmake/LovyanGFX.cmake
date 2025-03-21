message(STATUS "Fetching LovyanGFX ...")
FetchContent_Declare(
    LovyanGFX
    GIT_REPOSITORY "https://github.com/lovyan03/LovyanGFX"
    GIT_TAG  1.2.0
)
FetchContent_MakeAvailable(LovyanGFX)
include_directories(${lovyangfx_SOURCE_DIR}/src)

# Add the LovyanGFX library
file(GLOB_RECURSE LOVYANGFX_SOURCES ${lovyangfx_SOURCE_DIR}/src/*.cpp)
add_library(LovyanGFX STATIC ${LOVYANGFX_SOURCES})
target_include_directories(LovyanGFX PUBLIC ${lovyangfx_SOURCE_DIR}/src)