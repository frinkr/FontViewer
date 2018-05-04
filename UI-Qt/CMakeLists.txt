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
  QUDocumentWindow.ui
  QUOpenFontDialog.ui
  QUFontInfoWidget.ui
  QUCMapBlockWidget.ui
  QUSearchWidget.ui
  QUGlyphTableWidget.ui)

qt5_wrap_ui(UI_HEADERS ${UI})

set(SHARED_SOURCES 
  QUMain.h
  QUApplication.h
  QUApplication.cpp
  QUApplication.qrc
  qdarkstyle/style.qrc
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
  QUHtmlTemplate.h
  QUHtmlTemplate.cpp
  QUFontManager.h
  QUFontManager.cpp
  QUFontComboBox.h
  QUFontComboBox.cpp
  QUFontInfoWidget.h
  QUFontInfoWidget.cpp
  QUResource.h
  QUResource.cpp
  QUCMapBlockWidget.h
  QUCMapBlockWidget.cpp
  QUPopoverWindow.h
  QUPopoverWindow.cpp
  QUSearchWidget.h
  QUSearchWidget.cpp
  QUSearchEngine.h
  QUSearchEngine.cpp
  QUGlyphTableWidget.h
  QUGlyphTableWidget.cpp
  )
set(APPLE_SOURCES
  MacMain.mm
  MacQUDocumentWindowManager.mm
  MacApplicationDelegate.h
  MacApplicationDelegate.mm
  )
set(ALL_SOURCES
  QUDocumentWindowManager.cpp
  QUMain.cpp
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
