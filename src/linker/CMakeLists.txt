add_library(linker STATIC linker.cpp)

target_link_libraries(linker PRIVATE libs)
