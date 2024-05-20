cmake_minimum_required(VERSION 3.25) # FetchContent seems to work with 3.25
project(fluffy)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Building in release mode")
endif()

# Initialize PLATFORM variable with default value "NOT_SET"
set(PLATFORM "Desktop" CACHE STRING "Platform type (e.g., Web)")
message(STATUS "Platform: ${PLATFORM}")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall -fsanitize=address -fno-omit-frame-pointer")
set (CMAKE_CXX_STANDARD 20)

add_subdirectory(external)
add_subdirectory(tutorial)

# Our Project
set(SRC "src")

set(CURVESSRC
  ${SRC}/curvesrobotics.cpp
  ${SRC}/engsupport.cpp
  ${SRC}/fractal.cpp
  )

set(JULIASHADER
  ${SRC}/shaders_julia_set.cpp
  )

add_executable("${PROJECT_NAME}fourier" ${SRC}/fourierwraylib.cpp ${SRC}/engsupport.cpp)

add_executable("${PROJECT_NAME}curves" ${CURVESSRC})

add_executable("${PROJECT_NAME}juliashader" ${JULIASHADER})

#set(raylib_VERBOSE 1)
target_link_libraries("${PROJECT_NAME}fourier" raylib)
target_link_libraries("${PROJECT_NAME}curves" raylib)
target_link_libraries("${PROJECT_NAME}curves" -fsanitize=address)
target_link_libraries("${PROJECT_NAME}juliashader" raylib)
# Link the AddressSanitizer runtime library
# target_link_libraries("${PROJECT_NAME}curves" PUBLIC -fsanitize=address)

###
# Get resources/shaders from raylib
###
  set(SHADER_RESOURCES_FILES ${CMAKE_SOURCE_DIR}/shaders)
  set(SHADER_RESOURCES_FILES_DEST ${CMAKE_BINARY_DIR}/resources)
  file(COPY ${SHADER_RESOURCES_FILES} DESTINATION ${SHADER_RESOURCES_FILES_DEST})

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
  set(PROJECT_CURVES_OSX "${PROJECT_NAME}curvesosx")
  add_executable(${PROJECT_CURVES_OSX} MACOSX_BUNDLE ${CURVESSRC})

  set(PROJECT_JULIA_OSX "${PROJECT_NAME}juliashaderosx")
  add_executable(${PROJECT_JULIA_OSX} MACOSX_BUNDLE ${JULIASHADER})

  get_target_property(MY_RAYLIB_INCLUDES raylib INCLUDE_DIRECTORIES)
  foreach(dir ${MY_RAYLIB_INCLUDES})
    target_include_directories(${PROJECT_CURVES_OSX} PRIVATE ${dir})
    target_include_directories(${PROJECT_JULIA_OSX} PRIVATE ${dir})
  endforeach()

  set_target_properties(${PROJECT_CURVES_OSX} PROPERTIES
    BUNDLE True
    MACOSX_BUNDLE_GUI_IDENTIFIER my.domain.style.identifier.${PROJECT_CURVES_OSX}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_CURVES_OSX}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/customtemplate.plist.in
    )

  set(RESOURCE_SPECIFICATION "Resources.plist")
  message(STATUS "RESOURCE_SPECIFICATION:" ${RESOURCE_SPECIFICATION})
  configure_file(${CMAKE_SOURCE_DIR}/cmake/customtemplate.plist.in ${CMAKE_BINARY_DIR}/Info.plist @ONLY)

  set(SHADER_PLIST_FILE ${CMAKE_SOURCE_DIR}/cmake/Resources.plist)
  set(SHADER_PLIST_FILE_DEST ${PROJECT_JULIA_OSX}.app/Contents)
  file(COPY ${SHADER_PLIST_FILE} DESTINATION ${SHADER_PLIST_FILE_DEST})
  file(COPY ${SHADER_PLIST_FILE} DESTINATION ${CMAKE_BINARY_DIR})

  set(SHADER_RESOURCES_FILES ${CMAKE_SOURCE_DIR}/shaders)
  set(SHADER_RESOURCES_FILES_DEST ${PROJECT_JULIA_OSX}.app/Contents/MacOS/resources)
  file(COPY ${SHADER_RESOURCES_FILES} DESTINATION ${SHADER_RESOURCES_FILES_DEST})

  set_target_properties(${PROJECT_JULIA_OSX} PROPERTIES
    BUNDLE True
    MACOSX_BUNDLE_GUI_IDENTIFIER my.domain.style.identifier.${PROJECT_JULIA_OSX}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_JULIA_OSX}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist
    )

    target_link_libraries("${PROJECT_NAME}fourier" "-framework IOKit")
    target_link_libraries("${PROJECT_NAME}fourier" "-framework Cocoa")
    target_link_libraries("${PROJECT_NAME}fourier" "-framework OpenGL")
    target_link_libraries("${PROJECT_NAME}curves" "-framework IOKit")
    target_link_libraries("${PROJECT_NAME}curves" "-framework Cocoa")
    target_link_libraries("${PROJECT_NAME}curves" "-framework OpenGL")
    target_link_libraries("${PROJECT_NAME}juliashader" "-framework IOKit")
    target_link_libraries("${PROJECT_NAME}juliashader" "-framework Cocoa")
    target_link_libraries("${PROJECT_NAME}juliashader" "-framework OpenGL")
    target_link_libraries("${PROJECT_CURVES_OSX}" "-framework IOKit")
    target_link_libraries("${PROJECT_CURVES_OSX}" "-framework Cocoa")
    target_link_libraries("${PROJECT_CURVES_OSX}" "-framework OpenGL")
    target_link_libraries("${PROJECT_CURVES_OSX}" raylib)
    set_target_properties("${PROJECT_CURVES_OSX}" PROPERTIES DEBUG_POSTFIX "_debug" RELEASE_POSTFIX "")
    target_link_libraries("${PROJECT_JULIA_OSX}" "-framework IOKit")
    target_link_libraries("${PROJECT_JULIA_OSX}" "-framework Cocoa")
    target_link_libraries("${PROJECT_JULIA_OSX}" "-framework OpenGL")
    target_link_libraries("${PROJECT_JULIA_OSX}" raylib)
    set_target_properties("${PROJECT_JULIA_OSX}" PROPERTIES DEBUG_POSTFIX "_debug" RELEASE_POSTFIX "")

endif()

###############################################################################
# Specify different properties for different build types
###############################################################################
set_target_properties("${PROJECT_NAME}fourier" PROPERTIES DEBUG_POSTFIX "_debug" RELEASE_POSTFIX "")
set_target_properties("${PROJECT_NAME}curves" PROPERTIES DEBUG_POSTFIX "_debug" RELEASE_POSTFIX "")
set_target_properties("${PROJECT_NAME}juliashader" PROPERTIES DEBUG_POSTFIX "_debug" RELEASE_POSTFIX "")

###############################################################################
#### TESTS
###############################################################################
add_subdirectory(tests)

#############################################################
#### Set up installer config.
#############################################################
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
install(TARGETS "${PROJECT_NAME}fourier" RUNTIME DESTINATION bin)
install(TARGETS "${PROJECT_NAME}curves" RUNTIME DESTINATION bin)
install(TARGETS "${PROJECT_NAME}juliashader" RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_BINARY_DIR}/resources/ DESTINATION bin/resources)
install(DIRECTORY ${CMAKE_BINARY_DIR}/resources/ DESTINATION bin/resources)
set(CPACK_PACKAGE_NAME "CoolStuff")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)

