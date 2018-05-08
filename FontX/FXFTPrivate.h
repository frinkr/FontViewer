#pragma once
#include "FX.h"
#include "FXFT.h"

FT_Error
FXFilePathToOpenArgs(const FXString & filePath,
                     FT_Open_Args * args);

FT_Error
FXFTCountFaces(FXFTLibrary lib,
               const FXString & filePath,
               size_t & count);

FT_Error
FXFTEnumurateFaces(FXFTLibrary lib,
                   const FXString & filePath,
                   std::function<bool(FXFTFace face, size_t index)> callback);

FT_Error
FXFTOpenFace(FXFTLibrary lib,
             const FXString & filePath,
             size_t index,
             FXFTFace * face);
