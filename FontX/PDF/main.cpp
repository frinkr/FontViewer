#include "FontX/FXFace.h"
#include "FontX/FXLib.h"
#include "FontX/FXLog.h"
#include "FXPDFFace.h"
#include "FXPDFDocument.h"

#include <iostream>

std::string getFontFile(const std::string & fileName) {
    return std::string(FX_RESOURCES_DIR) + "/Fonts/" + fileName;
}

std::string getPDFFile(const std::string & fileName) {
    return std::string(FX_RESOURCES_DIR) + "/PDFs/" + fileName;
}

std::string getUCDRoot() {
    return std::string(FX_RESOURCES_DIR) + "/UCD";
}

int main() {
    FXLib::init(getUCDRoot()); {
        auto doc = std::make_shared<FXPDFDocument>(getPDFFile("print.pdf"));
        if (doc->open()) {
            for (size_t i = 0; i < doc->fontCount(); ++ i) {
                FX_INFO("Font " << i);

                auto info = doc->fontInfo(i); {
                    FX_INFO("\t   info.baseFont = " << info.baseFont);
                    FX_INFO("\t   info.subType  = " << info.subType);
                    FX_INFO("\t   info.isSubset = " << info.isSubset);
                }
                
                FXPtr<FXFace> face = doc->createFace(i);
                if (face) {
                    
                    FX_INFO("\t   face: UPEM: " << face->upem());
                    FX_INFO("\t   face: PSName: " << face->postscriptName());
                    FX_INFO("\t   face: GlyphCount: " << face->glyphCount());
                    FX_INFO("\t   face: Format: " << face->attributes().format);
                    FX_INFO("\t   face: Family Name: " << face->attributes().sfntNames.familyName());
                    FX_INFO("\t   face: Style Name: " << face->attributes().sfntNames.styleName());
                }
                else {
                    FX_ERROR("\t   Failed to load font " << i);
                }
            }
            
        }
    } FXLib::finish();
    return 0;
}
