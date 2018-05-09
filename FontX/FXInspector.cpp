#include <hb.h>
#include <hb-ft.h>
#include <hb-ot.h>

#include "FXFace.h"
#include "FXInspector.h"
#include "FXFTPrivate.h"

namespace {
    struct FXOTLookup {
    
    };

    struct FXOTFeature {
        FXTag tag;
        FXVector<FXOTLookup> lookups;
    };

    struct FXOTLanguage {
        FXTag tag;
        FXVector<FXOTFeature> features;
    };

    struct FXOTScript {
        FXTag tag;
        FXVector<FXTag>  languages;    
    };

    struct FXOTTable {
        FXTag tag;
        FXVector<FXOTScript> scripts;
    };

    
    
}

struct FXInspectorImp {

    explicit FXInspectorImp(FXFace * face)
        : face_(face) {
        init();
    }
    void
    init() {
        FT_Face ftFace = face_->face();
        
        // Create hb font and face
        hb_face_t * hbFace = hb_ft_face_create_referenced(ftFace);
        hb_font_t * hbFont = hb_ft_font_create_referenced(ftFace);
    
        hb_ot_font_set_funcs(hbFont);
        unsigned int upem = hb_face_get_upem(hbFace);
        hb_font_set_scale(hbFont, upem, upem);
    }

    FXFace  * face_;
};

FXInspector::FXInspector(FXFace * face)
    : imp_(new FXInspectorImp(face)) {
}

