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
    {
    }

    ~FXShaperImp() {
        reset();
        hb_font_destroy(hbFont_);
    }
    
    void reset() {
        

        hasFallbackShaping_ = false;

        glyphs_.clear();
    }
    
    

    void
    shape(const FXString & text,
          FXTag script,
          FXTag language,
          const FXShapingOptions & opts) {

        reset();

        if (text.empty())
            return;

        const std::u32string u32Text = FXUnicode::utf8ToUTF32(text);
        const auto haveEncodedGID = std::any_of(u32Text.begin(), u32Text.end(), [](auto c) { return FXCharIsEncodedGID(c);});

        if (haveEncodedGID && !opts.forceShapeGIDEncodedText) {
            hasFallbackShaping_ = true;
            fallbackShape(text);
            return postShaping(opts.glyphSpacing);
        }
        
        if (!hbFont_ || !hbFace_)
            FXHBCreateFontFace(face_, &hbFont_, &hbFace_);

        if (opts.bidi.activated) 
            glyphs_ = shapeBidi(u32Text, script, language, opts);
        else
            glyphs_ = shapeNoBidi(u32Text, script, language, opts);

        if (shouldFallback()) {
            hasFallbackShaping_ = true;
            fallbackShape(text);
        }
        else {
            hasFallbackShaping_ = false;
        }

        return postShaping(opts.glyphSpacing);
    }

    bool
    shouldFallback() const {
        for (auto & g: glyphs_) {
            if (g.id)
                return false;
        }
        return true;
    }

    void
    fallbackShape(const FXString & text) {
        auto u32 = FXUnicode::utf8ToUTF32(text);

        for (size_t i = 0; i < u32.size(); ++ i) {
            FXGChar ch(u32[i]);
            if (FXCharIsEncodedGID(ch.value))
                ch = FXGChar(FXCharDecodeGID(ch.value), FXGCharTypeGlyphID);

            FXGlyph g = face_->glyph(ch);
            FXShaper::GlyphInfo gi;
            gi.id      = g.gid;
            gi.cluster = i;
            gi.advance = FXVec2d<fu>{fu(g.metrics.horiAdvance? g.metrics.horiAdvance: g.metrics.width), fu(g.metrics.vertAdvance? g.metrics.vertAdvance: g.metrics.height)};
            gi.offset  = FXVec2d<fu>{0, 0};
        }

    }

    void
    postShaping(double spacing) {
        for (size_t i = 0; i < glyphs_.size(); ++ i) {
            auto & g = glyphs_[i];
            g.spacing.x = face_->upem() * spacing;
            g.advance.x += g.spacing.x;
        }
    }


    static FXVector<FXShaper::GlyphInfo>
    readHbBuffer(hb_buffer_t * buffer) {
        unsigned int hbGlyphCount = 0;
        auto hbGlyphInfos = hb_buffer_get_glyph_infos(buffer, &hbGlyphCount);
        auto hbGlyphPositions = hb_buffer_get_glyph_positions(buffer, &hbGlyphCount);

        FXVector<FXShaper::GlyphInfo> glyphs;
        for (size_t i = 0; i < hbGlyphCount; ++ i) {
            FXShaper::GlyphInfo g;
            g.id      = hbGlyphInfos[i].codepoint;
            g.cluster = hbGlyphInfos[i].cluster;
            g.advance = FXVec2d<fu>{fu(hbGlyphPositions[i].x_advance), fu(hbGlyphPositions[i].y_advance)};
            g.offset  = FXVec2d<fu>{fu(hbGlyphPositions[i].x_offset), fu(hbGlyphPositions[i].y_offset)};
            glyphs.push_back(g);
        }
        return glyphs;
        
    }
    
    hb_buffer_t *
    hbShape(const char32_t * text,
            size_t textLength,
            hb_script_t script,
            hb_direction_t direction,
            FXShapingOptions opts)
    {
        auto buffer = hb_buffer_create();

        hb_buffer_set_direction(buffer, direction);
        hb_buffer_set_script(buffer, script);
        hb_buffer_set_cluster_level(buffer, HB_BUFFER_CLUSTER_LEVEL_CHARACTERS);
        hb_buffer_add_utf32(buffer, (const uint32_t*)text, textLength, 0, textLength);

                
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
        return buffer;
           
    }

    FXVector<FXShaper::GlyphInfo>
    shapeBidi(const std::u32string & u32Text,
              FXTag script,
              FXTag language,
              const FXShapingOptions & opts) {

        struct FXBidiRun {
            size_t          textBegin;
            size_t          textEnd;
            hb_direction_t  direction;
            hb_script_t     script;
            FXVector<FXShaper::GlyphInfo> glyphs;
        };
        
        // Let's do bidi
        auto u32BidiText = (const FriBidiChar*)(u32Text.c_str());
        auto u32BidiTextLength = u32Text.size();
            
        static_assert(sizeof(FriBidiChar) == sizeof(typename std::u32string::value_type));
            
        FriBidiParType bidiParType = FRIBIDI_PAR_ON;
        switch (opts.bidi.direction) {
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
            
        if (opts.bidi.resolveScripts) {
            hb_script_t lastScriptValue;
            int lastScriptIndex = -1;
            int lastSetIndex = -1;

            for (int i = 0; i < u32BidiTextLength; ++i) {
                if (hbScripts[i] == HB_SCRIPT_COMMON ||
                    hbScripts[i] == HB_SCRIPT_INHERITED ||
                    (opts.bidi.resolveUnknownScripts && hbScripts[i] == HB_SCRIPT_UNKNOWN) ) {
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
                || (opts.bidi.breakOnScriptChange && hbScripts[i] != hbScripts[lastCharIndex])
                || (opts.bidi.breakOnLevelChange && bidiLevels[i] != bidiLevels[lastCharIndex]))
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

            auto buffer = hbShape((const char32_t*)u32BidiText + bidiRun.textBegin,
                                     bidiRun.textEnd - bidiRun.textBegin,
                                     bidiRun.script,
                                     bidiRun.direction, opts);

            if (HB_DIRECTION_IS_BACKWARD(bidiRun.direction))
                hb_buffer_reverse(buffer);
            
            // get result
            bidiRun.glyphs = readHbBuffer(buffer);
            
            if (HB_DIRECTION_IS_BACKWARD(bidiRun.direction))
                for (auto & g :bidiRun.glyphs)
                    g.rtl = true;
            
            hb_buffer_destroy(buffer);
        }
            
            
        // Copy glyphs
        size_t totalGlyphCount = 0;
        for (auto & run : bidiRuns)
            totalGlyphCount += run.glyphs.size();
            
        auto newTypes = std::vector<FriBidiCharType>(totalGlyphCount, 0);
        auto newScripts = std::vector<hb_script_t>(totalGlyphCount, HB_SCRIPT_COMMON);
        auto newLevels = std::vector<FriBidiLevel>(totalGlyphCount, 0);

        FXVector<FXShaper::GlyphInfo> glyphs(totalGlyphCount);
        size_t glyphIndexBase = 0;
        for (size_t i = 0; i < bidiRuns.size(); ++ i) {
            auto & bidiRun = bidiRuns[i];
            for (size_t gIndex = 0; gIndex < bidiRun.glyphs.size(); ++ gIndex) {
                auto sourceIndex = bidiRun.glyphs[gIndex].cluster + bidiRun.textBegin;
                    
                newScripts[glyphIndexBase + gIndex] = hbScripts[sourceIndex];
                newTypes[glyphIndexBase + gIndex] = bidiTypes[sourceIndex];
                newLevels[glyphIndexBase + gIndex] = bidiLevels[sourceIndex];

                glyphs[glyphIndexBase + gIndex] = bidiRun.glyphs[gIndex];
                glyphs[glyphIndexBase + gIndex].cluster += bidiRun.textBegin;
            }
            glyphIndexBase += bidiRun.glyphs.size();
        }
            
        // Reorder glyphs
        FXVector<FriBidiStrIndex> map(totalGlyphCount, 0);
        for (auto i = 0; i < totalGlyphCount; ++ i)
            map[i] = i;
            
        fribidi_reorder_line(0, &newTypes[0], totalGlyphCount, 0, bidiParType, &newLevels[0], 0, &map[0]);

        FXVector<FXShaper::GlyphInfo> glyphsReordered(totalGlyphCount);
        for (auto i = 0; i < totalGlyphCount; ++ i) 
            glyphsReordered[i] = glyphs[map[i]];

        return glyphsReordered;
    }

    FXVector<FXShaper::GlyphInfo>
    shapeNoBidi(const std::u32string & u32Text,
                FXTag script,
                FXTag language,
                const FXShapingOptions & opts) {
        hb_direction_t dir = HB_DIRECTION_LTR;
        if (opts.bidi.direction == FXBidiDirection::RTL)
            dir = HB_DIRECTION_RTL;

        hb_script_t hbScript = HB_SCRIPT_COMMON;
        if (script != FXOT::DEFAULT_SCRIPT)
            hbScript = hb_ot_tag_to_script(script);
        auto buffer = hbShape(u32Text.c_str(), u32Text.length(), hbScript, dir, opts);

        auto glyphs = readHbBuffer(buffer);
        hb_buffer_destroy(buffer);
        return glyphs;
    }
    
    
    FXFace * face_{};

    bool hasFallbackShaping_ {false};
    hb_font_t * hbFont_ {};
    hb_face_t * hbFace_ {};

    FXVector<FXShaper::GlyphInfo> glyphs_ {};

};


FXShaper::FXShaper(FXFace * face)
    : imp_(new FXShaperImp(face)){}
    
void
FXShaper::shape(const FXString & text,
                FXTag script,
                FXTag language,
                const FXShapingOptions & opts){
    return imp_->shape(text, script, language, opts);
}

size_t
FXShaper::glyphCount() const {
    return imp_->glyphs_.size();
}

const FXShaper::GlyphInfo &
FXShaper::glyphInfo(size_t index) const {
    return imp_->glyphs_[index];
}

FXGlyphID
FXShaper::glyph(size_t index) const {
    return imp_->glyphs_[index].id;
}

FXVec2d<fu>
FXShaper::advance(size_t index) const {
    return imp_->glyphs_[index].advance;
}

FXVec2d<fu>
FXShaper::offset(size_t index) const {
    return imp_->glyphs_[index].offset;

}

size_t
FXShaper::cluster(size_t index) const {
    return imp_->glyphs_[index].cluster;
}

FXVec2d<fu>
FXShaper::spacing(size_t index) const {
    if (index < imp_->glyphs_.size())
        return imp_->glyphs_[index].spacing;
    else
        return {0, 0};
}

FXVec2d<fu>
FXShaper::kerning(size_t index) const {
    if (index < imp_->glyphs_.size())
        return imp_->glyphs_[index].spacing;
    else
        return {0, 0};
}

FXFace *
FXShaper::face() const {
    return imp_->face_;
}
    
bool
FXShaper::hasFallbackShaping() const {
    return imp_->hasFallbackShaping_;
}
