cmake_minimum_required(VERSION 3.0)
include(cmake/bootstrap.cmake)

project(FontViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(FX_HAVE_PDF_ADDON=1)

# Subdirectories
add_subdirectory(FontX)
add_subdirectory(FontXCommandLine)
add_subdirectory(UI-Qt)
add_subdirectory(App)
