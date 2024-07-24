add_subdirectory(assembler)
add_subdirectory(linker)
add_subdirectory(utilities)

add_library(core INTERFACE)
target_link_libraries(core 
    INTERFACE 
        assembler 
        linker
        utilities
)