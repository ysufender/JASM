add_subdirectory(CLIParser)

add_library(libs INTERFACE)

target_link_libraries(libs 
    INTERFACE
        CLIParser
)
