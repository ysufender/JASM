cmake_minimum_required(VERSION 3.30)

project(JASM 
    VERSION 1.0.0
    DESCRIPTION "Assembler and linker for JASM"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDART_REQUIRED TRUE)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    #add_compile_options("-O0")
endif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

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


add_executable(jasm 
    src/main.cpp
)

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
