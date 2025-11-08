add_subdirectory(disassembler)
add_subdirectory(assembler)
add_subdirectory(linker)

add_library(bytemode INTERFACE)

if(TOOLCHAIN_MODE)
    target_sources(bytemode
        INTERFACE
            $<TARGET_OBJECTS:byteassembler>
            $<TARGET_OBJECTS:bytelinker>
            $<TARGET_OBJECTS:bytedisassembler>
    )
endif(TOOLCHAIN_MODE)

target_link_libraries(bytemode
    INTERFACE
    byteassembler
    bytelinker
    bytedisassembler
)
