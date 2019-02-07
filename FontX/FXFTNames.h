#pragma once
#include "FXFT.h"

std::string
FXGetPlatformName(FT_UShort platformId);

std::string
FXGetEncodingName(FT_UShort platformId, FT_UShort encodingId);

FXString
FXGetLanguageName(FT_UShort platformId, FT_UShort languageId);

FXString
FXToString(FT_UShort platformId, FT_UShort encodingId, void * string, uint32_t stringLen);

/* SFNT name id to string */
FXString
FXSFNTGetName(FT_SfntName * sfnt);

/* SFNT string value */
FXString
FXSFNTGetValue(FT_SfntName * sfnt);

/* SFNT language name */
FXString
FXSFNTGetLanguage(FT_SfntName *sfnt, FT_Face face);
