add_library(libs INTERFACE)

add_subdirectory(CLIParser)

target_link_libraries(libs
    INTERFACE
        CLIParser
)
