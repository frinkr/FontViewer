#include "FXFace.h"
#include "FXShaper.h"
#include "FXUnicode.h"
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
        if(hasFallbackShaping_) {
            delete [] hbGlyphInfos_;
            delete [] hbGlyphPositions_;
        }
        
        if (hbPlan_)
            hb_shape_plan_destroy(hbPlan_);
        hbPlan_ = nullptr;
        
        if (hbBuffer_)
            hb_buffer_destroy(hbBuffer_);
        hbBuffer_ = nullptr;

        hbGlyphCount_ = 0;
        hbGlyphInfos_ = nullptr;
        hbGlyphPositions_ = nullptr;

        hasFallbackShaping_ = false;
    }
    
    void
    shape(const FXString & text,
          FXTag script,
          FXTag language,
          FXShapingDirection direction,
          const FXVector<FXTag> & onFeatures,
          const FXVector<FXTag> & offFeatures) {

        reset();

        if (text.empty())
            return;
        
        if (!hbFont_ || !hbFace_)
            FXHBCreateFontFace(face_, &hbFont_, &hbFace_);
        
        // setup the buffer
        hbBuffer_ = hb_buffer_create();
        if (script == FXOT::DEFAULT_SCRIPT)
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
    
        hb_feature_t * features = nullptr;
        unsigned int featuresCount = 0;
        if (featuresVec.size()) {
            features = &featuresVec[0];
            featuresCount = featuresVec.size();
        }
        hbPlan_ = hb_shape_plan_create_cached(hbFace_,
                                              &segment_props,
                                              features,
                                              featuresCount,
                                              shappers);
    
        hb_shape_plan_execute(hbPlan_,
                              hbFont_,
                              hbBuffer_,
                              features,
                              featuresCount);
    

        // get result
        hbGlyphInfos_ = hb_buffer_get_glyph_infos(hbBuffer_, &hbGlyphCount_);
        hbGlyphPositions_ = hb_buffer_get_glyph_positions(hbBuffer_, &hbGlyphCount_);

        if (shouldFallback()) {
            hasFallbackShaping_ = true;
            fallbackShape(text);
        }
        else {
            hasFallbackShaping_ = false;
        }
    }

    bool
    shouldFallback() const {
        if (!hbGlyphCount_)
            return true;
        
        for (size_t i = 0; i < hbGlyphCount_; ++ i) {
            if (hbGlyphInfos_[i].codepoint) // not undef
                return false;
        }
        return true;
    }

    void
    fallbackShape(const FXString & text) {
        auto u32 = FXUnicode::utf8ToUTF32(text);
        hbGlyphInfos_ = new hb_glyph_info_t[u32.size()];
        hbGlyphPositions_ = new hb_glyph_position_t[u32.size()];
        for (size_t i = 0; i < u32.size(); ++ i) {
            const FXGChar ch(u32[i]);
            FXGlyph g = face_->glyph(ch);
            hbGlyphInfos_[i].cluster = i;
            hbGlyphInfos_[i].codepoint = g.gid;
            hbGlyphPositions_[i].x_advance = g.metrics.horiAdvance;
            hbGlyphPositions_[i].y_advance = g.metrics.vertAdvance;
            hbGlyphPositions_[i].x_offset = 0;
            hbGlyphPositions_[i].y_offset = 0;
        }
    }

    FXFace * face_;

    bool hasFallbackShaping_ {false};
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
        imp_->hbGlyphPositions_[index].x_advance,
        imp_->hbGlyphPositions_[index].y_advance);
}

FXVec2d<fu>
FXShaper::offset(size_t index) const {
    return FXMakeVec2d<fu>(
        imp_->hbGlyphPositions_[index].x_offset,
        imp_->hbGlyphPositions_[index].y_offset);

}

FXFace *
FXShaper::face() const {
    return imp_->face_;
}
    
bool
FXShaper::hasFallbackShaping() const {
    return imp_->hasFallbackShaping_;
}
