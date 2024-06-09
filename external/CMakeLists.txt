cmake_minimum_required(VERSION 3.25) # FetchContent seems to work with 3.25

include(FetchContent)
include(ExternalProject)

# Dependencies
set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

####################################################################################################
# Pull in libevent
####################################################################################################
set(LIBEVENT_TAR_PATH "${CMAKE_SOURCE_DIR}/external/bundles/libevent-2.1.12-stable.tar.gz")
set(LIBEVENT_SHA256 "92e6de1be9ec176428fd2367677e61ceffc2ee1cb119035037a27d346b0403bb")

FetchContent_Declare(
  libevent
  URL file://${LIBEVENT_TAR_PATH}
  URL_HASH SHA256=${LIBEVENT_SHA256}
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_GetProperties(libevent)
if(NOT libevent_POPULATED)
  FetchContent_Populate(libevent)
  ExternalProject_Add(libevent_ext
    PREFIX ${libevent_BINARY_DIR}
    SOURCE_DIR ${libevent_SOURCE_DIR}
    CONFIGURE_COMMAND ${libevent_SOURCE_DIR}/configure --prefix=${libevent_BINARY_DIR}/install --disable-openssl
    BUILD_IN_SOURCE TRUE
    BUILD_COMMAND make ${MAKE_JOBS} # instead of ${CMAKE_MAKE_PROGRAM}
    INSTALL_COMMAND make install # instead of ${CMAKE_MAKE_PROGRAM} install
    BUILD_ALWAYS 0
    BUILD_BYPRODUCTS ${libevent_BINARY_DIR}/install/lib/libevent.a
  )
endif()

add_library(my_libevent STATIC IMPORTED)
add_dependencies(my_libevent libevent_ext)
set_target_properties(my_libevent
  PROPERTIES
  IMPORTED_LOCATION ${libevent_BINARY_DIR}/install/lib/libevent.a
)

set(LIBEVENT_INCLUDE_DIR ${libevent_SOURCE_DIR}/include CACHE PATH "Libevent include directory" FORCE)
message(STATUS "external -> LIBEVENT_INCLUDE set to ${LIBEVENT_INCLUDE_DIR}")
target_include_directories(my_libevent INTERFACE ${LIBEVENT_INCLUDE_DIR})
target_link_directories(my_libevent INTERFACE ${libevent_BINARY_DIR}/install/lib)

####################################################################################################
# Set the path to the installation directory of libevent
####################################################################################################
set(LIBEVENT_INSTALL_DIR "${libevent_BINARY_DIR}/install" CACHE PATH "libevent install dir")
message(STATUS "external -> LIBEVENT_INSTALL_DIR set to ${LIBEVENT_INSTALL_DIR}")

####################################################################################################
# Add the libevent installation directory to CMAKE_PREFIX_PATH
####################################################################################################
list(APPEND CMAKE_PREFIX_PATH "${LIBEVENT_INSTALL_DIR}")
message(STATUS "The CMAKE_PREFIX_PATH is ${CMAKE_PREFIX_PATH}")

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        9a2a4eadc0e50e0534278010624eac15991413a7
  FIND_PACKAGE_ARGS
)

# This will try calling find_package() first for both dependencies
FetchContent_MakeAvailable(Catch2)

####################################################################################################
# Simple Fast Multimedia Library
####################################################################################################
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)
