add_subdirectory(CLIParser)
add_subdirectory(Catch2)

add_library(libs INTERFACE)

target_link_libraries(libs 
    INTERFACE
        CLIParser
)

if (${TEST_MODE})
    target_link_libraries(libs
        INTERFACE
            Catch2::Catch2WithMain
    )
endif()
