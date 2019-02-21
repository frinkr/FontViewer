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

add_library(FontX
  STATIC
  FX.h
  FXTag.h
  FXFT.h
  FXLib.h
  FXLib.cpp
  FXLog.h
  FXLog.cpp
  FXFace.cpp
  FXFace.h
  FXCMap.h
  FXCMap.cpp
  FXUnicode.h
  FXUnicode.cpp
  FXGlyph.h
  FXGlyph.cpp
  FXPixmap.h
  FXPixmap.cpp
  FXFaceDatabase.h
  FXFaceDatabase.cpp
  FXGlyphCache.h
  FXGlyphCache.cpp
  FXShaper.h
  FXShaper.cpp
  FXInspector.h
  FXInspector.cpp
  FXOT.h
  FXOT.cpp
  FXVersion.h
  FXVersion.cpp

  FXFTPrivate.h
  FXFTPrivate.cpp
  FXFTNames.h
  FXFTNames.cpp
  
  FXBoostPrivate.h
  FXBoostPrivate.cpp
  
  FXHBPrivate.h
  FXHBPrivate.cpp
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
