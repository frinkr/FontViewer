cmake_minimum_required(VERSION 3.0)
project(FontX)

if (NOT WIN32)
  find_package(PkgConfig)
endif()

if (APPLE)
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/opt/icu4c/lib/pkgconfig")
endif()

find_package(Freetype REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem regex serialization)
find_package(Iconv REQUIRED)

if (WIN32)
  find_package(ICU REQUIRED uc)
  find_package(HARFBUZZ REQUIRED harfbuzz)
else()
  pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
  pkg_check_modules(ICU REQUIRED icu-uc)
endif()

add_subdirectory(PDF)

add_library(FontX
  STATIC
  FX.h
  FXBoostPrivate.cpp
  FXBoostPrivate.h
  FXCMap.cpp
  FXCMap.h
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
  FXGlyphCache.cpp
  FXGlyphCache.h
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
  ${Boost_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
  ${HARFBUZZ_INCLUDE_DIRS}
  ${ICU_INCLUDE_DIRS}
  ${Iconv_INCLUDE_DIRS}
  )

target_link_libraries(FontX
  ${Boost_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${ICU_LIBRARIES}
  ${HARFBUZZ_LIBRARIES}
  ${Iconv_LIBRARIES}
  )

if (APPLE)
  target_link_libraries(FontX
    ${ICU_LDFLAGS}
    ${HARFBUZZ_LDFLAGS}
    "-framework CoreServices"
  )
endif()

#########################################################
add_executable(FontXTester main.cpp)
target_link_libraries(FontXTester
  FontX
  ${Boost_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${ICU_LIBRARIES}
  ${HARFBUZZ_LIBRARIES}
  ${Iconv_LIBRARIES}
  )
