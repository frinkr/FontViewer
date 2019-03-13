#include "FXLib.h"
#include "FXFace.h"
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
        auto face = FXFace::createFace(getFontFile("SourceSansVariable-Roman.otf"), 0);
        FXGlyph g = face->glyph(FXGChar('a'));
        assert(face);
        return;

        assert(face->face());
        assert(face->index() == 0);
        assert(face->postscriptName() == "SourceSansVariable-Roman");
        assert(face->upem() == 1000);

        FXCMap cm = face->currentCMap();
        assert(cm.isUnicode());
        for (FXPtr<FXGCharBlock> block : cm.blocks()) {
            std::cout << "BLOCK " << block->name() << ": " << block->size() << std::endl;
            size_t count = block->size();
            for (size_t i = 0; i < count; ++ i) {
                FXGChar c = block->get(i);
                FXGlyph glyph = face->glyph(c);
                std::cout << " - char " << std::hex << c.value << std::dec
                          << ": id " << glyph.gid
                          << ", lsb: " << glyph.metrics.lsb()
                          << ", rsb: " << glyph.metrics.rsb()
                          << std::endl;
            }
        }
    } FXLib::finish();
    return 0;
}
