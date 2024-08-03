add_executable(tests
        ../src/assemblycontext.cpp

        asmcontext.cpp

        extensions.cpp

        assembler/modeflags.cpp
)

set_target_properties(tests
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin//${OUTPUT_PATH}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_PATH}/"
)

target_include_directories(tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/
)

target_link_libraries(tests
    PRIVATE
        libs
        core
)
