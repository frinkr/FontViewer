#include "FontX/FXLog.h"
#include "FontX/FXShaper.h"
#include "FontX/FXUnicode.h"
#include "FCLDatabase.h"

namespace {
    class FCLShaping: public FCLDatabaseProcessor {
        FXString
        name() const override {
            return "shaping";
        }

        void
        processDatabase(FXPtr<const FCLDatabase> db) override {
            if (auto desc = db->findDescriptor("AdobeArabic-Regular")) {
                auto face = FXFace::createFace(*desc);

                const std::u16string text = u"\x0634\x0632";
                
                FXShaper shaper(face.get());
//                shaper.shape(FXUnicode::utf16ToUTF8(text),
//                             FXOT::DEFAULT_SCRIPT,
//                             FXOT::DEFAULT_LANGUAGE,
//                             FXShapingLTR,
//                             FXShapingBidiOptions{.bidiActivated = true});
            }

            
        }
    };

    FCLDatabaseProcessorAutoRegister<FCLShaping> sProcessor;
}
