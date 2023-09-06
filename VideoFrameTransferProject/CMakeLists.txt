cmake_minimum_required(VERSION 3.8)

project(VideoFrameTransfer C CXX)

# Assuming the common.cmake is located at the same path as in the original. Adjust if needed.
include(cmake/common.cmake)

# Proto file
get_filename_component(video_proto "protos/video_service.proto" ABSOLUTE)
get_filename_component(video_proto_path "${video_proto}" PATH)

# Generated sources
set(video_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/video_service.pb.cc")
set(video_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/video_service.pb.h")
set(video_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/video_service.grpc.pb.cc")
set(video_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/video_service.grpc.pb.h")
add_custom_command(
      OUTPUT "${video_proto_srcs}" "${video_proto_hdrs}" "${video_grpc_srcs}" "${video_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${video_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${video_proto}"
      DEPENDS "${video_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# video_grpc_proto library
add_library(video_grpc_proto
  ${video_grpc_srcs}
  ${video_grpc_hdrs}
  ${video_proto_srcs}
  ${video_proto_hdrs})
target_link_libraries(video_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Targets: VideoClient and VideoServer
foreach(_target
  VideoClient VideoServer  
  )
  add_executable(${_target} "${_target}.cpp" JQueue.cpp)
  target_link_libraries(${_target}
    video_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    ${OpenCV_LIBS})
endforeach()
