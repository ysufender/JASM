if(TOOLCHAIN_MODE)
    add_library(byteassembler
        STATIC
            assembler.cpp
            instructions.cpp
            modeflags.cpp
    )
else()
    add_library(byteassembler
        OBJECT
            assembler.cpp
            instructions.cpp
            modeflags.cpp
    )
endif(TOOLCHAIN_MODE)

target_link_libraries(byteassembler PRIVATE libs)
