cmake_minimum_required(VERSION 3.0)
project(FontViewer)

add_subdirectory(Font)
add_subdirectory(UI-Qt)

find_package(ZLIB REQUIRED)
add_executable(FontViewer main.cpp)
target_link_libraries(FontViewer ZLIB::ZLIB Font UI-Qt)
