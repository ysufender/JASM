cmake_minimum_required(VERSION 3.30)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(JASM 
    VERSION 1.0.0
    DESCRIPTION "Assembler and linker for JASM"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDART_REQUIRED TRUE)

#
# CLI Arguments 
# 
option(TOOLCHAIN_MODE "Enable toolchain mode" OFF)
option(TEST_MODE "Activate the unit tests" OFF)
option(USE_PIPELINES_OPT "Pipeline support" OFF)
set(OUTPUT_PATH "" CACHE STRING "")

configure_file(
    ${CMAKE_SOURCE_DIR}/JASMConfig.hpp.in
    ${CMAKE_BINARY_DIR}/include/JASMConfig.hpp
    @ONLY
)

if(NOT ${TEST_MODE})
    add_executable(jasm 
        src/jasm.cpp 
        src/assemblycontext.cpp
    )
endif(NOT ${TEST_MODE})

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
)

# After the include call so everything includes those above
add_subdirectory(lib)
link_libraries(libs)
add_subdirectory(src)

if(${TEST_MODE})
    add_subdirectory(tests)
endif(${TEST_MODE})

if(NOT ${TEST_MODE})
    set_target_properties(jasm
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin//${OUTPUT_PATH}/lib"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/"
    )

    target_link_libraries(jasm 
        PRIVATE 
            core 
            #libs
    )
endif(NOT ${TEST_MODE})
