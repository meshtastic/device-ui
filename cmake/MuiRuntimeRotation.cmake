# Dual-layout build support for MUI_RUNTIME_ROTATION.
#
# Links both perpendicular generated layouts into one binary so the screen
# rotation becomes a runtime setting. The primary tree (this build's VIEW_*)
# keeps its symbols and supplies every shared asset; the secondary tree is
# compiled with its colliding globals renamed via a generated forced include.
#
# mui_runtime_rotation_sources(<sources_var>) rewrites the caller's source list.
# mui_runtime_rotation_configure(<target>)    applies the target settings.

set(MUI_DUAL_VIEWS ui_320x240 ui_240x320)
set(MUI_GLUE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tools/generated/dual)
set(MUI_GENERATOR ${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_dual_ui.py)

function(mui_runtime_rotation_sources sources_var)
    list(GET MUI_DUAL_VIEWS 0 _first)
    list(GET MUI_DUAL_VIEWS 1 _second)
    if(GENERATED_VIEW STREQUAL _first)
        set(_primary ${_first})
        set(_secondary ${_second})
    elseif(GENERATED_VIEW STREQUAL _second)
        set(_primary ${_second})
        set(_secondary ${_first})
    else()
        message(FATAL_ERROR
            "MUI_RUNTIME_ROTATION supports only ${_first} and ${_second}, not '${GENERATED_VIEW}': "
            "it links the two perpendicular layouts and no other generated tree has a counterpart.")
    endif()

    find_package(Python COMPONENTS Interpreter REQUIRED)
    execute_process(COMMAND ${Python_EXECUTABLE} ${MUI_GENERATOR} --check ${_primary}
                    RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
    if(NOT _rc EQUAL 0)
        message(FATAL_ERROR "MUI_RUNTIME_ROTATION: ${_out}${_err}")
    endif()

    # The build reads the source inventory from the manifest so it cannot drift
    # from the generator.
    file(STRINGS ${MUI_GLUE_DIR}/manifest.txt _manifest)
    set(_secondary_names "")
    set(_glue_names "")
    foreach(_line IN LISTS _manifest)
        if(_line MATCHES "^core_source (.+)$")
            list(APPEND _secondary_names ${CMAKE_MATCH_1})
        elseif(_line MATCHES "^secondary_asset ${_secondary} (.+)$")
            # only the secondary tree's unique assets need compiling
            list(APPEND _secondary_names ${CMAKE_MATCH_1})
        elseif(_line MATCHES "^glue_source (.+)$")
            list(APPEND _glue_names ${CMAKE_MATCH_1})
        endif()
    endforeach()

    # Drop every generated tree from the catch-all glob, then add each tree
    # exactly once. Without this both trees enter the target and their
    # identically named globals collide.
    set(_sources ${${sources_var}})
    list(FILTER _sources EXCLUDE REGEX "/generated/ui_[0-9]+x[0-9]+/")

    file(GLOB_RECURSE _primary_sources ${CMAKE_CURRENT_SOURCE_DIR}/generated/${_primary}/*)
    list(APPEND _sources ${_primary_sources})

    set(_secondary_sources "")
    foreach(_name IN LISTS _secondary_names)
        set(_path ${CMAKE_CURRENT_SOURCE_DIR}/generated/${_secondary}/${_name})
        if(EXISTS ${_path})
            list(APPEND _secondary_sources ${_path})
        endif()
    endforeach()

    # from the manifest, not a glob: a stale .c left in the directory must not
    # silently enter the build
    set(_glue_sources "")
    foreach(_name IN LISTS _glue_names)
        list(APPEND _glue_sources ${MUI_GLUE_DIR}/${_name})
    endforeach()
    list(APPEND _sources ${_secondary_sources} ${_glue_sources})

    # The secondary tree and the secondary-side bridge must see the renames and
    # the secondary headers; the primary-side bridge must see the primary tree
    # untouched.
    set(_rename_flags
        -include ${MUI_GLUE_DIR}/ui_secondary_rename.h
        -I${CMAKE_CURRENT_SOURCE_DIR}/generated/${_secondary})
    set_source_files_properties(
        ${_secondary_sources} ${MUI_GLUE_DIR}/bridge_secondary.c
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTIES COMPILE_OPTIONS "${_rename_flags}")

    # C++ sources referencing generated styles dispatch them to the active tree.
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/source/graphics/TFT/Themes.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/graphics/TFT/TFTView_320x240.cpp
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTIES COMPILE_OPTIONS "-include;${MUI_GLUE_DIR}/style_routing.h")

    set(MUI_PRIMARY_VIEW ${_primary} PARENT_SCOPE)
    set(${sources_var} ${_sources} PARENT_SCOPE)
endfunction()

function(mui_runtime_rotation_configure target)
    target_compile_definitions(${target} PUBLIC MUI_RUNTIME_ROTATION)
    target_include_directories(${target} PUBLIC ${MUI_GLUE_DIR})
endfunction()
