if(TOOLCHAIN_MODE)
    add_library(bytedisassembler
        STATIC
            disassembler.cpp
            instructions.cpp
    )
else()
    add_library(bytedisassembler
        OBJECT
            disassembler.cpp
            instructions.cpp
    )
endif(TOOLCHAIN_MODE)

target_link_libraries(bytedisassembler
    PRIVATE
        libs
)
