cmake_minimum_required(VERSION 3.25) # FetchContent seems to work with 3.25

find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
add_executable("${PROJECT_NAME}tests"
  coordinate.cpp
  ../src/engsupport.cpp
  ../src/fractal.cpp
  )
target_link_libraries("${PROJECT_NAME}tests" PRIVATE
  Catch2::Catch2WithMain
  raylib
  )
