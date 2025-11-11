add_library(core
    STATIC
        system.cpp
        assemblycontext.cpp
        jasm.cpp
)

target_link_libraries(core 
    PRIVATE
        libs
        extensions
)
