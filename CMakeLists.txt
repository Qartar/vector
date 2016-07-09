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
