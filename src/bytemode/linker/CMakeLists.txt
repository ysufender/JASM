if(TOOLCHAIN_MODE)
    add_library(bytelinker
        STATIC
            linker.cpp
    )
else()
    add_library(bytelinker
        OBJECT
            linker.cpp
    )
endif(TOOLCHAIN_MODE)

target_link_libraries(bytelinker PRIVATE libs)
