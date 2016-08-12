# Features.cmake

function(SetPlatformFeatures)
    set(SRC_DIR "${PROJECT_SOURCE_DIR}/src/platform")
    set(TMP_DIR "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp")

    message(STATUS "Detecting platform architecture features")

    file(COPY ${SRC_DIR}/Features.cpp DESTINATION ${TMP_DIR})

    if(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
        set(DEFINITIONS "-std=c++11")
    endif()

    try_run(
        RUN_RESULT
        COMPILE_RESULT
        ${CMAKE_BINARY_DIR}
        ${TMP_DIR}/Features.cpp
        CMAKE_FLAGS
            -DCMAKE_SKIP_RPATH:BOOL=${CMAKE_SKIP_RPATH}
            -DINCLUDE_DIRECTORIES:STRING=${SRC_DIR}
        COMPILE_DEFINITIONS
            ${DEFINITIONS}
        COMPILE_OUTPUT_VARIABLE
            COMPILE_OUTPUT_RESULT
        RUN_OUTPUT_VARIABLE
            RUN_OUTPUT_RESULT
    )

    if(NOT ${COMPILE_RESULT})
        message(STATUS "Detecting platform architecture features - failed!")
    else()
        message(STATUS "Detecting platform architecture features - ${RUN_OUTPUT_RESULT}")

        add_definitions("-D_F_BITS=${RUN_RESULT}")

        # Enable enhanced instruction set if available
        if(RUN_OUTPUT_RESULT MATCHES "AVX2")
            set(ARCH "AVX2")
        elseif(RUN_OUTPUT_RESULT MATCHES "AVX")
            set(ARCH "AVX")
        elseif(RUN_OUTPUT_RESULT MATCHES "SSE2")
            set(ARCH "SSE2")
        elseif(RUN_OUTPUT_RESULT MATCHES "SSE")
            set(ARCH "SSE")
        endif()

        if(DEFINED ARCH AND CMAKE_CXX_COMPILER_ID MATCHES MSVC)
            add_compile_options("/arch:${ARCH}")
            message(STATUS "Using enhanced instruction set - ${ARCH}")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU)
            # For GCC/Clang each feature needs to be set explicitly.
            string(REGEX MATCHALL "[^, ]+" ARCH_LIST "${RUN_OUTPUT_RESULT}")
            foreach(ARCH_VAR ${ARCH_LIST})
                string(TOLOWER ${ARCH_VAR} ARCH_LOWER)
                add_compile_options("-m${ARCH_LOWER}")
            endforeach()
        endif()
    endif()
endfunction()
