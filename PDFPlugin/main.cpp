#include "FontX/FXLib.h"
#include "FontX/FXFace.h"
#include "FXPDF.h"

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
        FXPDFDocument doc(getPDFFile("Lorem.pdf"));
        doc.open();
    } FXLib::finish();
    return 0;
}
