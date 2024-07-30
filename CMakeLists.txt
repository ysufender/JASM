cmake_minimum_required(VERSION 3.30)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(jasm 
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
set(USE_PIPELINES_OPT FALSE CACHE BOOL "Enable pipelines")
set(TOOLCHAIN_MODE FALSE CACHE BOOL "Enable toolchain mode")
set(JASM_TEST_MODE FALSE CACHE BOOL "Activate the unit tests")

# This is before the include call to prevent it including 
# project headers. Might cause problems.
add_subdirectory(lib)

configure_file(
    ${CMAKE_SOURCE_DIR}/JASMConfig.hpp.in
    ${CMAKE_BINARY_DIR}/include/JASMConfig.hpp
)

add_executable(${CMAKE_PROJECT_NAME} 
    src/jasm.cpp 
    src/assemblycontext.cpp
)

include_directories(${CMAKE_PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/header_only
        ${CMAKE_BINARY_DIR}/include
)

# After the include call so everything includes those above
add_subdirectory(src)


set_target_properties(${CMAKE_PROJECT_NAME}
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)



target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE core libs)
