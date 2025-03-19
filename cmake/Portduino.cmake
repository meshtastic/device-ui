# Include Portduino native framework
message(STATUS "Fetching Portduino ...")
FetchContent_Declare(
    Portduino
    GIT_REPOSITORY "https://github.com/mverch67/framework-portduino"
    GIT_TAG  "ce0ed7e9c62f3082b30ee9a5c369f30c1e55194a"
)
FetchContent_MakeAvailable(Portduino)
include_directories(${portduino_SOURCE_DIR}/cores/portduino)
include_directories(${portduino_SOURCE_DIR}/cores/portduino/FS)
include_directories(${portduino_SOURCE_DIR}/cores/arduino)
include_directories(${portduino_SOURCE_DIR}/libraries/SPI/src)
include_directories(${portduino_SOURCE_DIR}/libraries/Wire/src)
include_directories(${portduino_SOURCE_DIR}/ArduinoCore-API/api)

# Specify source files for Portduino
file(GLOB_RECURSE PORTDUINO_SOURCES
    ${portduino_SOURCE_DIR}/cores/portduino/*.c*
    ${portduino_SOURCE_DIR}/cores/portduino/linux/*.c*
    ${portduino_SOURCE_DIR}/cores/portduino/linux/gpio/*.c*
    ${portduino_SOURCE_DIR}/cores/portduino/FS/*.c*
    ${portduino_SOURCE_DIR}/cores/portduino/simulated/*.c*
    ${portduino_SOURCE_DIR}/cores/arduino/*.c*
    ${portduino_SOURCE_DIR}/ArduinoCore-API/api/*.c*
)
