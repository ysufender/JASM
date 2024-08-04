add_executable(tests
        asmcontext.cpp

        extensions/string.cpp
        extensions/stream.cpp
        extensions/system.cpp
        extensions/serialization.cpp

        assembler/modeflags.cpp
        assembler/instructions.cpp
        assembler/assembler.cpp
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
        jasm
)

file(GLOB JASM_FILES CONFIGURE_DEPENDS "script/*.jasm")

foreach(JASM_FILE ${JASM_FILES})
    file(RELATIVE_PATH OUT_JASM ${CMAKE_CURRENT_SOURCE_DIR} ${JASM_FILE})
    file(COPY ${JASM_FILE} DESTINATION "${CMAKE_BINARY_DIR}/bin/Test/${OUT_JASM}")
    #configure_file(${JASM_FILE} "bin/Test/${OUT_JASM}")
endforeach(JASM_FILE ${JASM_FILES})
