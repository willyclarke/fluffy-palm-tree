cmake_minimum_required(VERSION 3.25) # FetchContent seems to work with 3.25
project(fluffy)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -Wall")
set (CMAKE_CXX_STANDARD 17)

add_subdirectory(external)

# Our Project
set(SRC "src")

set(CURVESSRC
  ${SRC}/curvesrobotics.cpp
  ${SRC}/engsupport.cpp
  ${SRC}/fractal.cpp
  )

add_executable("${PROJECT_NAME}fourier" ${SRC}/fourierwraylib.cpp ${SRC}/engsupport.cpp)

add_executable("${PROJECT_NAME}curves" ${CURVESSRC})

#set(raylib_VERBOSE 1)
target_link_libraries("${PROJECT_NAME}fourier" raylib)
target_link_libraries("${PROJECT_NAME}curves" raylib)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
  set(PROJECT_CURVES_OSX "${PROJECT_NAME}curvesosx")
  add_executable(${PROJECT_CURVES_OSX} MACOSX_BUNDLE ${CURVESSRC})

  get_target_property(MY_RAYLIB_INCLUDES raylib INCLUDE_DIRECTORIES)
  foreach(dir ${MY_RAYLIB_INCLUDES})
    target_include_directories(${PROJECT_CURVES_OSX} PRIVATE ${dir})
  endforeach()

  set_target_properties(${PROJECT_CURVES_OSX} PROPERTIES
    BUNDLE True
    MACOSX_BUNDLE_GUI_IDENTIFIER my.domain.style.identifier.${PROJECT_CURVES_OSX}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_CURVES_OSX}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/customtemplate.plist.in
    )

    target_link_libraries("${PROJECT_NAME}fourier" "-framework IOKit")
    target_link_libraries("${PROJECT_NAME}fourier" "-framework Cocoa")
    target_link_libraries("${PROJECT_NAME}fourier" "-framework OpenGL")
    target_link_libraries("${PROJECT_NAME}curves" "-framework IOKit")
    target_link_libraries("${PROJECT_NAME}curves" "-framework Cocoa")
    target_link_libraries("${PROJECT_NAME}curves" "-framework OpenGL")
    target_link_libraries("${PROJECT_CURVES_OSX}" "-framework IOKit")
    target_link_libraries("${PROJECT_CURVES_OSX}" "-framework Cocoa")
    target_link_libraries("${PROJECT_CURVES_OSX}" "-framework OpenGL")
    target_link_libraries("${PROJECT_CURVES_OSX}" raylib)

endif()

###############################################################################
#### TESTS
###############################################################################
add_subdirectory(tests)

#############################################################
#### Set up installer config.
#############################################################
install(TARGETS "${PROJECT_NAME}fourier" RUNTIME DESTINATION bin)
install(TARGETS "${PROJECT_NAME}curves" RUNTIME DESTINATION bin)
set(CPACK_PACKAGE_NAME "CoolStuff")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)

