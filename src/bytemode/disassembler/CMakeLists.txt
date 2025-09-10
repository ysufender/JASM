add_library(bytedisassembler 
    STATIC 
        disassembler.cpp
        instructions.cpp
)

target_link_libraries(bytedisassembler
    PRIVATE
        libs
)
