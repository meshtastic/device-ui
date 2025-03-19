message(STATUS "Fetching protobufs ...")
FetchContent_Declare(
    Protobuf
    GIT_REPOSITORY "https://github.com/meshtastic/protobufs"
    GIT_TAG  v2.6.2
)
FetchContent_MakeAvailable(Protobuf)
