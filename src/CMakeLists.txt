add_subdirectory(bytemode)
add_subdirectory(extensions)
add_subdirectory(core)

add_library(src INTERFACE)

if(TOOLCHAIN_MODE)
    target_sources(src
        INTERFACE
            $<TARGET_OBJECTS:core>
            $<TARGET_OBJECTS:extensions>
    )
endif(TOOLCHAIN_MODE)

target_link_libraries(src
    INTERFACE
        core
        bytemode
        extensions
)
