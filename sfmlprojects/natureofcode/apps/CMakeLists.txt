cmake_minimum_required(VERSION 3.16)
project(NatureOfCode LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(NTOC_TARGET natureofcode)

add_executable(${NTOC_TARGET} natureofcode.cpp)
# target_link_libraries(${NTOC_TARGET} PRIVATE sfml-graphics PRIVATE MyLibrary::MyLibrary)
# target_link_libraries(${NTOC_TARGET} PRIVATE sfml-graphics wwsfml)
target_link_libraries(${NTOC_TARGET} PRIVATE wwsfml)
target_compile_features(${NTOC_TARGET} PRIVATE cxx_std_20)

# Ensure app1 depends on the copy_fonts target
add_dependencies(${NTOC_TARGET} copy_fonts wwsfml)

