#include "FontX/FXLog.h"
#include "FCLDatabase.h"

namespace {
    class FCLPDump: public FCLDatabaseProcessor {
        FXString
        name() const override {
            return "dump";
        }

        void
        processDatabase(FXPtr<const FCLDatabase> db) const override {
            for (size_t i = 0; i < db->faceCount(); ++ i) {
                auto & atts = db->faceAttributes(i);
                FX_INFO(atts.sfntNames.postscriptName());
            }
        }
    };

    FCLDatabaseProcessorAutoRegister<FCLPDump> dumpProcessor;
}
