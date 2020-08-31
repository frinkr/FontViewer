cmake_minimum_required(VERSION 3.0)
project(FontViewer)
include(cmake/bootstrap.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-multichar")
endif()

# PDF
set(FX_BUILD_PDF_ADDON ON)

if (FX_BUILD_PDF_ADDON)
add_definitions(
  -DFX_HAS_PDF_ADDON=1
  )
endif()


# Subdirectories
add_subdirectory(FontX)
add_subdirectory(FontXCommandLine)
add_subdirectory(UI-Qt)
add_subdirectory(App)
