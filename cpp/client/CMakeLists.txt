project(grpc-error-details-client)
cmake_minimum_required(VERSION 3.5)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

find_package(Protobuf REQUIRED)
find_package(Grpc REQUIRED)

set(PROTO_SOURCE ${CMAKE_CURRENT_LIST_DIR}/../../proto/example.proto)
protobuf_generate_cpp(EXAMPLE_PROTO_SOURCE EXAMPLE_PROTO_HEADER ${PROTO_SOURCE})
protobuf_grpc_generate_cpp(EXAMPLE_PROTO_GRPC_SOURCE EXAMPLE_PROTO_GRPC_HEADER ${PROTO_SOURCE})

protobuf_generate_cpp(GOOGLE_STATUS_PROTO_SOURCE GOOGLE_STATUS_PROTO_HEADER ${CMAKE_CURRENT_LIST_DIR}/../../proto/googleapis/google/rpc/status.proto)

set(SOURCES main.cpp 
    ${EXAMPLE_PROTO_SOURCE} ${EXAMPLE_PROTO_HEADER}
    ${EXAMPLE_PROTO_GRPC_SOURCE} ${EXAMPLE_PROTO_GRPC_HEADER}
    ${GOOGLE_STATUS_PROTO_HEADER}
)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        ${PROTOBUF_LIBRARY}
        ${GRPC_LIBRARIES}
)
set_target_properties(${PROJECT_NAME} PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR})