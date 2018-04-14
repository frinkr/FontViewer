cmake_minimum_required(VERSION 3.0)
project(UI-Qt)

set(CMAKE_AUTOMOC ON)
set(CMAKE AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

set(UI QUDocumentWindow.ui QUOpenFontDialog.ui)

qt5_wrap_ui(UI_HEADERS ${UI})

set(SHARED_SOURCES 
  QUMain.h
  QUApplication.h
  QUApplication.cpp
  QUApplication.qrc
  QUSingleApplication.h
  QUSingleApplication.cpp
  QUSingleApplication_p.h
  QUDocumentWindowManager.h
  QUDocumentWindow.h
  QUDocumentWindow.cpp
  QUDocument.h
  QUDocument.cpp
  QUOpenFontDialog.h
  QUOpenFontDialog.cpp
  QUConv.h
  QUConv.cpp
  QUEncoding.h
  QUEncoding.cpp
  QUGlyphListView.h
  QUGlyphListView.cpp
  QUGlyphInfoWidget.h
  QUGlyphInfoWidget.cpp
  QUToolBarWidget.cpp
  QUToolBarWidget.h
  QUTemplateInstantializer.h
  QUTemplateInstantializer.cpp
  QUFontManager.h
  QUFontManager.cpp
  QUFontComboBox.h
  QUFontComboBox.cpp
  )
set(APPLE_SOURCES
  MacMain.mm
  MacQUDocumentWindowManager.mm
  MacApplicationDelegate.h
  MacApplicationDelegate.mm
  )
set(WIN32_SOURCES
  QUDocumentWindowManager.cpp
  QUMain.cpp
  )

if (APPLE)
  set(PLATFORM_SOURCES ${APPLE_SOURCES})
else()
  set(PLATFORM_SOURCES ${WIN32_SOURCES})
endif()

add_library(UI-Qt
  ${SHARED_SOURCES}
  ${PLATFORM_SOURCES}
  ${UI}
  ${UI_HEADERS}
  )

target_link_libraries(UI-Qt PRIVATE Qt5::Core Qt5::CorePrivate)
target_link_libraries(UI-Qt PRIVATE Qt5::Gui Qt5::GuiPrivate)
target_link_libraries(UI-Qt PRIVATE Qt5::Widgets Qt5::WidgetsPrivate)
target_link_libraries(UI-Qt PRIVATE Qt5::Network Qt5::NetworkPrivate)

if (APPLE)
  target_link_libraries(UI-Qt PRIVATE "-framework Cocoa")
endif()

target_include_directories(UI-Qt PUBLIC
  ${CMAKE_SOURCE_DIR}
  )
