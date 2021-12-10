#include "FXFace.h"
#include "FXShaper.h"
#include "FXUnicode.h"
#include "FXFTPrivate.h"
#include "FXHBPrivate.h"
#include <fribidi/fribidi.h>

#include <unicode/uchar.h>
#include <unicode/uscript.h>
#include <unicode/unorm2.h>
#include <unicode/unistr.h>
#include <unicode/ubidi.h>

struct FXShaperImp {
    explicit FXShaperImp(FXFace * face)
        : face_ (face)
        , hbFont_(nullptr)
        , hbFace_(nullptr)
        , hbBuffer_(nullptr)
        , hbGlyphCount_(0)
    {
    }

    ~FXShaperImp() {
        reset();
        hb_font_destroy(hbFont_);
    }
    
    void reset() {
        
        if (hbBuffer_)
            hb_buffer_destroy(hbBuffer_);
        hbBuffer_ = nullptr;

        hbGlyphCount_ = 0;

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
          const FXShapingGenralOptions & opts,
          const FXShapingBidiOptions & bidiOpts) {

        reset();

        if (text.empty())
            return;

        const std::u32string u32Text = FXUnicode::utf8ToUTF32(text);
        const auto haveEncodedGID = std::any_of(u32Text.begin(), u32Text.end(), [](auto c) { return FXCharIsEncodedGID(c);});

        if (haveEncodedGID && !opts.forceShapeGIDEncodedText) {
            hasFallbackShaping_ = true;
            fallbackShape(text);
            return addGlyphSpacing(opts.glyphSpacing);
        }
        
        if (!hbFont_ || !hbFace_)
            FXHBCreateFontFace(face_, &hbFont_, &hbFace_);
        
        if (bidiOpts.bidiActivated) {
            // Let's do bidi
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
            
            {
                
                FriBidiParType paraDir = FRIBIDI_PAR_RTL;
                
                FriBidiChar logStr[] = {0x0630, 0x062a, ' ', '9', '%'};
                FriBidiChar visStr[] = {'%', '9', ' ', 0x062a, 0x0630};
                size_t strLen = sizeof(logStr)/sizeof(logStr[0]);
                std::vector<FriBidiCharType> logBidiTypes(strLen, FRIBIDI_TYPE_LTR);
                std::vector<FriBidiLevel>    logBidiLevels(strLen, 0);
                std::vector<FriBidiCharType> visBidiTypes = logBidiTypes;
                std::vector<FriBidiLevel>    visBidiLevels = logBidiLevels;
                
                fribidi_get_bidi_types(logStr, strLen, logBidiTypes.data());
                fribidi_get_bidi_types(visStr, strLen, visBidiTypes.data());
                auto L0 = fribidi_get_par_embedding_levels(logBidiTypes.data(), logBidiTypes.size(), &paraDir, logBidiLevels.data());
                auto L1 = fribidi_get_par_embedding_levels(visBidiTypes.data(), visBidiTypes.size(), &paraDir, visBidiLevels.data());
                
                std::vector<FriBidiChar> logReorderedStr(strLen, 0);
                for (size_t i = 0; i < strLen; ++ i) logReorderedStr[i] = logStr[i];
                std::vector<FriBidiStrIndex> logMap(strLen, 0);
                for (size_t i = 0; i < strLen; ++ i) logMap[i] = i;
                auto R0 = fribidi_reorder_line(0, logBidiTypes.data(), logBidiTypes.size(), 0, paraDir, logBidiLevels.data(), logReorderedStr.data(), logMap.data());

                std::vector<FriBidiChar> visReorderedStr(strLen, 0);
                for (size_t i = 0; i < strLen; ++ i) visReorderedStr[i] = visStr[i];
                std::vector<FriBidiStrIndex> visMap(strLen, 0);
                for (size_t i = 0; i < strLen; ++ i) visMap[i] = i;
                auto R1 = fribidi_reorder_line(0, visBidiTypes.data(), visBidiTypes.size(), 0, paraDir, visBidiLevels.data(), visReorderedStr.data(), visMap.data());



                // icu
                std::vector<UBiDiLevel> uLogBidiLevels(strLen, 0), uVisBidiLevels(strLen, 0);
                std::vector<int32_t> uLogMap(strLen, 0), uVisMap(strLen, 0);
                for (size_t i = 0; i < strLen; ++ i) uLogBidiLevels[i] = logBidiLevels[i];
                for (size_t i = 0; i < strLen; ++ i) uVisBidiLevels[i] = visBidiLevels[i];
                for (size_t i = 0; i < strLen; ++ i) uLogMap[i] = i;
                for (size_t i = 0; i < strLen; ++ i) uVisMap[i] = i;

                ubidi_reorderVisual(uLogBidiLevels.data(), uLogBidiLevels.size(), uLogMap.data());
                ubidi_reorderVisual(uVisBidiLevels.data(), uVisBidiLevels.size(), uVisMap.data());
                
                assert(logBidiTypes.size() == visBidiTypes.size());
            }
            
            
            fribidi_get_bidi_types(u32BidiText, u32BidiTextLength, &bidiTypes[0]);
            fribidi_get_par_embedding_levels(&bidiTypes[0], u32BidiTextLength, &bidiParType, &bidiLevels[0]);
            for (size_t i = 0; i < u32BidiTextLength; ++ i)
                hbScripts[i] = hb_unicode_script(hb_unicode_funcs_get_default(), u32BidiText[i]);
            
            if (bidiOpts.resolveScripts) {
                hb_script_t lastScriptValue;
                int lastScriptIndex = -1;
                int lastSetIndex = -1;

                for (int i = 0; i < u32BidiTextLength; ++i) {
                    if (hbScripts[i] == HB_SCRIPT_COMMON ||
                        hbScripts[i] == HB_SCRIPT_INHERITED ||
                        (bidiOpts.resolveUnknownScripts && hbScripts[i] == HB_SCRIPT_UNKNOWN) ) {
                        if (lastScriptIndex != -1) {
                            hbScripts[i] = lastScriptValue;
                            lastSetIndex = i;
                        }
                        else if (i && hbScripts[i] == HB_SCRIPT_INHERITED) {
                            hbScripts[i] = hbScripts[i-1];
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
                    run.direction = FRIBIDI_LEVEL_IS_RTL(bidiLevels[lastCharIndex])? HB_DIRECTION_RTL: HB_DIRECTION_LTR;
                    
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
                hb_buffer_set_cluster_level(buffer, HB_BUFFER_CLUSTER_LEVEL_CHARACTERS);
                hb_buffer_add_utf32(buffer,
                                    u32BidiText + bidiRun.textBegin,
                                    bidiRun.textEnd - bidiRun.textBegin, 0,
                                    bidiRun.textEnd - bidiRun.textBegin);
                
                std::vector<hb_feature_t> featuresVec;
                for (FXTag t : opts.onFeatures) {
                    hb_feature_t f {t, 1/*on*/, 0, (unsigned int)-1};
                    featuresVec.push_back(f);
                }
                for (FXTag t :  opts.offFeatures) {
                    hb_feature_t f {t, 0/*off*/, 0, (unsigned int)-1};
                    featuresVec.push_back(f);
                }

                hb_segment_properties_t segment_props;
                hb_buffer_guess_segment_properties(buffer);
                hb_buffer_get_segment_properties(buffer, &segment_props);
            
                const char ** shappers = hb_shape_list_shapers();
            
                hb_feature_t * features = nullptr;
                unsigned int featuresCount = 0;
                if (featuresVec.size()) {
                    features = &featuresVec[0];
                    featuresCount = featuresVec.size();
                }

                hb_shape(hbFont_,
                         buffer,
                         features,
                         featuresCount);
                

                if (HB_DIRECTION_IS_BACKWARD(bidiRun.direction))
                    hb_buffer_reverse(buffer);
                // get result
                bidiRun.glyphInfos = hb_buffer_get_glyph_infos(buffer, &bidiRun.glyphCount);
                bidiRun.glyphPositions = hb_buffer_get_glyph_positions(buffer, &bidiRun.glyphCount);
            }
            
            
            // Copy glyphs
            size_t totalGlyphCount = 0;
            for (auto & run : bidiRuns)
                totalGlyphCount += run.glyphCount;
            
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
                
                for (size_t gIndex = 0; gIndex < bidiRun.glyphCount; ++ gIndex) {
                    //auto gIndex = j; //bidiRun.direction == HB_DIRECTION_LTR? j : (bidiRun.glyphCount - 1 - j);
                    
                    auto sourceIndex = bidiRun.glyphInfos[gIndex].cluster + bidiRun.textBegin;
                    
                    newGlyphIndices[glyphIndexBase + gIndex] = bidiRun.glyphInfos[gIndex].codepoint;
                    newScripts[glyphIndexBase + gIndex] = hbScripts[sourceIndex];
                    newTypes[glyphIndexBase + gIndex] = bidiTypes[sourceIndex];
                    newLevels[glyphIndexBase + gIndex] = bidiLevels[sourceIndex];
                    
                    hbGlyphInfos[glyphIndexBase + gIndex] = bidiRun.glyphInfos[gIndex];
                    hbGlyphInfos[glyphIndexBase + gIndex].cluster += bidiRun.textBegin;
                    hbGlyphPositions[glyphIndexBase + gIndex] = bidiRun.glyphPositions[gIndex];
                }
                glyphIndexBase += bidiRun.glyphCount;
            }
            
            FXVector<int> hbGIDs;
            for (auto & g: hbGlyphInfos) {
                hbGIDs.push_back(g.codepoint);
            }
            // Reorder glyphs
            
            FXVector<FriBidiStrIndex> map(totalGlyphCount, 0);
            for (auto i = 0; i < totalGlyphCount; ++ i)
                map[i] = i;
            
            fribidi_reorder_line(0, &newTypes[0], totalGlyphCount, 0, bidiParType, &newLevels[0], 0, &map[0]);
            
            hbGlyphCount_ = totalGlyphCount;
            hbGlyphInfos_ = FXVector<hb_glyph_info_t>(totalGlyphCount);
            hbGlyphPositions_ = FXVector<hb_glyph_position_t>(totalGlyphCount);
            for (auto i = 0; i < totalGlyphCount; ++ i) {
                hbGlyphInfos_[i] = hbGlyphInfos[map[i]];
                hbGlyphPositions_[i] = hbGlyphPositions[map[i]];
            }
            
            return addGlyphSpacing(opts.glyphSpacing);
        }
        
        
        //////////////////////////////////////////////////////////////////////////////////////
        hb_direction_t dir = HB_DIRECTION_LTR;
        if (bidiOpts.direction == FXBidiDirection::RTL)
            dir = HB_DIRECTION_RTL;
        
        
        
        
        
        
        
        
        // setup the buffer
        hbBuffer_ = hb_buffer_create();
        if (script == FXOT::DEFAULT_SCRIPT)
            hb_buffer_set_script(hbBuffer_, HB_SCRIPT_COMMON);
        else
            hb_buffer_set_script(hbBuffer_, hb_ot_tag_to_script(script));
        hb_buffer_set_language(hbBuffer_, hb_ot_tag_to_language(language));
        hb_buffer_set_direction(hbBuffer_, (hb_direction_t)opts.direction);
        hb_buffer_add_utf8(hbBuffer_, text.c_str(), text.length(), 0, text.length());
        //hb_buffer_add_utf32(hbBuffer_, (const uint32_t *)u32Bidi.c_str(), u32Bidi.length(), 0, u32Bidi.length());
    
        std::vector<hb_feature_t> featuresVec;
        for (FXTag t : opts.onFeatures) {
            hb_feature_t f {t, 1/*on*/, 0, (unsigned int)-1};
            featuresVec.push_back(f);
        }
        for (FXTag t :  opts.offFeatures) {
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
    
        hb_shape(hbFont_,
                 hbBuffer_,
                 features,
                 featuresCount);
    

        // get result
        auto inf = hb_buffer_get_glyph_infos(hbBuffer_, &hbGlyphCount_);
        auto pos = hb_buffer_get_glyph_positions(hbBuffer_, &hbGlyphCount_);
        hbGlyphInfos_ = FXVector<hb_glyph_info_t>(hbGlyphCount_);
        hbGlyphPositions_ = FXVector<hb_glyph_position_t>(hbGlyphCount_);
        for (size_t i = 0; i < hbGlyphCount_; ++ i) {
            hbGlyphInfos_[i] = inf[i];
            hbGlyphPositions_[i] = pos[i];
        }
        
        if (shouldFallback()) {
            hasFallbackShaping_ = true;
            fallbackShape(text);
        }
        else {
            hasFallbackShaping_ = false;
        }

        return addGlyphSpacing(opts.glyphSpacing);
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
        hbGlyphInfos_ = FXVector<hb_glyph_info_t>(u32.size());
        hbGlyphPositions_ = FXVector<hb_glyph_position_t>(u32.size());
        for (size_t i = 0; i < u32.size(); ++ i) {
            FXGChar ch(u32[i]);
            if (FXCharIsEncodedGID(ch.value))
                ch = FXGChar(FXCharDecodeGID(ch.value), FXGCharTypeGlyphID);

            FXGlyph g = face_->glyph(ch);
            hbGlyphInfos_[i].cluster = i;
            hbGlyphInfos_[i].codepoint = g.gid;
            hbGlyphPositions_[i].x_advance = g.metrics.horiAdvance? g.metrics.horiAdvance: g.metrics.width;
            hbGlyphPositions_[i].y_advance = g.metrics.vertAdvance? g.metrics.vertAdvance: g.metrics.height;
            hbGlyphPositions_[i].x_offset = 0;
            hbGlyphPositions_[i].y_offset = 0;
        }
        hbGlyphCount_ = u32.size();
    }

    void
    addGlyphSpacing(double spacing) {
        if (!spacing)
            return;
        
        for (size_t i = 0; i < hbGlyphInfos_.size(); ++ i) 
            hbGlyphPositions_[i].x_advance += face_->upem() * spacing;
    }
    
    FXFace * face_{};

    bool hasFallbackShaping_ {false};
    hb_font_t * hbFont_ {};
    hb_face_t * hbFace_ {};
    hb_buffer_t * hbBuffer_{};
    
    unsigned int  hbGlyphCount_ {};
    FXVector<hb_glyph_info_t> hbGlyphInfos_ {};
    FXVector<hb_glyph_position_t> hbGlyphPositions_ {};

};


FXShaper::FXShaper(FXFace * face)
    : imp_(new FXShaperImp(face)){}
    
void
FXShaper::shape(const FXString & text,
                FXTag script,
                FXTag language,
                const FXShapingGenralOptions & opts,
                const FXShapingBidiOptions & bidiOpts) {
    return imp_->shape(text, script, language, opts, bidiOpts);
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

size_t
FXShaper::cluster(size_t index) const {
    return imp_->hbGlyphInfos_[index].cluster;
}

FXFace *
FXShaper::face() const {
    return imp_->face_;
}
    
bool
FXShaper::hasFallbackShaping() const {
    return imp_->hasFallbackShaping_;
}
