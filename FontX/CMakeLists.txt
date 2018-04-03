cmake_minimum_required(VERSION 3.0)
project(FontX)

set(FX_DIR ${PROJECT_SOURCE_DIR})

if (APPLE)
  find_package(PkgConfig)
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/opt/icu4c/lib/pkgconfig")
endif()

find_package(Freetype REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem regex)

if (WIN32)
  include(FindICU)
else()
  pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
#  pkg_check_modules(ICU REQUIRED icu-uc)
endif()

add_library(FontX
  FX.h
  FXLib.h
  FXLib.cpp
  FXFace.cpp
  FXFace.h
  FXCMap.h
  FXCMap.cpp
  FXUCD.h
  FXUCD.cpp
  
  FXPrivate.h
  FXPrivate.cpp
  FXNamesPrivate.h
  FXNamesPrivate.cpp
  FXBoostPrivate.h
  FXBoostPrivate.cpp
  )

target_include_directories(FontX PUBLIC
  ${Boost_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
  ${HARFBUZZ_INCLUDE_DIRS}
  ${ICU_INCLUDE_DIRS}
  )

target_link_libraries(FontX
  ${Boost_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${ICU_LIBRARIES}
  ${HARFBUZZ_LIBRARIES}
  )

if (APPLE)
target_link_libraries(FontX
  ${ICU_LDFLAGS}
  ${HARFBUZZ_LDFLAGS}
  )  
endif()

if ()
  get_cmake_property(_variableNames VARIABLES)
  list (SORT _variableNames)
  foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endif()

#########################################################
add_executable(FontXCL main.cpp)
target_compile_definitions(FontXCL
  PRIVATE
  -DFX_RESOURCES_DIR=\"${FX_DIR}/../Resources\"
  )

target_link_libraries(FontXCL FontX)
