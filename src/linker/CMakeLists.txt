add_library(linker STATIC linker.cpp)

target_include_directories(linker 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
)