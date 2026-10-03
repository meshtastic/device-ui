message(STATUS "Fetching protobufs ...")
FetchContent_Declare(
    Protobuf
    GIT_REPOSITORY "https://github.com/meshtastic/protobufs"
    GIT_TAG  v2.8.0
)
FetchContent_MakeAvailable(Protobuf)
