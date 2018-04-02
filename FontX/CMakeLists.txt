cmake_minimum_required(VERSION 3.0)
project(FontX)

set(FX_DIR ${PROJECT_SOURCE_DIR})

find_package(PkgConfig)
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/opt/icu4c/lib/pkgconfig")

#include(FindFreetype)
pkg_check_modules(FREETYPE REQUIRED freetype2)
pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
pkg_check_modules(ICU REQUIRED icu-uc)

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
  ${FREETYPE_LDFLAGS} #${FREETYPE_LIBRARIES}
  ${ICU_LDFLAGS}
  ${HARFBUZZ_LDFLAGS}
  )


#########################################################
add_executable(FontXCL main.cpp)
target_compile_definitions(FontXCL
  PRIVATE
  -DFX_FONT_DIR=\"${FX_DIR}/../Resources\"
  )

target_link_libraries(FontXCL FontX)
