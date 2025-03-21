# Set the name of the C and C++ compilers
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)
set(CMAKE_CXX_COMPILER xtensa-esp32-elf-g++)

# Set the path to the compiler tools
set(TOOLCHAIN_PATH "$ENV{HOME}/.platformio/packages/toolchain-xtensa-esp32/bin")

# Set the compiler flags
set(CMAKE_C_FLAGS "-mlongcalls -mtext-section-literals")
set(CMAKE_CXX_FLAGS "-mlongcalls -mtext-section-literals")

# Set the linker flags
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib")

# Set the path to the ESP32 SDK
set(ESP32_SDK_PATH "$ENV{HOME}/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32")

# Include directories for the ESP32 SDK
include_directories(${ESP32_SDK_PATH}/include)
include_directories(${ESP32_SDK_PATH}/include/esp_common)
include_directories(${ESP32_SDK_PATH}/include/esp_common/include)
include_directories(${ESP32_SDK_PATH}/include/esp32)
include_directories(${ESP32_SDK_PATH}/include/heap)
include_directories(${ESP32_SDK_PATH}/include/log)
include_directories(${ESP32_SDK_PATH}/include/newlib)
include_directories(${ESP32_SDK_PATH}/include/soc)
include_directories(${ESP32_SDK_PATH}/include/xtensa)
include_directories(${ESP32_SDK_PATH}/include/xtensa/include)
include_directories(${ESP32_SDK_PATH}/include/freertos/include/esp_additions/freertos)
include_directories(${ESP32_SDK_PATH}/include/freertos/port/xtensa/include)
include_directories(${ESP32_SDK_PATH}/qio_qspi/include)
