if(ENABLE_DOCTESTS)
    message(STATUS "Fetching doctest ...")
    add_definitions(-DENABLE_DOCTEST_IN_LIBRARY)
    FetchContent_Declare(
        DocTest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        # renovate: datasource=github-tags packageName=onqtam/doctest
        GIT_TAG v2.5.2
    )

    FetchContent_MakeAvailable(DocTest)
    include_directories(${DOCTEST_INCLUDE_DIR})
    message("-- adding include_directories(${DOCTEST_INCLUDE_DIR})")
endif()
