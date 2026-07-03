cmake_minimum_required(VERSION 4.0)
project(RayTracerCPP)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC_FILES src/*.h src/*.cpp)

find_package(OpenMP REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

add_executable(RayTracerCPP ${SRC_FILES})

target_include_directories(RayTracerCPP PUBLIC src)

target_link_libraries(RayTracerCPP PRIVATE ${OpenMP_CXX_LIBRARIES} OpenMP::OpenMP_CXX gdi32 user32)
