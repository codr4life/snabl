cmake_minimum_required(VERSION 3.0)

project(snabl)

set(CMAKE_CXX_COMPILER clang++)

add_compile_options(-std=c++17 -Wall -Werror -O2 -g)

file(GLOB_RECURSE sources src/snabl/*.cpp)

add_library(libsnabl STATIC ${sources})
target_include_directories(libsnabl PUBLIC src/)

set_target_properties(libsnabl PROPERTIES PREFIX "")

add_executable(snabl ${sources} src/tests.cpp src/main.cpp)
target_include_directories(snabl PUBLIC src/)

file(GLOB headers src/snabl/*.hpp)
install(FILES ${headers} DESTINATION include/snabl)

file(GLOB headers src/snabl/libs/*.hpp)
install(FILES ${headers} DESTINATION include/snabl/libs)

file(GLOB headers src/snabl/types/*.hpp)
install(FILES ${headers} DESTINATION include/snabl/types)

install(FILES build/libsnabl.a DESTINATION lib)
install(FILES build/snabl DESTINATION bin PERMISSIONS WORLD_EXECUTE)
