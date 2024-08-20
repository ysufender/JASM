add_library(bytelinker STATIC linker.cpp)

target_link_libraries(bytelinker PRIVATE libs)
