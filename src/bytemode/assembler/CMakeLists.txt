add_library(byteassembler
    STATIC
        assembler.cpp
        instructions.cpp
        modeflags.cpp
)

target_link_libraries(byteassembler PRIVATE libs)
