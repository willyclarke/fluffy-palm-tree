cmake_minimum_required(VERSION 3.25) # FetchContent seems to work with 3.25
project(tutlibevent)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Building in release mode")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall -fsanitize=address -fno-omit-frame-pointer")
set (CMAKE_CXX_STANDARD 20)

# Our Project
set(SRC ".")

set(TUT1
  ${SRC}/libeventtut1.cpp
  ${SRC}/statemachine.cpp
  )

set(LETARGET "libeventtut1")
add_executable(${LETARGET} ${TUT1})
add_dependencies(${LETARGET} libevent_ext) # Ensure dependency is built before your target

target_include_directories(${LETARGET} SYSTEM PUBLIC ${LIBEVENT_INCLUDE_DIR})
message(STATUS "${LETARGET} -> include directory set to ${LIBEVENT_INCLUDE_DIR}")

if (APPLE)
target_link_libraries(${LETARGET} PUBLIC
        # here you can add any library dependencies
        ${LIBEVENT_INSTALL_DIR}/lib/libevent.a
#        rt
        pthread
)
else()
target_link_libraries(${LETARGET} PUBLIC
        # here you can add any library dependencies
        ${LIBEVENT_INSTALL_DIR}/lib/libevent.a
        rt
        pthread
)
endif()
