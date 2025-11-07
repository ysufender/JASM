if(NOT TOOLCHAIN_MODE)
    add_subdirectory(disassembler)
endif(NOT TOOLCHAIN_MODE)

add_subdirectory(assembler)
add_subdirectory(linker)

add_library(bytemode INTERFACE)

target_link_libraries(bytemode
    INTERFACE
        byteassembler
        bytelinker
)

if(NOT TOOLCHAIN_MODE)
    target_link_libraries(bytemode
        INTERFACE
            bytedisassembler
    )
endif(NOT TOOLCHAIN_MODE)
