cmake_minimum_required(VERSION 3.0)

project(vector)

add_library(math INTERFACE)
target_include_directories(math INTERFACE src)

add_executable(vector
    src/test/main.cpp
    src/test/Conformance.cpp
    src/test/Conformance.h
    src/test/Performance.cpp
    src/test/Performance.h
)

target_link_libraries(vector math)

add_executable(trace
    src/trace/Trace.cpp
    src/trace/Trace.h
    src/trace/Frustum.h

    src/trace/Image.cpp
    src/trace/Image.h
    src/trace/Scene.h
    src/trace/Color.h
)

target_link_libraries(trace math)
