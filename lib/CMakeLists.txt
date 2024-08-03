add_library(libs INTERFACE)

add_subdirectory(CLIParser)

if(${TEST_MODE})
    add_subdirectory(Catch2)

    target_link_libraries(libs
        INTERFACE
        Catch2::Catch2WithMain
    )
endif(${TEST_MODE})

target_link_libraries(libs 
    INTERFACE
        CLIParser
)
