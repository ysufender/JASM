add_subdirectory(disassembler)
add_subdirectory(assembler)
add_subdirectory(linker)

add_library(bytemode INTERFACE)

target_link_libraries(bytemode
    INTERFACE
        byteassembler
        bytelinker
        bytedisassembler
)
