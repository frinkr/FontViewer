#include "FontX/FXLog.h"
#include "FontX/FXUnicode.h"
#include "FontX/FXShaper.h"

#include "FCLDatabase.h"


namespace {
    class FCLDumpD: public FCLDatabaseProcessor {
    public:
        FXString
        name() const override {
            return "dump_d";
        }

        void
        processDatabase(FXPtr<const FCLDatabase> db) override {
            for (size_t i = 0; i < db->faceCount(); ++ i) {
                auto & desc = db->faceDescriptor(i);
                auto & atts = db->faceAttributes(i);
                dump_d(db->createFace(desc));
            }
            
        }

    private:
        void dump_d(FXPtr<FXFace> face) {
            if (face->cmaps().empty() || !face->currentCMap().isUnicode())
                return;
            if (!face->upem())
                return;
            if (auto glyph = face->glyph('d'); glyph.gid != FXGIDNotDef) {
                FX_INFO(face->postscriptName() << ": " <<  nomalizeFontUnit(face, glyph.metrics.horiBearingY));
            }
        }
        int nomalizeFontUnit(FXPtr<FXFace> face, fu f) {
            return static_cast<int>(std::round(f * 1000.0 / face->upem()));
        }
        
        
    private:
        int  minKern {0};
        int  maxKern {0};
    };

    FCLDatabaseProcessorAutoRegister<FCLDumpD> dumpProcessor;
}



namespace {
    class FCLDumpR: public FCLDatabaseProcessor {
    public:
        FXString
        name() const override {
            return "dump_r";
        }

        void
        processDatabase(FXPtr<const FCLDatabase> db) override {
            if (auto desc1 = db->findDescriptor("MyriadPro-Regular")) {   
                if (auto desc2 = db->findDescriptor("Alphabeta")) {
                    auto face1 = db->createFace(*desc1);
                    auto face2 = db->createFace(*desc2);

                    for (char c = 32; c < 127; ++ c) {
                        auto g1 = face1->glyph(c);
                        auto g2 = face2->glyph(c);
                        if (g1.gid != FXGIDNotDef && g2.gid != FXGIDNotDef) {
                            FX_INFO(c
                                    << ", HT: " << div(g1.metrics.height, g2.metrics.height)
                                    << ", BY: " << div(g1.metrics.horiBearingY, g2.metrics.horiBearingY)
                                    );
                            
                        }
                    }
                }
            }

            
        }

    private:
        
        template <typename T>
        static double div(T a, T b) {
            return double(a) / b;
        }
        int nomalizeFontUnit(FXPtr<FXFace> face, fu f) {
            return static_cast<int>(std::round(f * 1000.0 / face->upem()));
        }
        
    };

    FCLDatabaseProcessorAutoRegister<FCLDumpR> dumpRProcessor;
}
