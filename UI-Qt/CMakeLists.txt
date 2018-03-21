cmake_minimum_required(VERSION 3.0)
project(UI-Qt)

set(CMAKE_AUTOMOC ON)
set(CMAKE AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(UI ${CMAKE_CURRENT_LIST_DIR}/QUMainWindow.ui)

qt5_wrap_ui(UI_HEADERS ${UI})

add_library(UI-Qt
    QUMainWindow.cpp
    QUMainWindow.h
    ${UI}
    ${UI_HEADERS} 
    QUApp.cpp
    QUApp.h)

target_link_libraries(UI-Qt PRIVATE Qt5::Core Qt5::CorePrivate Qt5::WinMain)
target_link_libraries(UI-Qt PRIVATE Qt5::Gui Qt5::GuiPrivate)
target_link_libraries(UI-Qt PRIVATE Qt5::Widgets Qt5::WidgetsPrivate)