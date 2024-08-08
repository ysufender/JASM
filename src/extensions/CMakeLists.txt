add_library(extensions 
    STATIC
        stringextensions.cpp
        streamextensions.cpp
        system.cpp
        serialization.cpp
)

target_link_libraries(extensions PRIVATE libs)
