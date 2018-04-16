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

FXString
FXSFNTGetName(FT_SfntName * sfnt);

FXString
FXSFNTGetValue(FT_SfntName * sfnt);

FXString
FXSFNTGetLanguage(FT_SfntName *sfnt, FT_Face face);
