add_subdirectory(byteassembler)

add_library(assembler INTERFACE)

target_link_libraries(assembler
    INTERFACE
        byteassembler
)
