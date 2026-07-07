message(STATUS "Fetching nanopb ...")
#set(nanopb_BUILD_GENERATOR "OFF")
#set(nanopb_BUILD_RUNTIME "OFF")
FetchContent_Declare(
    nanopb
    GIT_REPOSITORY https://github.com/nanopb/nanopb.git
    GIT_TAG 0.4.9.1
)
FetchContent_MakeAvailable(nanopb)

# --- Begin grpc-tools Check ---
# Re-run find_package to expose the cached Python interpreter variables globally
find_package(Python COMPONENTS Interpreter QUIET)
if(Python_EXECUTABLE)
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import grpc_tools"
        RESULT_VARIABLE GRPC_TOOLS_STATUS
        OUTPUT_QUIET
        ERROR_QUIET
    )

    if(NOT GRPC_TOOLS_STATUS EQUAL 0)
        message(FATAL_ERROR
            "=================================================================\n"
            " ERROR: Python module 'grpc-tools' is missing!\n"
            " This is required by Nanopb to compile your .proto files.\n"
            " Please install it using one of the following commands:\n"
            "   👉 System package: sudo apt install python3-grpc-tools\n"
            "   👉 Pip package:    pip install grpc-tools\n"
            "=================================================================\n"
        )
    else()
        message(STATUS "Found grpc-tools - handled by ${Python_EXECUTABLE}")
    endif()
else()
    message(FATAL_ERROR "Python interpreter was not found (required for Nanopb).")
endif()
# --- End grpc-tools Check ---

# Set paths for protobuf files and generated files
set(PROTO_FILES_DIR ${protobuf_SOURCE_DIR}/meshtastic)
set(GENERATED_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR}/mesh/generated)

message("-- CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
message("-- PROTO_FILES_DIR=${PROTO_FILES_DIR}")
message("-- GENERATED_FILES_DIR=${GENERATED_FILES_DIR}")

# Create the output directory if it does not exist
file(MAKE_DIRECTORY ${GENERATED_FILES_DIR}/meshtastic)

# Create a custom command to generate C++ files from .proto files
file(GLOB PROTO_FILES ${PROTO_FILES_DIR}/*.proto)
set(GENERATED_FILES)
foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_FILE_NAME ${PROTO_FILE} NAME_WE)
    add_custom_command(
        OUTPUT ${GENERATED_FILES_DIR}/meshtastic/${PROTO_FILE_NAME}.pb.cpp ${GENERATED_FILES_DIR}/${PROTO_FILE_NAME}.pb.h
        COMMAND ${CMAKE_COMMAND} -E env PYTHONWARNINGS=ignore::UserWarning
                ${nanopb_SOURCE_DIR}/generator/protoc 
                --experimental_allow_proto3_optional 
                --nanopb_out=-S.cpp:${GENERATED_FILES_DIR} 
                -I=${protobuf_SOURCE_DIR} 
                -I=${PROTO_FILES_DIR} 
                ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        WORKING_DIRECTORY ${protobuf_SOURCE_DIR}
        COMMENT "Generating C++ files from ${PROTO_FILE}"
    )
    list(APPEND GENERATED_SOURCES ${GENERATED_FILES_DIR}/meshtastic/${PROTO_FILE_NAME}.pb.cpp)
    list(APPEND GENERATED_HEADERS ${GENERATED_FILES_DIR}/meshtastic/${PROTO_FILE_NAME}.pb.h)
endforeach()

# Add the generated files to the project
add_library(Protobufs ${GENERATED_SOURCES})
target_include_directories(Protobufs PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${GENERATED_FILES_DIR} ${nanopb_SOURCE_DIR})
