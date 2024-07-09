add_library(assembler STATIC assembler.cpp)

target_include_directories(assembler 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
)