cmake_minimum_required(VERSION 3.0)
project(UI-Qt)


find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
add_library(UI-Qt
    QUMainWindow.cpp
    QUApp.cpp
    QUApp.h)

target_link_libraries(UI-Qt PRIVATE Qt5::Core Qt5::CorePrivate Qt5::WinMain)
target_link_libraries(UI-Qt PRIVATE Qt5::Gui Qt5::GuiPrivate)
target_link_libraries(UI-Qt PRIVATE Qt5::Widgets Qt5::WidgetsPrivate)