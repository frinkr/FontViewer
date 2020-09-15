#pragma once

#include <functional>
#include <ft2build.h>
#include FT_CID_H
#include FT_FONT_FORMATS_H
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_IMAGE_H
#include FT_MODULE_H
#include FT_MULTIPLE_MASTERS_H
#include FT_SFNT_NAMES_H
#include FT_SIZES_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TABLES_H
#include FT_TYPE1_TABLES_H
#include FT_WINFONTS_H
#include "FX.h"

static_assert(sizeof(FXFixed) == sizeof(FT_Fixed), "FXFixed must be same size with FT_Fixed");
