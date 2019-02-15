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

if (WIN32)
  find_package(Qt5WinExtras CONFIG REQUIRED)
endif()

if (UIQT_USE_FONTCONFIG)
  find_package(Fontconfig REQUIRED)
  add_definitions(-DUIQT_USE_FONTCONFIG=1)
endif()

set(UI
  QXAboutDialog.ui
  QXAboutFontsDialog.ui
  QXCMapBlockWidget.ui
  QXDocumentWindow.ui
  QXFontCollectionDialog.ui
  QXFontInfoWidget.ui
  QXGlyphTableWidget.ui
  QXOpenFontDialog.ui
  QXPreferencesDialog.ui
  QXProgressDialog.ui
  QXSearchWidget.ui
  QXShapingWidget.ui
  QXVariableWidget.ui
  )

qt5_wrap_ui(UI_HEADERS ${UI})

set(SHARED_SOURCES 
  QXAboutDialog.cpp
  QXAboutDialog.h
  QXAboutFontsDialog.cpp
  QXAboutFontsDialog.h
  QXApplication.cpp
  QXApplication.h
  QXApplication.qrc
  QXCMapBlockWidget.cpp
  QXCMapBlockWidget.h
  QXCollectionView.cpp
  QXCollectionView.h
  QXConv.cpp
  QXConv.h
  QXDockTitleBarWidget.cpp
  QXDockTitleBarWidget.h
  QXDocument.cpp
  QXDocument.h
  QXDocumentWindow.cpp
  QXDocumentWindow.h
  QXDocumentWindowManager.cpp
  QXDocumentWindowManager.h
  QXEncoding.cpp
  QXEncoding.h
  QXFontCollectionDialog.cpp
  QXFontCollectionDialog.h
  QXFontComboBox.cpp
  QXFontComboBox.h
  QXFontInfoWidget.cpp
  QXFontInfoWidget.h
  QXFontManager.cpp
  QXFontManager.h
  QXGlyphInfoWidget.cpp
  QXGlyphInfoWidget.h
  QXGlyphListView.cpp
  QXGlyphListView.h
  QXGlyphTableWidget.cpp
  QXGlyphTableWidget.h
  QXHtmlTemplate.cpp
  QXHtmlTemplate.h
#QXJumpListHelper.cpp
#QXJumpListHelper.h
  QXMain.cpp
  QXMain.h
  QXMenuBar.cpp
  QXMenuBar.h
  QXOpenFontDialog.cpp
  QXOpenFontDialog.h
  QXPopoverWindow.cpp
  QXPopoverWindow.h
  QXPreferences.cpp
  QXPreferences.h
  QXPreferencesDialog.cpp
  QXPreferencesDialog.h
  QXProgressDialog.cpp
  QXProgressDialog.h
  QXResource.cpp
  QXResource.h
  QXSearchEngine.cpp
  QXSearchEngine.h
  QXSearchWidget.cpp
  QXSearchWidget.h
  QXShapingWidget.cpp
  QXShapingWidget.h
  QXTheme.cpp
  QXTheme.h
  QXToolBar.cpp
  QXToolBar.h
  QXVariableWidget.cpp
  QXVariableWidget.h
  )
set(APPLE_SOURCES
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
if (WIN32)
  target_link_libraries(UI-Qt PRIVATE Qt5::WinExtras Qt5::WinExtrasPrivate)
endif()

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
