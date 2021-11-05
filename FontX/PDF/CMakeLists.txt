cmake_minimum_required(VERSION 3.0)
project(FontXPDF)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(FontXPDF
  FXPDFDocument.h
  FXPDFDocument.cpp
  FXPDFFace.cpp
  FXPDFFace.h
  )

# PoDoFo
set(FX_USE_SYSTERM_PODOFO ON)

if (FX_USE_SYSTERM_PODOFO)
  find_package(PoDoFo REQUIRED)
  
  target_include_directories(FontXPDF PUBLIC
    ${PODOFO_INCLUDE_DIR}
    )

  target_link_libraries(FontXPDF PRIVATE
    FontX
    ${PODOFO_LIBRARY}
    )
  
else(FX_USE_SYSTERM_PODOFO)
  set(PODOFO_BUILD_LIB_ONLY ON)
  set(PODOFO_NO_FONTMANAGER ON)
  set(PODOFO_BUILD_STATIC ON)
  add_subdirectory(podofo)
  
  target_include_directories(FontXPDF PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/podofo
    ${CMAKE_CURRENT_SOURCE_DIR}/podofo/src
    )

  target_link_libraries(FontXPDF PRIVATE
    FontX
    podofo_static
    )
endif(FX_USE_SYSTERM_PODOFO)

target_include_directories(FontXPDF PUBLIC
  ${CMAKE_SOURCE_DIR}
  )
