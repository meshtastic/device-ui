message(STATUS "Fetching LVGL ...")
add_compile_definitions(LV_LVGL_H_INCLUDE_SIMPLE)
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
add_compile_definitions(LV_COMP_CONF_INCLUDE_SIMPLE)
add_compile_definitions(LV_BUILD_TEST=0)
add_compile_definitions(LV_USE_LIBINPUT=1)
add_compile_definitions(LV_LIBINPUT_XKB=1)
FetchContent_Declare(lvgl 
                     GIT_REPOSITORY https://github.com/lvgl/lvgl.git
                     GIT_TAG  "9c043167685fc08fbcd30ddf2c285ea1089be82d"
)
FetchContent_MakeAvailable(lvgl)
target_include_directories(lvgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${lvgl_SOURCE_DIR}/src)
