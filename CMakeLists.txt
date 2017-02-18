cmake_minimum_required(VERSION 2.8)
project(MicroMon)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

add_executable(micromon micromon.cpp)
target_compile_options(micromon PRIVATE -g;-O0;-Wall;-std=c++11)
target_include_directories(micromon PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(micromon m ${SDL2_LIBRARIES})
install(TARGETS micromon RUNTIME DESTINATION bin)
