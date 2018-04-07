
find_library(GPR_LIBRARY gpr)
find_library(GRPC_LIBRARY grpc)
find_library(GRPC++_LIBRARY grpc++)
find_library(GRPC++_ERROR_DETAILS grpc++_error_details)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

set(GRPC_LIBRARIES 
    ${GPR_LIBRARY}
    ${GRPC_LIBRARY}
    ${GRPC++_LIBRARY}
    ${GRPC++_ERROR_DETAILS}
)

macro(protobuf_grpc_generate_cpp source_name header_name protobuf_file)
    get_filename_component(_protobuf_file_name_we ${protobuf_file} NAME_WE)
    get_filename_component(_protobuf_file_directory ${protobuf_file} DIRECTORY)
    set(${source_name} "${CMAKE_CURRENT_BINARY_DIR}/${_protobuf_file_name_we}.grpc.pb.cc")
    set(${header_name} "${CMAKE_CURRENT_BINARY_DIR}/${_protobuf_file_name_we}.grpc.pb.h")
    add_custom_command(
        OUTPUT ${${source_name}} ${${header_name}}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            -I ${_protobuf_file_directory} 
            --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
            ${protobuf_file})
endmacro()
