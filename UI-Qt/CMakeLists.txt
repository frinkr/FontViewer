cmake_minimum_required(VERSION 3.0)
project(UI-Qt)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (UNIX AND NOT APPLE)
  set(UIQT_USE_FONTCONFIG ON)
endif()

    find_package(Qt6 CONFIG REQUIRED)
    # Note: 46 target(s) were omitted.
    
find_package(Qt6Core REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Network REQUIRED)

if (WIN32)
  #find_package(Qt6WinExtras CONFIG REQUIRED)
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
  QXFontListWindow.ui
  QXFontInfoWidget.ui
  QXGlyphTableWidget.ui
  QXOutlineDialog.ui
  QXPreferencesDialog.ui
  QXShapingWidget.ui
  QXShapingOptionsWidget.ui
  QXVariableWidget.ui
  )

qt6_wrap_ui(UI_HEADERS ${UI})

set(SHARED_SOURCES
  score_match.cpp
  score_match.h
  QXAboutDialog.cpp
  QXAboutDialog.h
  QXAboutFontsDialog.cpp
  QXAboutFontsDialog.h
  QXApplication.cpp
  QXApplication.h
  QXApplication.qrc
  QXCMapBlockWidget.cpp
  QXCMapBlockWidget.h
  QXCollectionModel.cpp
  QXCollectionModel.h
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
  QXFontInfoWidget.cpp
  QXFontInfoWidget.h
  QXFontListWindow.cpp
  QXFontListWindow.h
  QXFontListView.cpp
  QXFontListView.h
  QXFontListModel.cpp
  QXFontListModel.h
  QXFontManager.cpp
  QXFontManager.h
  QXGlyphCollectionView.cpp
  QXGlyphCollectionView.h
  QXGlyphInfoWidget.cpp
  QXGlyphInfoWidget.h
  QXGlyphTableWidget.cpp
  QXGlyphTableWidget.h
  QXHtmlTemplate.cpp
  QXHtmlTemplate.h
  QXIconicButton.cpp
  QXIconicButton.h
  QXIconEngine.cpp
  QXIconEngine.h
  QXImageHelpers.h
  QXImageHelpers.cpp
  QXMain.cpp
  QXMain.h
  QXMenuBar.cpp
  QXMenuBar.h
  QXOutlineDialog.h
  QXOutlineDialog.cpp
  QXOutlineWidget.h
  QXOutlineWidget.cpp
  QXPopoverWindow.cpp
  QXPopoverWindow.h
  QXPreferences.cpp
  QXPreferences.h
  QXPreferencesDialog.cpp
  QXPreferencesDialog.h
  QXResources.cpp
  QXResources.h
  QXRelatedFontsWidget.cpp
  QXRelatedFontsWidget.h
  QXSearchEngine.cpp
  QXSearchEngine.h
  QXShapingWidget.cpp
  QXShapingWidget.h
  QXShapingOptionsWidget.cpp
  QXShapingOptionsWidget.h
  QXSplash.cpp
  QXSplash.h
  QXTheme.cpp
  QXTheme.h
  QXThemedWindow.cpp
  QXThemedWindow.h
  QXToastMessage.cpp
  QXToastMessage.h
  QXToolBar.cpp
  QXToolBar.h
  QXVariableWidget.cpp
  QXVariableWidget.h
  )

set(APPLE_SOURCES
  MacHelper.h
  MacHelper.mm
  QXThemedWindowMac.mm
  )
set(WIN32_SOURCES
  WinHelper.h
  WinHelper.cpp
  QXThemedWindowWin.cpp
  )

if (APPLE)
  set(PLATFORM_SOURCES ${APPLE_SOURCES})
elseif(WIN32)
  set(PLATFORM_SOURCES ${WIN32_SOURCES})
endif()

add_library(UI-Qt
  STATIC
  ${SHARED_SOURCES}
  ${PLATFORM_SOURCES}
  ${UI}
  ${UI_HEADERS}
  )

target_link_libraries(UI-Qt PRIVATE Qt6::Core)
target_link_libraries(UI-Qt PRIVATE Qt6::Gui)
target_link_libraries(UI-Qt PRIVATE Qt6::Widgets)
target_link_libraries(UI-Qt PRIVATE Qt6::Network)
if (WIN32)
  #target_link_libraries(UI-Qt PRIVATE Qt6::WinExtras Qt6::WinExtrasPrivate)
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
