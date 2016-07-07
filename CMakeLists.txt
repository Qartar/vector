cmake_minimum_required(VERSION 3.0)

project(vector)

add_executable(vector
    main.cpp

    Conformance.cpp
    Conformance.h
    Performance.cpp
    Performance.h

    Intersect.h

    Default.h
    Aligned.h
    Intrinsic.h
)
