cmake_minimum_required(VERSION 3.0)
project(UI-Qt)

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

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

if (UIQT_USE_FONTCONFIG)
  find_package(Fontconfig REQUIRED)
  add_definitions(-DUIQT_USE_FONTCONFIG=1)
endif()

set(UI
  QUAboutDialog.ui
  QUCMapBlockWidget.ui
  QUDocumentWindow.ui
  QUFontInfoWidget.ui
  QUGlyphTableWidget.ui
  QUOpenFontDialog.ui
  QUPreferencesDialog.ui
  QUProgressDialog.ui
  QUSearchWidget.ui
  QUShapingWidget.ui
  )

qt5_wrap_ui(UI_HEADERS ${UI})

set(SHARED_SOURCES 
  QUAboutDialog.cpp
  QUAboutDialog.h
  QUApplication.cpp
  QUApplication.h
  QUApplication.qrc
  QUCMapBlockWidget.cpp
  QUCMapBlockWidget.h
  QUConv.cpp
  QUConv.h
  QUDockTitleBarWidget.cpp
  QUDockTitleBarWidget.h
  QUDocument.cpp
  QUDocument.h
  QUDocumentWindow.cpp
  QUDocumentWindow.h
  QUDocumentWindowManager.cpp
  QUDocumentWindowManager.h
  QUEncoding.cpp
  QUEncoding.h
  QUFontComboBox.cpp
  QUFontComboBox.h
  QUFontInfoWidget.cpp
  QUFontInfoWidget.h
  QUFontManager.cpp
  QUFontManager.h
  QUGlyphInfoWidget.cpp
  QUGlyphInfoWidget.h
  QUGlyphListView.cpp
  QUGlyphListView.h
  QUGlyphTableWidget.cpp
  QUGlyphTableWidget.h
  QUHtmlTemplate.cpp
  QUHtmlTemplate.h
  QUMain.cpp
  QUMain.h
  QUMenuBar.cpp
  QUMenuBar.h
  QUOpenFontDialog.cpp
  QUOpenFontDialog.h
  QUPopoverWindow.cpp
  QUPopoverWindow.h
  QUPreferencesDialog.cpp
  QUPreferencesDialog.h
  QUProgressDialog.cpp
  QUProgressDialog.h
  QUResource.cpp
  QUResource.h
  QUSearchEngine.cpp
  QUSearchEngine.h
  QUSearchWidget.cpp
  QUSearchWidget.h
  QUShapingWidget.cpp
  QUShapingWidget.h
  QUTheme.cpp
  QUTheme.h
  QUToolBar.cpp
  QUToolBar.h
  )
set(APPLE_SOURCES
  MacApplicationDelegate.h
  MacApplicationDelegate.mm
  MacHelper.h
  MacHelper.mm
  )

if (APPLE)
  set(PLATFORM_SOURCES ${APPLE_SOURCES})
else()
  set(PLATFORM_SOURCES ${ALL_SOURCES})
endif()

add_library(UI-Qt
  ${SHARED_SOURCES}
  ${PLATFORM_SOURCES}
  ${UI}
  ${UI_HEADERS}
  )

target_link_libraries(UI-Qt PRIVATE Qt5::Core)
target_link_libraries(UI-Qt PRIVATE Qt5::Gui)
target_link_libraries(UI-Qt PRIVATE Qt5::Widgets)
target_link_libraries(UI-Qt PRIVATE Qt5::Network)
target_link_libraries(UI-Qt PRIVATE FontX)

if (UIQT_USE_FONTCONFIG)
  target_link_libraries(UI-Qt PRIVATE ${FONTCONFIG_LIBRARIES})
endif()

if (APPLE)
  target_link_libraries(UI-Qt PRIVATE "-framework Cocoa")
endif()

target_include_directories(UI-Qt PUBLIC
  ${CMAKE_SOURCE_DIR}
  )
