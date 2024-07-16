cmake_minimum_required(VERSION 3.16)
project(wwsfmllib LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(WWSFMLLIB_TARGET wwsfml)


add_library(${WWSFMLLIB_TARGET}
    src/wwsfmllib.cpp
    include/wwsfmllib.hpp
)

target_compile_features(${WWSFMLLIB_TARGET} PRIVATE cxx_std_20)

target_link_libraries(${WWSFMLLIB_TARGET} PUBLIC sfml-graphics)

# Specify the include directories for the library
target_include_directories(${WWSFMLLIB_TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


