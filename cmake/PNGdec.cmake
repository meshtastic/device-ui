message(STATUS "Fetching PNGdec ...")
FetchContent_Declare(
    PNGdec
    GIT_REPOSITORY "https://github.com/mverch67/PNGdec"
    GIT_TAG  d88b6fe2ec8d49c2b097a529b34d1d615ca5cf1b
)
FetchContent_MakeAvailable(PNGdec)
include_directories(${pngdec_SOURCE_DIR}/src)

# Add the LovyanGFX library
file(GLOB_RECURSE PNGDEC_SOURCES ${pngdec_SOURCE_DIR}/src/*.cpp)
add_library(PNGdec STATIC ${PNGDEC_SOURCES})
target_include_directories(PNGdec PUBLIC ${pngdec_SOURCE_DIR}/src)