cmake_minimum_required(VERSION 3.0)
project(FontX)

if (NOT WIN32)
  find_package(PkgConfig)
endif()

if (APPLE)
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/opt/icu4c/lib/pkgconfig")
endif()

find_package(Freetype REQUIRED)
find_package(cereal REQUIRED)
find_package(Iconv REQUIRED)
find_package(fribidi REQUIRED)

if (WIN32)
  find_package(ICU REQUIRED uc)
  find_package(HARFBUZZ REQUIRED harfbuzz)
else()
  pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
  pkg_check_modules(ICU REQUIRED icu-uc)
endif()

if (FX_BUILD_PDF_ADDON)
  add_subdirectory(PDF)
endif()

add_library(FontX
  STATIC
  FX.h
  FXCMap.cpp
  FXCMap.h
  FXCache.h
  FXFontFileIterator.cpp
  FXFontFileIterator.h
  FXFS.cpp
  FXFS.h
  FXFT.h
  FXFTNames.cpp
  FXFTNames.h
  FXFTPrivate.cpp
  FXFTPrivate.h
  FXFace.cpp
  FXFace.h
  FXFaceDatabase.cpp
  FXFaceDatabase.h
  FXGlyph.cpp
  FXGlyph.h
  FXHBPrivate.cpp
  FXHBPrivate.h
  FXInspector.cpp
  FXInspector.h
  FXLib.cpp
  FXLib.h
  FXLog.cpp
  FXLog.h
  FXOT.cpp
  FXOT.h
  FXPDF.h
  FXPixmap.cpp
  FXPixmap.h
  FXShaper.cpp
  FXShaper.h
  FXTag.h
  FXUnicode.cpp
  FXUnicode.h
  FXVersion.cpp
  FXVersion.h
  )

target_include_directories(FontX PUBLIC
  ${cereal_INCLUDE_DIRS}
  ${FRIBIDI_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIRS}
  ${HARFBUZZ_INCLUDE_DIRS}
  ${ICU_INCLUDE_DIRS}
  ${Iconv_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}
  )

target_link_libraries(FontX
  ${FRIBIDI_LIBRARY}
  ${FREETYPE_LIBRARIES}
  ${ICU_LIBRARIES}
  ${HARFBUZZ_LIBRARIES}
  ${Iconv_LIBRARIES}
  )

if (FX_BUILD_PDF_ADDON)
  target_link_libraries(FontX FontXPDF)
endif()

if (APPLE)
  target_link_libraries(FontX
    ${ICU_LDFLAGS}
    ${HARFBUZZ_LDFLAGS}
    "-framework CoreServices"
    "-framework CoreText"
    )
endif()
