cmake_minimum_required(VERSION 3.0)
project(FontCore)

include(FindFreetype)
#include(FindHarfbuzz)
#find_package(HARFBUZZ REQUIRED)

add_library(FontCore
    FVFace.cpp
    FVFace.h)

target_include_directories(FontCore PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(FontCore ${FREETYPE_LIBRARIES})