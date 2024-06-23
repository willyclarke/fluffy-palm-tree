cmake_minimum_required(VERSION 3.16)
project(PBRWithSFML LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(PBR_TARGET pbrmain)

add_executable(${PBR_TARGET} pbrmain.cpp)
target_link_libraries(${PBR_TARGET} PRIVATE sfml-graphics)
target_compile_features(${PBR_TARGET} PRIVATE cxx_std_20)

# Ensure app1 depends on the copy_fonts target
add_dependencies(${PBR_TARGET} copy_fonts)

