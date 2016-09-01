cmake_minimum_required(VERSION 3.0)

project(vector)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

########################################
# Select default configuration for single-configuration generators

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

########################################
# Compiler specific options

# Clang/C2
if(MSVC AND CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ox")

# Clang/LLVM
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm -lc++")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

# GNU C
elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

# MSVC
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox /Ot")

endif()

########################################
# Feature detection

include(Features)
SetPlatformFeatures()

########################################
# vector

add_library(vector STATIC
    src/vector/Reference.h
    src/vector/Intrinsic.h
    src/vector/Aliased.h
    src/vector/Intersect.h

    src/vector/Vector.cpp

    src/platform/Features.h
    src/platform/Platform.h
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
    src/trace/Light.h
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
    src/trace/Light.h
)

target_link_libraries(light_app trace)

########################################
# vector_test

add_executable(vector_test
    src/test/main.cpp
    src/test/Timer.h
    src/test/Conformance.cpp
    src/test/Conformance.h
    src/test/Performance.cpp
    src/test/Performance.h
)

target_link_libraries(vector_test trace vector)
