cmake_minimum_required(VERSION 3.0)

project(vector)

########################################
# math

add_library(math STATIC
    src/math/Default.h
    src/math/Aligned.h
    src/math/Intrinsic.h
    src/math/Intersect.h

    src/math/Math.cpp
)

target_include_directories(math PUBLIC src)

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
# light_app

add_executable(light_app
    src/trace/Light.cpp
)

target_link_libraries(light_app trace)

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
