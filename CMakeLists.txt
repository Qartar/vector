cmake_minimum_required(VERSION 3.0)

project(vector)

########################################
# math

add_library(math INTERFACE)
target_include_directories(math INTERFACE src)

########################################
# trace

add_library(trace STATIC
    src/trace/Trace.h
    src/trace/Frustum.h

    src/trace/Image.cpp
    src/trace/Image.h
    src/trace/Scene.h
    src/trace/Color.h
)

target_link_libraries(trace math)

########################################
# trace_app

add_executable(trace_app
    src/trace/Trace.cpp
)

target_link_libraries(trace_app trace)

########################################
# vector

add_executable(vector
    src/test/main.cpp
    src/test/Conformance.cpp
    src/test/Conformance.h
    src/test/Performance.cpp
    src/test/Performance.h
)

target_link_libraries(vector trace math)
