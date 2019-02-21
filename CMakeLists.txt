cmake_minimum_required(VERSION 3.0)
project(FontViewer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-multichar")
endif()

set(FX_DIR ${PROJECT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${FX_DIR}/cmake")

add_definitions(
  -DFX_RESOURCES_DIR=\"${FX_DIR}/Resources\"
  )

add_subdirectory(FontX)
add_subdirectory(UI-Qt)
add_subdirectory(App)
