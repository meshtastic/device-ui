message(STATUS "Fetching libdeflate ...")
set(LIBDEFLATE_BUILD_STATIC_LIB "Build the static library" 1)
set(LIBDEFLATE_BUILD_SHARED_LIB "Build the shared library" 0)
set(LIBDEFLATE_COMPRESSION_SUPPORT "Support compression" 0)
set(LIBDEFLATE_DECOMPRESSION_SUPPORT "Support decompression" 1)
set(LIBDEFLATE_ZLIB_SUPPORT "Support the zlib format" 0)
set(LIBDEFLATE_GZIP_SUPPORT "Support the gzip format" 1)
set(LIBDEFLATE_FREESTANDING "Build a freestanding library" 0)
set(LIBDEFLATE_BUILD_GZIP "Build the libdeflate-gzip program" 0)
set(LIBDEFLATE_BUILD_TESTS "Build the test programs" 0)

FetchContent_Declare(
    libdeflate
    GIT_REPOSITORY "https://github.com/mverch67/libdeflate.git"
    GIT_TAG  "ce93eca5c1260da59c347bfebfd271343d55be38"
)
FetchContent_MakeAvailable(libdeflate)
include_directories(${libdeflate_SOURCE_DIR}/lib)

file(GLOB_RECURSE libdeflate_SOURCES ${libdeflate_SOURCE_DIR}/lib/*.c)
add_library(libdeflate STATIC ${libdeflate_SOURCES})
target_include_directories(libdeflate PUBLIC ${libdeflate_SOURCE_DIR})
target_include_directories(libdeflate PUBLIC ${libdeflate_SOURCE_DIR}/lib)