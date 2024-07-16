cmake_minimum_required(VERSION 3.16)
project(PBRWithSFML LANGUAGES CXX)

##############################
### Copy fonts.
##############################
set(FONT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/sfmlprojects/fonts")
set(FONT_DEST_DIR "${CMAKE_BINARY_DIR}/fonts")
set(FONT_FILE "miracode/Miracode.ttf")

file(MAKE_DIRECTORY ${FONT_DEST_DIR})

# Add a custom target to copy the font file
add_custom_target(copy_fonts ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${FONT_SOURCE_DIR}/${FONT_FILE}
    ${FONT_DEST_DIR}/${FONT_FILE}
    COMMENT "Copying ${FONT_FILE} to ${FONT_DEST_DIR}"
)

add_subdirectory(lib)
add_subdirectory(physicallybasedrendering/apps)
add_subdirectory(natureofcode/apps)

