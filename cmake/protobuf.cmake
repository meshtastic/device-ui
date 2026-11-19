message(STATUS "Fetching protobufs ...")
FetchContent_Declare(
    Protobuf
    GIT_REPOSITORY "https://github.com/meshtastic/protobufs"
    # renovate: datasource=github-tags packageName=meshtastic/protobufs
    GIT_TAG  v2.8.3
)
FetchContent_MakeAvailable(Protobuf)
