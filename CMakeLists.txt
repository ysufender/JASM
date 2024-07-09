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

add_subdirectory(src)
add_subdirectory(lib)

add_executable(JASM src/main.cpp)

configure_file(
    ${CMAKE_SOURCE_DIR}/include/JASMConfig.hpp.in
    ${CMAKE_BINARY_DIR}/include/JASMConfig.hpp
)

target_include_directories(JASM 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(JASM PRIVATE core libs)