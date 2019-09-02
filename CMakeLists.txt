cmake_minimum_required(VERSION 3.8)
project(surreals)

set(CMAKE_CXX_STANDARD 14)

set(GCC_COVERAGE_COMPILE_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

set(SOURCE_FILES surreals.cpp surreals.h)
add_library(surreals ${SOURCE_FILES})

add_executable(demo-infinite demos/demo-infinite.cpp)
add_executable(demo-finite-mult demos/demo-finite-mult.cpp)
add_executable(demo-finite-genesis-simple demos/demo-finite-genesis-simple.cpp)
add_executable(demo-finite-genesis-full demos/demo-finite-genesis-full.cpp)
add_executable(demo-finite-float2surreal demos/demo-finite-float2surreal.cpp)

target_link_libraries(demo-infinite surreals)
target_link_libraries(demo-finite-mult surreals)
target_link_libraries(demo-finite-genesis-simple surreals)
target_link_libraries(demo-finite-genesis-full surreals)
target_link_libraries(demo-finite-float2surreal surreals)