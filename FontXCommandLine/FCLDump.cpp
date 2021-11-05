#include "FontX/FXLog.h"
#include "FCLDatabase.h"

namespace {
    class FCLDump: public FCLDatabaseProcessor {
        FXString
        name() const override {
            return "dump";
        }

        void
        processDatabase(FXPtr<const FCLDatabase> db) override {

            {
                auto face = FXFace::createFace("/System/Library/Fonts/Supplemental/Apple Chancery.ttf", 0);
                auto insp = face->inspector();
                
            }

            
            for (size_t i = 0; i < db->faceCount(); ++ i) {
                auto & desc = db->faceDescriptor(i);
                dumpFace(db->createFace(desc));
            }
                
            FX_INFO("====== BIGGEST BLOCK ===== ");
            FX_INFO("  name: " << biggestBlockName);
            FX_INFO("  size: " << biggestBlockSize);
                
            FX_INFO("====== MOST GLYPH FACE ===== ");
            FX_INFO("  name: " << mostGlyphFontName);
            FX_INFO("  size: " << mostGlyphCount);

        }

    private:
        void dumpFace(FXPtr<FXFace> face) {
            if (face->glyphCount() > mostGlyphCount) {
                mostGlyphCount = face->glyphCount();
                mostGlyphFontName = face->postscriptName();
            }
                
            auto & blocks = face->currentCMap().blocks();
            for (auto & block: blocks) {
                if (block->size() > biggestBlockSize) {
                    biggestBlockSize = block->size();
                    biggestBlockName = face->postscriptName() + ": " + block->name();
                }
            }
        }
            
    private:
        size_t   mostGlyphCount {};
        FXString mostGlyphFontName {};
        size_t   biggestBlockSize {0};
        FXString biggestBlockName {};
    };

    FCLDatabaseProcessorAutoRegister<FCLDump> dumpProcessor;
}
