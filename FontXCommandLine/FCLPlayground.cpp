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
            for (size_t i = 0; i < db->faceCount(); ++ i) {
                auto & desc = db->faceDescriptor(i);
                auto face = FXFace::createFace(desc);
                if (face->currentCMap().isUnicode()) {
                    if (face->glyph(' ').gid == 0)
                        FX_INFO("no sp: " << desc.index << " - " << desc.filePath << ": " << db->faceAttributes(i).sfntNames.postscriptName());
                }
                else
                    FX_INFO("no unicm: " << desc.index << " - " << desc.filePath << ": " << db->faceAttributes(i).sfntNames.postscriptName());
            }
        }

    };

    FCLDatabaseProcessorAutoRegister<FCLPlayground> playground;
}
