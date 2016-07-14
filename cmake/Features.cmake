# Features.cmake

function(SetPlatformFeatures)
    set(SRC_DIR "${PROJECT_SOURCE_DIR}/src/platform")
    set(TMP_DIR "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp")

    file(COPY ${SRC_DIR}/Features.cpp DESTINATION ${TMP_DIR})

    try_run(
        RUN_RESULT
        COMPILE_RESULT
        ${CMAKE_BINARY_DIR}
        ${TMP_DIR}/Features.cpp
        CMAKE_FLAGS
            -DCMAKE_SKIP_RPATH:BOOL=${CMAKE_SKIP_RPATH}
            -DINCLUDE_DIRECTORIES:STRING=${SRC_DIR}
        COMPILE_OUTPUT_VARIABLE
            COMPILE_OUTPUT_RESULT
        RUN_OUTPUT_VARIABLE
            RUN_OUTPUT_RESULT
    )

    if(${COMPILE_RESULT})
        add_definitions("-D_F_BITS=${RUN_RESULT}")
        message(STATUS "Platform features detected: ${RUN_OUTPUT_RESULT}")
    else()
        message(WARNING "Platform feature detection failed!")
    endif()
endfunction()
