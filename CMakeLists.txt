cmake_minimum_required(VERSION 3.0)
project(FontViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-multichar")
endif()

set(FX_DIR ${PROJECT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${FX_DIR}/cmake")

# Resource folder
add_definitions(
  -DFX_RESOURCES_DIR=\"${FX_DIR}/Resources\"
  )

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
