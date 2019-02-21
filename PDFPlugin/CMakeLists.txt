cmake_minimum_required(VERSION 3.0)
project(PDFPlugin)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(OpenSSL REQUIRED)

################################################
# PoDoFo
set(PODOFO_BUILD_LIB_ONLY ON)
set(PODOFO_NO_FONTMANAGER ON)
set(PODOFO_BUILD_STATIC ON)
add_subdirectory(podofo)

add_library(PDFPlugin
  SHARED
  FXPDF.h
  FXPDF.cpp
  )

target_include_directories(PDFPlugin PUBLIC
  ${OPENSSL_INCLUDE_DIR}
  )

target_link_libraries(PDFPlugin PRIVATE
  FontX
  podofo_static)

target_include_directories(PDFPlugin PUBLIC
  ${CMAKE_SOURCE_DIR}
  )


#########################################################
add_executable(PDFPluginCL main.cpp)
target_link_libraries(PDFPluginCL
  FontX
  PDFPlugin
  )
