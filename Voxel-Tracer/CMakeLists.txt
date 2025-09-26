cmake_minimum_required(VERSION 3.20)

project(VoxelTracer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(VoxelTracer "src/MainApp.cpp" "src/MainLayer.h" "src/MainLayer.cpp" "src/VoxelCamera.h")

include_directories(VoxelTracer PRIVATE
  ${CMAKE_SOURCE_DIR}/RealEngine/src
  ${CMAKE_SOURCE_DIR}/RealEngine/vendor/ImGui
  src
)

file(COPY assets DESTINATION "${CMAKE_BINARY_DIR}/Voxel-Tracer")

target_link_libraries(VoxelTracer RealEngine)
