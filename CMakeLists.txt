cmake_minimum_required(VERSION 3.30)

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


if(${TEST_MODE})
    add_library(jasm
        STATIC 
            src/jasm.cpp
            src/assemblycontext.cpp
            src/system.cpp
    )
else()
    add_executable(jasm 
        src/jasm.cpp 
        src/assemblycontext.cpp
        src/system.cpp
    )

    set_target_properties(jasm
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin//${OUTPUT_PATH}/lib"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/"
    )
endif(${TEST_MODE})

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
        core 
)

if(${TEST_MODE})
    add_subdirectory(tests)
endif(${TEST_MODE})
