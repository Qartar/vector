cmake_minimum_required(VERSION 3.0)

project(vector)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(Features)
SetPlatformFeatures()

if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU)
    add_compile_options(-std=c++11)
endif()

########################################
# vector

add_library(vector STATIC
    src/vector/Default.h
    src/vector/Intrinsic.h
    src/vector/Intersect.h

    src/vector/Vector.cpp

    src/platform/Features.h
)

target_include_directories(vector PUBLIC src src/platform)

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

target_link_libraries(trace vector)

########################################
# trace_test

add_executable(trace_test
    src/trace/Trace.cpp
)

target_link_libraries(trace_test trace)

########################################
# light_app

add_executable(light_app
    src/trace/Light.cpp
)

target_link_libraries(light_app trace)

########################################
# vector_test

add_executable(vector_test
    src/test/main.cpp
    src/test/Conformance.cpp
    src/test/Conformance.h
    src/test/Performance.cpp
    src/test/Performance.h
)

target_link_libraries(vector_test trace vector)
