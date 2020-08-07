#include "FXFace.h"
#include "FXShaper.h"
#include "FXUnicode.h"
#include "FXFTPrivate.h"
#include "FXHBPrivate.h"
#include <fribidi/fribidi.h>

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
    
    
    struct FXBidiRun {
        size_t textBegin;
        size_t textEnd;
        hb_buffer_t *buffer;
        hb_direction_t direction;
        hb_script_t script;
        unsigned int glyphCount;
        hb_glyph_info_t *glyphInfos;
        hb_glyph_position_t *glyphPositions;
    };
    
    void
    shape(const FXString & text,
          FXTag script,
          FXTag language,
          FXShapingDirection direction,
          FXShappingBidiOptions bidiOpts,
          const FXVector<FXTag> & onFeatures,
          const FXVector<FXTag> & offFeatures) {

        reset();

        if (text.empty())
            return;
        
        if (!hbFont_ || !hbFace_)
            FXHBCreateFontFace(face_, &hbFont_, &hbFace_);
        
        if (bidiOpts.bidiActivated) {
            
            // Let's do bidi
            std::u32string u32Text = FXUnicode::utf8ToUTF32(text);
            auto u32BidiText = (const FriBidiChar*)(u32Text.c_str());
            auto u32BidiTextLength = u32Text.size();
            
            static_assert(sizeof(FriBidiChar) == sizeof(typename std::u32string::value_type));
            
            FriBidiParType bidiParType = FRIBIDI_PAR_ON;
            switch (bidiOpts.direction) {
                case FXBidiDirection::LTR: bidiParType = FRIBIDI_PAR_LTR; break;
                case FXBidiDirection::RTL: bidiParType = FRIBIDI_PAR_RTL; break;
                case FXBidiDirection::AUTO: bidiParType = FRIBIDI_PAR_ON; break;
            }
            
            FXVector<FriBidiCharType> bidiTypes(u32BidiTextLength, FRIBIDI_TYPE_LTR);
            FXVector<FriBidiLevel> bidiLevels(u32BidiTextLength, 0);
            FXVector<hb_script_t> hbScripts(u32BidiTextLength, HB_SCRIPT_COMMON);
            
            fribidi_get_bidi_types(u32BidiText, u32BidiTextLength, &bidiTypes[0]);
            fribidi_get_par_embedding_levels(&bidiTypes[0], u32BidiTextLength, &bidiParType, &bidiLevels[0]);
            for (size_t i = 0; i < u32BidiTextLength; ++ i)
                hbScripts[i] = hb_unicode_script(hb_unicode_funcs_get_default(), u32BidiText[i]);
            
            if (bidiOpts.resolveScripts) {
                hb_script_t lastScriptValue;
                int lastScriptIndex = -1;
                int lastSetIndex = -1;

                for (int i = 0; i < u32BidiTextLength; ++i) {
                    if (hbScripts[i] == HB_SCRIPT_COMMON || hbScripts[i] == HB_SCRIPT_INHERITED) {
                        if (lastScriptIndex != -1) {
                            hbScripts[i] = lastScriptValue;
                            lastSetIndex = i;
                        }
                    } else {
                        for (int j = lastSetIndex + 1; j < i; ++j)
                            hbScripts[j] = hbScripts[i];
                        lastScriptValue = hbScripts[i];
                        lastScriptIndex = i;
                        lastSetIndex = i;
                    }
                }
            }
            
            
            // Split the text by script and bidi level
            size_t lastCharIndex = 0;
            FXVector<FXBidiRun> bidiRuns;
            for (size_t i = 0; i <= u32BidiTextLength; ++ i) {
                if (i == u32BidiTextLength
                    || (bidiOpts.breakOnScriptChange && hbScripts[i] != hbScripts[lastCharIndex])
                    || (bidiOpts.breakOnLevelChange && bidiLevels[i] != bidiLevels[lastCharIndex]))
                {
                    FXBidiRun run;
                    run.textBegin = lastCharIndex;
                    run.textEnd = i;
                    run.script = hbScripts[lastCharIndex];
                    run.direction = bidiLevels[lastCharIndex] & 1? HB_DIRECTION_RTL: HB_DIRECTION_LTR;
                    
                    bidiRuns.push_back(run);
                    lastCharIndex = i;
                }
            }
            
            // Shape
            for (size_t i = 0; i < bidiRuns.size(); ++ i) {
                auto & bidiRun = bidiRuns[i];
                auto buffer = hb_buffer_create();
                bidiRun.buffer = buffer;
                hb_buffer_set_direction(buffer, bidiRun.direction);
                hb_buffer_set_script(buffer, bidiRun.script);
                hb_buffer_add_utf32(buffer,
                                    u32BidiText + bidiRun.textBegin,
                                    bidiRun.textEnd - bidiRun.textBegin, 0,
                                    bidiRun.textEnd - bidiRun.textBegin);
                
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
                hb_buffer_guess_segment_properties(buffer);
                hb_buffer_get_segment_properties(buffer, &segment_props);
            
                const char ** shappers = hb_shape_list_shapers();
            
                hb_feature_t * features = nullptr;
                unsigned int featuresCount = 0;
                if (false && featuresVec.size()) {
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
                                      buffer,
                                      features,
                                      featuresCount);
                

                // get result
                bidiRun.glyphInfos = hb_buffer_get_glyph_infos(buffer, &bidiRun.glyphCount);
                bidiRun.glyphPositions = hb_buffer_get_glyph_positions(buffer, &bidiRun.glyphCount);
            }
            
            
            // Copy glyphs
            size_t totalGlyphCount = 0;
            for (auto & run : bidiRuns)
                totalGlyphCount += run.glyphCount;
            
            delete [] hbGlyphInfos_;
            delete [] hbGlyphPositions_;
            hbGlyphCount_ = 0;
            
            
            auto newCodePoints = std::vector<uint32_t>(totalGlyphCount, 0);
            auto newGlyphIndices = std::vector<int>(totalGlyphCount, 0);
            auto newTypes = std::vector<FriBidiCharType>(totalGlyphCount, 0);
            auto newScripts = std::vector<hb_script_t>(totalGlyphCount, HB_SCRIPT_COMMON);
            auto newLevels = std::vector<FriBidiLevel>(totalGlyphCount, 0);
            
            FXVector<hb_glyph_info_t> hbGlyphInfos(totalGlyphCount, hb_glyph_info_t{});
            FXVector<hb_glyph_position_t> hbGlyphPositions(totalGlyphCount, hb_glyph_position_t{});
            size_t glyphIndexBase = 0;
            for (size_t i = 0; i < bidiRuns.size(); ++ i) {
                auto & bidiRun = bidiRuns[i];
                
                for (size_t j = 0; j < bidiRun.glyphCount; ++ j) {
                    auto gIndex = bidiRun.direction == HB_DIRECTION_LTR? j : (bidiRun.glyphCount - 1 - j);
                    
                    auto sourceIndex = bidiRun.glyphInfos[gIndex].cluster + bidiRun.textBegin;
                    
                    newGlyphIndices[glyphIndexBase + j] = bidiRun.glyphInfos[gIndex].codepoint;
                    newScripts[glyphIndexBase + j] = hbScripts[sourceIndex];
                    newTypes[glyphIndexBase + j] = bidiTypes[sourceIndex];
                    newLevels[glyphIndexBase + j] = bidiLevels[sourceIndex];
                    
                    hbGlyphInfos[glyphIndexBase + j] = bidiRun.glyphInfos[gIndex];
                    hbGlyphPositions[glyphIndexBase + j] = bidiRun.glyphPositions[gIndex];
                }
                glyphIndexBase += bidiRun.glyphCount;
            }
            
            
            // Reorder glyphs
            
            FXVector<FriBidiStrIndex> map(u32BidiTextLength, 0);
            for (auto i = 0; i < u32BidiTextLength; ++ i)
                map[i] = i;
            
            fribidi_reorder_line(0, &newTypes[0], totalGlyphCount, 0, bidiParType, &newLevels[0], 0, &map[0]);
            
            hbGlyphCount_ = totalGlyphCount;
            hbGlyphInfos_ = new hb_glyph_info_t[totalGlyphCount];
            hbGlyphPositions_ = new hb_glyph_position_t[totalGlyphCount];
            for (auto i = 0; i < totalGlyphCount; ++ i) {
                hbGlyphInfos_[i] = hbGlyphInfos[map[i]];
                hbGlyphPositions_[i] = hbGlyphPositions[map[i]];
            }
            
            return;
            
            
            
        }
        
        
        //////////////////////////////////////////////////////////////////////////////////////
        
        
        
        
        
        
        
        
        
        // setup the buffer
        hbBuffer_ = hb_buffer_create();
        if (script == FXOT::DEFAULT_SCRIPT)
            hb_buffer_set_script(hbBuffer_, HB_SCRIPT_COMMON);
        else
            hb_buffer_set_script(hbBuffer_, hb_ot_tag_to_script(script));
        hb_buffer_set_language(hbBuffer_, hb_ot_tag_to_language(language));
        hb_buffer_set_direction(hbBuffer_, (hb_direction_t)direction);
        hb_buffer_add_utf8(hbBuffer_, text.c_str(), text.length(), 0, text.length());
        //hb_buffer_add_utf32(hbBuffer_, (const uint32_t *)u32Bidi.c_str(), u32Bidi.length(), 0, u32Bidi.length());
    
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

    FXFace * face_{};

    bool hasFallbackShaping_ {false};
    hb_font_t * hbFont_ {};
    hb_face_t * hbFace_ {};
    hb_buffer_t * hbBuffer_{};
    hb_shape_plan_t * hbPlan_{};
    
    unsigned int  hbGlyphCount_ {};
    hb_glyph_info_t * hbGlyphInfos_ {};
    hb_glyph_position_t * hbGlyphPositions_ {};

};


FXShaper::FXShaper(FXFace * face)
    : imp_(new FXShaperImp(face)){}
    
void
FXShaper::shape(const FXString & text,
                FXTag script,
                FXTag language,
                FXShapingDirection direction,
                FXShappingBidiOptions bidiOpts,
                const FXVector<FXTag> & onFeatures,
                const FXVector<FXTag> & offFeatures) {
    return imp_->shape(text, script, language, direction, bidiOpts, onFeatures, offFeatures);
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
