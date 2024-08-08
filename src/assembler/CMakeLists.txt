add_library(assembler 
    STATIC 
        assembler.cpp
        instructions.cpp
        modeflags.cpp
)

target_link_libraries(assembler PRIVATE libs)
