cmake_minimum_required(VERSION 3.15)

################################################################################
##                             D E V I C E   U I                               #
################################################################################
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/version.txt ver)
project(device_ui VERSION ${ver} HOMEPAGE_URL https://github.com/meshtastic/device-ui LANGUAGES C CXX)

message(STATUS "device-ui version: ${PROJECT_VERSION}")

set(MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code.
                        Tests in tests/*.cpp will still be enabled." ${MAIN_PROJECT})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON) # with newer cmake versions put all find_package in global scope
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(GENERATED_VIEW ${VIEW} "ui_320x240")

message(STATUS "Using C++${CMAKE_CXX_STANDARD}")
message(STATUS "Using Generated View: " ${GENERATED_VIEW})

add_compile_definitions(ARCH_PORTDUINO)
add_compile_definitions(ARDUINO)
add_compile_definitions(VIEW_320x240)
add_compile_definitions(USE_X11=1)

if(ENABLE_DOCTESTS)
    add_compile_definitions(UNIT_TEST)
endif()

include(FetchContent)
include(Portduino)
include(LovyanGFX)
include(LVGL)
include(protobuf)
include(nanopb)
include(Doctest)

# Manually set the Doctest include directory if not set by the Doctest module
if(NOT DEFINED DOCTEST_INCLUDE_DIR)
    set(DOCTEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/path/to/doctest) # Adjust the path as necessary
endif()

file(GLOB_RECURSE sources      source/* generated/* portduino/* locale/* generated/${GENERATED_VIEW}/*)
file(GLOB_RECURSE sources_test tests/*.cpp)

add_library(DeviceUI ${sources})
target_link_libraries(DeviceUI PRIVATE lvgl::lvgl LovyanGFX Portduino Protobufs)
target_compile_options(DeviceUI PUBLIC -Wall -Wfloat-conversion)

target_include_directories(DeviceUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(DeviceUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(DeviceUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/locale)
target_include_directories(DeviceUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/portduino)
target_include_directories(DeviceUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated/${GENERATED_VIEW})
target_include_directories(DeviceUI PRIVATE ${GENERATED_FILES_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${nanopb_SOURCE_DIR})

if(ENABLE_DOCTESTS)
    target_include_directories(DeviceUI PRIVATE ${doctest_SOURCE_DIR})
endif()

#
# Unit Tests
#
if(ENABLE_DOCTESTS)
#    include(CTest)
#    enable_testing()
    add_executable(tests ${sources_test})
    target_link_libraries(tests PRIVATE DeviceUI doctest::doctest lvgl::lvgl LovyanGFX Portduino Protobufs)
    target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    add_test(NAME tests COMMAND tests)
endif()