cmake_minimum_required(VERSION 3.0)
project(UI-QtQuick)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (NOT WIN32)
  set(UIQT_USE_FONTCONFIG ON)
endif()

find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${QtQml_INCLUDE_DIRS})

add_definitions(${Qt5Widgets_DEFINITIONS} ${QtQml_DEFINITIONS} ${${Qt5Quick_DEFINITIONS}})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



if(NOT DEFINED HEADERS)
  file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
endif()
if(NOT DEFINED SOURCES)
  file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
endif()

qt5_add_resources(QT_RESOURCES qml.qrc)

source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})
add_executable(
  UI-QtQuick
  ${HEADERS}
  ${SOURCES}
  ${QT_RESOURCES})
target_link_libraries(
  UI-QtQuick
  Qt5::Widgets
  Qt5::Qml
  Qt5::Quick
  )
