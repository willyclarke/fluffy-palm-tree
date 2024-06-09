cmake_minimum_required(VERSION 3.16)
project(PBRWithSFML LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_executable(pbrmain pbrmain.cpp)
target_link_libraries(pbrmain PRIVATE sfml-graphics)
target_compile_features(pbrmain PRIVATE cxx_std_17)
