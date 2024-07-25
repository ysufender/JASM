add_library(extensions 
    STATIC
        stringextensions.cpp
        streamextensions.cpp
        system.cpp
)

target_include_directories(extensions
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
)