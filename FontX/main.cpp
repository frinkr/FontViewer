#include "FXLib.h"
#include "FXFace.h"
#include <iostream>

std::string getFontFile(const std::string & fileName) {
    return std::string(FX_RESOURCES_DIR) + "/Fonts/" + fileName;
}

std::string getUCDRoot() {
    return std::string(FX_RESOURCES_DIR) + "/UCD";
}

int main() {
    FXLib::init(getUCDRoot()); {
        auto face = FXFace::createFace(getFontFile("MyriadPro-Regular.otf"), 0);
        assert(face);

        assert(face->face());
        assert(face->index() == 0);
        assert(face->postscriptName() == "MyriadPro-Regular");
        assert(face->upem() == 1000);

#if 0
        for (const FXCMap & cm : face->cmaps()) {
            std::cout << cm.description()  << ": " << std::endl;
            for (const auto & block : cm.blocks()) {
                std::cout << " - " << block->name() << ": " << block->size() << std::endl;
            };
        }
#endif

        FXCMap cm = face->currentCMap();
        assert(cm.isUnicode());
        for (FXPtr<FXCharBlock> block : cm.blocks()) {
            std::cout << "BLOCK " << block->name() << ": " << block->size() << std::endl;
            size_t count = block->size();
            for (size_t i = 0; i < count; ++ i) {
                FXChar c = block->get(i);
                FXGlyph glyph = face->glyph(c);
                std::cout << " - char " << std::hex << c << std::dec
                          << ": id " << glyph.gid
                          << ", lsb: " << glyph.metrics.lsb()
                          << ", rsb: " << glyph.metrics.rsb()
                          << std::endl;
            }
        }
        
        
    } FXLib::finish();
    return 0;
}
