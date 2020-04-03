#include "FontX/FXLog.h"
#include "FontX/FXUnicode.h"
#include "FontX/FXShaper.h"

#include "FCLDatabase.h"

namespace {
    class FCLPDumpKern: public FCLDatabaseProcessor {
    public:
        FXString
        name() const override {
            return "dump_kern";
        }

        void
        processDatabase(FXPtr<const FCLDatabase> db) override {
            FXSet<FXString> loadedFamilies;
            for (size_t i = 0; i < db->faceCount(); ++ i) {
                auto & desc = db->faceDescriptor(i);
                auto & atts = db->faceAttributes(i);
                if (loadedFamilies.count(atts.sfntNames.familyName()))
                    continue;
                loadedFamilies.insert(atts.sfntNames.familyName());
                FX_INFO(">>>> Dumping kernings of " << atts.sfntNames.postscriptName());
                dumpKern(db->createFace(desc));
            }
            
            FX_INFO("min/max kern:" << minKern << ", " << maxKern);
        }

    private:
        void dumpKern(FXPtr<FXFace> face) {
            if (face->glyphCount() > 2000) {
                FX_ERROR("Skip, too many glyphs (> 2000)");
                return;
            }
            if (face->cmaps().empty()) {
                FX_ERROR("  NO CMPAS");
                return;
            }
            auto & cm = face->currentCMap();
            if (!cm.isUnicode()) {
                FX_ERROR("   NO UNICODE CMAP");
                return;
            }
            
            FXShaper shapper(face.get());
            
            auto & blocks = cm.blocks();
            FX_INFO("  CMAP: " << cm.description());
            
            FXVector<FXChar> allChars;
            allChars.reserve(face->glyphCount());
            
            for (auto & block: blocks) {
                if (block->name() == "CJK Unified Ideographs" ||
                    block->name() == "CJK Unified Ideographs Extension B" ||
                    block->name() == "Low Surrogates" ||
                    block->name() == "Private Use Area" ||
                    block->name() == "Hangul Syllables")
                    continue; // Too big block
                for (size_t charIndex = 0; charIndex < block->size(); ++ charIndex) {
                    auto ch = block->get(charIndex);
                    if (ch.type != FXGCharTypeUnicode)
                        continue;
                    if (ch.value == 0x00AD) // SOFT HYPHEN
                        continue;
                    allChars.push_back(ch.value);
                }
            }
            
            FXMap<int, size_t> kernMap;
            
            for (size_t i = 0; i < allChars.size(); ++ i) {
                for (size_t j = i; j < allChars.size(); ++ j) {
                    if (auto kernI = shapeKern(shapper, allChars[i], allChars[j])) {
                        ++ kernMap[kernI];
                        //FX_INFO(joinChars(allChars[i], allChars[j]) << ": " << kernI);
                    }
                    
                    if (i == j)
                        continue;
                    
                    if (auto kernJ = shapeKern(shapper, allChars[j], allChars[i])) {
                        ++ kernMap[kernJ];
                        //FX_INFO(joinChars(allChars[j], allChars[i]) << ": " << kernJ);
                    }
                    
                }
            }
            
            for (auto [kern, count]: kernMap) {
                FXString s;
                if (count < 2)
                    s = "▏";
                else if (count < 3)
                    s = "▎";
                else if (count < 4)
                    s = "▍";
                else if (count < 5)
                    s = "▌";
                else if (count < 6)
                    s = "▋";
                else if (count < 7)
                    s = "▊";
                else if (count < 8)
                    s = "▉";
                else if (count < 9)
                    s = "█";
                else if (count < 10)
                    s = "▇";
                else
                for (size_t i = 0; i < int(count / 10); ++ i)
                    s += "▆";
                
                FX_INFO("    " << std::setw(5) << kern << ' ' << s);
                
                minKern = std::min(minKern, kern);
                maxKern = std::max(maxKern, kern);
            }
        }
        
        int
        shapeKern(FXShaper & shapper, FXChar a, FXChar b) {
            shapper.shape(joinChars(a, b));
            if (shapper.glyphCount() != 2)
                return 0;
            
            auto normalizeKern = [](auto face, auto kern) {
                return int(kern * 1000.0 / face->upem());
            };
            
            return normalizeKern(shapper.face(), shapper.advance(0).x - shapper.face()->glyph(a).metrics.horiAdvance);
        }
        
        
        static FXString
        joinChars(FXChar a, FXChar b) {
            return FXUnicode::utf8Str(a) + FXUnicode::utf8Str(b);
        }
        
    private:
        int  minKern {0};
        int  maxKern {0};
    };

    FCLDatabaseProcessorAutoRegister<FCLPDumpKern> dumpProcessor;
}
