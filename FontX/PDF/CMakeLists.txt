cmake_minimum_required(VERSION 3.0)
project(FontXPDF)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenSSL REQUIRED)

################################################
# PoDoFo
set(PODOFO_BUILD_LIB_ONLY ON)
set(PODOFO_NO_FONTMANAGER ON)
set(PODOFO_BUILD_STATIC ON)
add_subdirectory(podofo)

add_library(FontXPDF
  FXPDFDocument.h
  FXPDFDocument.cpp
  FXPDFFace.cpp
  FXPDFFace.h
  )

target_include_directories(FontXPDF PUBLIC
  ${OPENSSL_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/podofo
  ${CMAKE_CURRENT_SOURCE_DIR}/podofo
  )

target_link_libraries(FontXPDF PRIVATE
  FontX
  podofo_static)

target_include_directories(FontXPDF PUBLIC
  ${CMAKE_SOURCE_DIR}
  )


#########################################################
add_executable(FontXPDFTester main.cpp)
target_link_libraries(FontXPDFTester
  FontX
  FontXPDF
  )
