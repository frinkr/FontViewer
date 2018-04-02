#include "FXLib.h"
#include "FXFace.h"

std::string getFontFile(const std::string & fileName) {
    return std::string(FX_FONT_DIR) + "/" + fileName;
}

int main() {
    FXLib::init();
    auto face = FXFace::createFace(getFontFile("MyriadPro-Regular.otf"), 0);
    assert(face);

    assert(face->face());
    assert(face->index() == 0);
    assert(face->postscriptName() == "MyriadPro-Regular");
    FXLib::finish();
    return 0;
}
