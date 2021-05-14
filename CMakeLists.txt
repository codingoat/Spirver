cmake_minimum_required(VERSION 3.12)

add_library(Spirver 
		Spirver.cpp 
		Spirver.h
		Spirver.inl
		SpirverAstAnalyzer.cpp
		SpirverAstAnalyzer.h
		)

target_include_directories(Spirver PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/dependencies/glsl-opt/include"
        "${PROJECT_SOURCE_DIR}/dependencies/glslang/include"
        "${PROJECT_SOURCE_DIR}/dependencies/OGLPack/Include"
        "${PROJECT_SOURCE_DIR}/dependencies/spirv-cross/include"
        "${PROJECT_SOURCE_DIR}/dependencies/spirv-tools/include"
        "${OPENGL_INCLUDE_DIRS}"
        )

IF(WIN32)
        list(APPEND SPIRVER_LIB_DIRS
                ${PROJECT_SOURCE_DIR}/dependencies/glslang/lib/
                ${PROJECT_SOURCE_DIR}/dependencies/spirv-cross/lib/
                ${PROJECT_SOURCE_DIR}/dependencies/spirv-tools/lib/
                ${PROJECT_SOURCE_DIR}/dependencies/glsl-opt/lib/
        )
ELSE()
        list(APPEND SPIRVER_LIB_DIRS
                debug ${PROJECT_SOURCE_DIR}/dependencies/glslang/lib/debug/
                debug ${PROJECT_SOURCE_DIR}/dependencies/spirv-cross/lib/debug/
                debug ${PROJECT_SOURCE_DIR}/dependencies/glsl-opt/lib/debug/
                debug ${PROJECT_SOURCE_DIR}/dependencies/spirv-tools/lib/debug
                optimized ${PROJECT_SOURCE_DIR}/dependencies/glslang/lib/release/
                optimized ${PROJECT_SOURCE_DIR}/dependencies/spirv-cross/lib/release/
                optimized ${PROJECT_SOURCE_DIR}/dependencies/glsl-opt/lib/release/
                optimized ${PROJECT_SOURCE_DIR}/dependencies/spirv-tools/lib/release
        )
ENDIF()

target_link_directories(Spirver PUBLIC ${SPIRVER_LIB_DIRS})

list(APPEND SPIRVER_LIBS
        glslang
        SPIRV
        MachineIndependent
        OGLCompiler
        OSDependent
        GenericCodeGen
        SPIRV-Tools-opt
        SPIRV-Tools
        spirv-cross-core
        spirv-cross-glsl
        glsl_optimizer
        )

if(NOT WIN32)
        list(APPEND SPIRVER_LIBS
                glcpp-library
                mesa
                pthread
                )
endif()

target_link_libraries(Spirver
        ${SPIRVER_LIBS}
        )