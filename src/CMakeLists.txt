add_subdirectory(bytemode)
add_subdirectory(extensions)
add_subdirectory(core)

add_library(src INTERFACE)

target_link_libraries(src 
    INTERFACE 
        bytemode
        extensions
        core
)
