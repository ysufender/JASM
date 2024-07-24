add_library(utilities 
    STATIC
        stringextensions.cpp
        streamextensions.cpp
        system.cpp
)

target_include_directories(utilities
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
)