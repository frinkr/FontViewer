#pragma once
#include "FX.h"

/* include headers of FreeType */
#include <functional>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CID_H
#include FT_GLYPH_H
#include FT_MODULE_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TABLES_H

FT_Error FXFilePathToOpenArgs(const FXString & filePath,
                              FT_Open_Args * args);

FT_Error FXFTCountFaces(FXFTLibrary lib, const FXString & filePath, size_t & count);

FT_Error
FXFTEnumurateFaces(FXFTLibrary lib,
                   const FXString & filePath,
                   std::function<bool(FXFTFace face, size_t index)> callback);
