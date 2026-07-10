message(STATUS "Fetching protobufs ...")
FetchContent_Declare(
    Protobuf
    GIT_REPOSITORY "https://github.com/RCGV1/protobufs-fork"
    # Packet authenticity policy and XEdDSA capability from protobufs#983.
    GIT_TAG  19ab2d2711050cb462f0f57507f75b47bfa6832c
)
FetchContent_MakeAvailable(Protobuf)
