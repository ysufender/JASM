cmake_minimum_required(VERSION 3.30)

project(JASM 
    VERSION 0.1.2
    DESCRIPTION "Assembler and linker for JASM IL"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDART_REQUIRED TRUE)

message(STATUS "Checking CXX compiler.")
if("${CXX_COMPILER_NAME}" STREQUAL "x86_64-w64-mingw32-g++")
    message(STATUS "Using ${CXX_COMPILER_NAME}, linking libstdc++ statically.")
    add_link_options("-static" "-static-libstdc++" "-static-libgcc")
endif("${CXX_COMPILER_NAME}" STREQUAL "x86_64-w64-mingw32-g++")

#
# CLI Arguments 
# 
option(TOOLCHAIN_MODE "Enable toolchain mode" OFF)
option(USE_PIPELINES_OPT "Pipeline support" OFF)
set(OUTPUT_PATH "" CACHE STRING "")

#
# File Configures
#
configure_file(
    ${CMAKE_SOURCE_DIR}/JASMConfig.hpp.in
    ${CMAKE_BINARY_DIR}/include/JASMConfig.hpp
    @ONLY
)

#file(GLOB JASM_FILES CONFIGURE_DEPENDS "tests/script/*.jasm")
#foreach(JASM_FILE ${JASM_FILES})
#    file(RELATIVE_PATH OUT_JASM "${CMAKE_SOURCE_DIR}/tests" ${JASM_FILE})
#    file(COPY ${JASM_FILE} DESTINATION "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/${OUT_JASM}")
#endforeach(JASM_FILE ${JASM_FILES})

if (NOT TOOLCHAIN_MODE)
    add_executable(jasm 
        src/main.cpp
    )
else ()
    message(STATUS "Building in toolchain mode.")
    add_library(jasm
        src/main.cpp
    )
endif (NOT TOOLCHAIN_MODE)

set_target_properties(jasm
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin//${OUTPUT_PATH}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/"
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

# After the include call so everything includes those above
add_subdirectory(lib)
add_subdirectory(src)

target_link_libraries(jasm 
    PUBLIC 
        libs
        src
)
