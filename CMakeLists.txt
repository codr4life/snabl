cmake_minimum_required(VERSION 3.0)

project(snabl)

set(CMAKE_CXX_COMPILER g++)

add_compile_options(-std=c++17 -Wall -Werror -O2 -g)

file(GLOB_RECURSE src src/snabl/*.cpp)

add_library(libsnabl STATIC ${src})
target_include_directories(libsnabl PUBLIC src/)

set_target_properties(libsnabl PROPERTIES PREFIX "")

add_executable(snabl ${src} src/tests.cpp src/main.cpp)
target_include_directories(snabl PUBLIC src/)
target_link_libraries(snabl pthread)

file(GLOB inc src/snabl/*.hpp)
install(FILES ${inc} DESTINATION include/snabl)

file(GLOB inc src/snabl/libs/*.hpp)
install(FILES ${inc} DESTINATION include/snabl/libs)

file(GLOB inc src/snabl/types/*.hpp)
install(FILES ${inc} DESTINATION include/snabl/types)

install(FILES build/libsnabl.a DESTINATION lib)
install(FILES build/snabl DESTINATION bin PERMISSIONS WORLD_EXECUTE)
