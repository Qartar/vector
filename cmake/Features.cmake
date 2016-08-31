# Features.cmake

function(SetPlatformFeatures)
    set(SRC_DIR "${PROJECT_SOURCE_DIR}/src/platform")
    set(TMP_DIR "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp")

    message(STATUS "Detecting platform architecture features")

    file(COPY ${SRC_DIR}/Features.cpp DESTINATION ${TMP_DIR})

    try_run(
        RUN_RESULT
        COMPILE_RESULT
        ${CMAKE_BINARY_DIR}
        ${TMP_DIR}/Features.cpp
        CMAKE_FLAGS
            -DCMAKE_SKIP_RPATH:BOOL=${CMAKE_SKIP_RPATH}
            -DINCLUDE_DIRECTORIES:STRING=${SRC_DIR}
            -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
            -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
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
        endif()
    endif()

    # Use native architecture when compiling with Clang or GCC
    if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU)
        add_compile_options("-march=native")
    endif()
endfunction()
