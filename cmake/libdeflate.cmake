message(STATUS "Fetching libdeflate ...")
set(LIBDEFLATE_BUILD_STATIC_LIB ON CACHE BOOL "Build the static library" FORCE)
set(LIBDEFLATE_BUILD_SHARED_LIB OFF CACHE BOOL "Build the shared library" FORCE)
set(LIBDEFLATE_COMPRESSION_SUPPORT OFF CACHE BOOL "Support compression" FORCE)
set(LIBDEFLATE_DECOMPRESSION_SUPPORT ON CACHE BOOL "Support decompression" FORCE)
set(LIBDEFLATE_ZLIB_SUPPORT OFF CACHE BOOL "Support the zlib format" FORCE)
set(LIBDEFLATE_GZIP_SUPPORT ON CACHE BOOL "Support the gzip format" FORCE)
set(LIBDEFLATE_FREESTANDING OFF CACHE BOOL "Build a freestanding library" FORCE)
set(LIBDEFLATE_BUILD_GZIP OFF CACHE BOOL "Build the libdeflate-gzip program" FORCE)
set(LIBDEFLATE_BUILD_TESTS OFF CACHE BOOL "Build the test programs" FORCE)

FetchContent_Declare(
    libdeflate
    GIT_REPOSITORY "https://github.com/mverch67/libdeflate.git"
    GIT_TAG  "7291dd18ed11df673cce8e33b7f432a1273f1945"
)
FetchContent_MakeAvailable(libdeflate)
include_directories(${libdeflate_SOURCE_DIR}/lib)

file(GLOB_RECURSE libdeflate_SOURCES ${libdeflate_SOURCE_DIR}/lib/*.c)
add_library(libdeflate STATIC ${libdeflate_SOURCES})
target_include_directories(libdeflate PUBLIC ${libdeflate_SOURCE_DIR})
target_include_directories(libdeflate PUBLIC ${libdeflate_SOURCE_DIR}/lib)
