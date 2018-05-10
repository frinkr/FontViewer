#include "FXFace.h"
#include "FXShaper.h"
#include "FXFTPrivate.h"
#include "FXHBPrivate.h"

struct FXShaperImp {
    explicit FXShaperImp(FXFace * face)
        : face_ (face)
        , hbFont_(nullptr)
        , hbFace_(nullptr)
        , hbBuffer_(nullptr)
        , hbPlan_(nullptr)
        , hbGlyphCount_(0)
        , hbGlyphInfos_(nullptr)
        , hbGlyphPositions_(nullptr) {
    }

    ~FXShaperImp() {
        reset();
        hb_font_destroy(hbFont_);
    }
    
    void reset() {
        if (hbPlan_)
            hb_shape_plan_destroy(hbPlan_);
        hbPlan_ = nullptr;
        
        if (hbBuffer_)
            hb_buffer_destroy(hbBuffer_);
        hbBuffer_ = nullptr;

        hbGlyphCount_ = 0;
        hbGlyphInfos_ = nullptr;
        hbGlyphPositions_ = nullptr;
    }
    
    void
    shape(const FXString & text,
          FXTag script,
          FXTag language,
          FXShapingDirection direction,
          const FXVector<FXTag> & onFeatures,
          const FXVector<FXTag> & offFeatures) {

        reset();

        if (!hbFont_ || !hbFace_)
            FXHBCreateFontFace(face_, &hbFont_, &hbFace_);
        
        // setup the buffer
        hbBuffer_ = hb_buffer_create();
        if (script == FXTagOTDefaultScript)
            hb_buffer_set_script(hbBuffer_, HB_SCRIPT_COMMON);
        else
            hb_buffer_set_script(hbBuffer_, hb_ot_tag_to_script(script));
        hb_buffer_set_language(hbBuffer_, hb_ot_tag_to_language(language));
        hb_buffer_set_direction(hbBuffer_, (hb_direction_t)direction);
        hb_buffer_add_utf8(hbBuffer_, text.c_str(), text.length(), 0, text.length());
    
        std::vector<hb_feature_t> featuresVec;
        for (FXTag t : onFeatures) {
            hb_feature_t f {t, 1/*on*/, 0, (unsigned int)-1};
            featuresVec.push_back(f);
        }
        for (FXTag t :  offFeatures) {
            hb_feature_t f {t, 0/*off*/, 0, (unsigned int)-1};
            featuresVec.push_back(f);
        }

        hb_segment_properties_t segment_props;
        hb_buffer_guess_segment_properties(hbBuffer_);
        hb_buffer_get_segment_properties(hbBuffer_, &segment_props);
    
        // shape
        const char ** shappers = hb_shape_list_shapers();
    
        hbPlan_ = hb_shape_plan_create_cached(hbFace_,
                                              &segment_props,
                                              &featuresVec[0],
                                              featuresVec.size(),
                                              shappers);
    
        hb_shape_plan_execute(hbPlan_,
                              hbFont_,
                              hbBuffer_,
                              &featuresVec[0],
                              featuresVec.size());
    

        // get result
        hbGlyphInfos_ = hb_buffer_get_glyph_infos(hbBuffer_, &hbGlyphCount_);
        hbGlyphPositions_ = hb_buffer_get_glyph_positions(hbBuffer_, &hbGlyphCount_);
    }

    FXFace * face_;
    
    hb_font_t * hbFont_;
    hb_face_t * hbFace_;
    hb_buffer_t * hbBuffer_;
    hb_shape_plan_t * hbPlan_;
    
    unsigned int  hbGlyphCount_;
    hb_glyph_info_t * hbGlyphInfos_;
    hb_glyph_position_t * hbGlyphPositions_;
};


FXShaper::FXShaper(FXFace * face)
    : imp_(new FXShaperImp(face)){}
    
void
FXShaper::shape(const FXString & text,
                FXTag script,
                FXTag language,
                FXShapingDirection direction,
                const FXVector<FXTag> & onFeatures,
                const FXVector<FXTag> & offFeatures) {
    return imp_->shape(text, script, language, direction, onFeatures, offFeatures);
}

size_t
FXShaper::glyphCount() const {
    return imp_->hbGlyphCount_;
}

FXGlyphID
FXShaper::glyph(size_t index) const {
    return imp_->hbGlyphInfos_[index].codepoint;
}

FXVec2d<fu>
FXShaper::advance(size_t index) const {
    return FXMakeVec2d<fu>(
        imp_->hbGlyphPositions_[index].y_advance,
        imp_->hbGlyphPositions_[index].x_advance);
}

FXVec2d<fu>
FXShaper::offset(size_t index) const {
    return FXMakeVec2d<fu>(
        imp_->hbGlyphPositions_[index].y_offset,
        imp_->hbGlyphPositions_[index].x_offset);

}
