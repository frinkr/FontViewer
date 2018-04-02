cmake_minimum_required(VERSION 3.0)
project(FontX)

set(FX_DIR ${PROJECT_SOURCE_DIR})

include(FindFreetype)
include(FindICU)

add_library(FontX
  FX.h
  FXLib.h
  FXLib.cpp
  FXFace.cpp
  FXFace.h
  FXCMap.h
  FXCMap.cpp
  
  FXPrivate.h
  FXPrivate.cpp
  FXNamesPrivate.h
  FXNamesPrivate.cpp
  )

target_include_directories(FontX PUBLIC
  ${FREETYPE_INCLUDE_DIRS}
  ${HARFBUZZ_INCLUDE_DIRS}
  ${ICU_INCLUDE_DIRS}
  )

target_link_libraries(FontX
  ${FREETYPE_LIBRARIES}
  ${ICU_LIBRARIES}
  ${HARFBUZZ_LIBRARIES}
  )


#########################################################
add_executable(FontXCL main.cpp)
target_compile_definitions(FontXCL
  PRIVATE
  -DFX_FONT_DIR=\"${FX_DIR}/../Resources\"
  )

target_link_libraries(FontXCL FontX)
