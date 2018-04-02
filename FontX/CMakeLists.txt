cmake_minimum_required(VERSION 3.0)
project(FontX)

set(FX_DIR ${PROJECT_SOURCE_DIR})

include(FindFreetype)
#include(FindHarfbuzz)
#find_package(HARFBUZZ REQUIRED)

add_library(FontX
  FX.h
  FXLib.h
  FXLib.cpp
  FXPrivate.h
  FXFace.cpp
  FXFace.h)

target_include_directories(FontX PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(FontX ${FREETYPE_LIBRARIES})

add_executable(FontXCL main.cpp)
target_compile_definitions(FontXCL
  PRIVATE
  -DFX_FONT_DIR=\"${FX_DIR}/../Resources\"
  )
target_link_libraries(FontXCL FontX)
