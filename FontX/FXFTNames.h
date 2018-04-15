#pragma once
#include "FXFT.h"

std::string
FXPlatformName(FT_UShort platformId);

std::string
FXEncodingName(FT_UShort platformId, FT_UShort encodingId);

FXString
FXLanguageName(FT_UShort platformId, FT_UShort languageId);

FXString
FXToString(FT_UShort platformId, FT_UShort encodingId, void * string, uint32_t stringLen);

FXString
FXSFNTName(FT_SfntName * sfnt);

FXString
FXSFNTValue(FT_SfntName * sfnt);

FXString
FXSFNTLanguage(FT_SfntName *sfnt, FT_Face face);
