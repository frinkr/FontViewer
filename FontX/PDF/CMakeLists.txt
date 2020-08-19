cmake_minimum_required(VERSION 3.0)
project(FontXPDF)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PoDoFo REQUIRED)
find_package(MuPdf REQUIRED)

add_library(FontXPDF
  FXPDFDocument.h
  FXPDFDocument.cpp
  FXPDFFace.cpp
  FXPDFFace.h
  )

target_include_directories(FontXPDF PUBLIC
  ${PODOFO_INCLUDE_DIR}
  ${MUPDF_INCLUDE_DIR}
  )

target_link_libraries(FontXPDF PRIVATE
  FontX
  ${PODOFO_LIBRARY}
  ${MUPDF_LIBRARY}
  )

target_include_directories(FontXPDF PUBLIC
  ${CMAKE_SOURCE_DIR}
  )

#########################################################
add_executable(FontXPDFTester main.cpp)
target_link_libraries(FontXPDFTester
  FontX
  FontXPDF
  )
