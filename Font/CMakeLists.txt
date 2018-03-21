cmake_minimum_required(VERSION 3.0)
project(Font)

include(FindFreetype)
#include(FindHarfbuzz)
#find_package(HARFBUZZ REQUIRED)

add_library(Font
    FVFace.cpp
    FVFace.h)

target_include_directories(Font PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(Font ${FREETYPE_LIBRARIES})