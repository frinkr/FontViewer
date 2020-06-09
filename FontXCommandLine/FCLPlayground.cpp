#include "FontX/FXLog.h"
#include "FCLDatabase.h"

namespace {
    class FCLPlayground: public FCLDatabaseProcessor {
        FXString
        name() const override {
            return "playground";
        }

        void
        processDatabase(FXPtr<const FCLDatabase> db) override {
            
            {
                auto face = FXFace::createFace("/Users/frinkr/Library/Fonts/HeiseiKakuGo W5", 0);
                face->currentCMap();
                
            }
            
            for (size_t i = 0; i < db->faceCount(); ++ i) {
                auto & desc = db->faceDescriptor(i);
                auto face = FXFace::createFace(desc);
#if 1
                // Find the font which doesn't have the unicode cmap, or the unicode cmap doesn't contains the char space.
                if (face->currentCMap().isUnicode()) {
                    if (face->glyph(' ').gid == 0)
                        ;//FX_INFO("no sp: " << desc.index << " - " << desc.filePath << ": " << db->faceAttributes(i).sfntNames.postscriptName());
                }
                else
                    FX_INFO("no unicm: " << desc.index << " - " << desc.filePath << ": " << db->faceAttributes(i).sfntNames.postscriptName());
#elif 0
                // Find the font which have MacRomon encoding but no Unicode
                if (!face->currentCMap().isUnicode()) {
                    for (auto & cm: face->cmaps()) {
                        if (cm.platformID() == 1) {
                            FX_INFO(desc.index << " - " << desc.filePath);
                        }
                    }
                }
#elif 1
                // Print the cmaps of font which doesn't have an Unicode cmap
                if (!face->currentCMap().isUnicode()) {
                    for (auto & cm: face->cmaps()) {
                        FX_INFO(desc.index << " - " << desc.filePath << ": " << cm.description());
                    }
                }
#endif
            }
        }

    };

    FCLDatabaseProcessorAutoRegister<FCLPlayground> playground;
}
