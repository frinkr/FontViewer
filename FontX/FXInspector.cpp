#include <hb.h>
#include <hb-ft.h>
#include <hb-ot.h>

#include "FXFace.h"
#include "FXInspector.h"
#include "FXFTPrivate.h"

namespace {
    struct FXOTLookup {
        int index;
    };

    struct FXOTFeature {
        FXTag tag;
        int   index;
        FXVector<FXOTLookup> lookups;
    };

    struct FXOTLanguage {
        FXTag tag;
        int   index;
        FXVector<FXOTFeature> features;
    };

    struct FXOTScript {
        FXTag tag;
        int   index;
        FXVector<FXOTLanguage>  languages;    
    };

    struct FXOTTable {
        FXTag tag;
        FXVector<FXOTScript> scripts;
    };

    constexpr unsigned int FX_OT_LANGUAGE_DEFAUL_INDEX = 0xFFFFu;
    constexpr unsigned int FX_OT_LANGUAGE_MISSING_INDEX = -2;
    
    
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

        hb_font_t * hbFont = hb_ft_font_create_referenced(ftFace);
        hb_face_t * hbFace = hb_font_get_face(hbFont);
        
        hb_ot_font_set_funcs(hbFont);
        unsigned int upem = hb_face_get_upem(hbFace);
        hb_font_set_scale(hbFont, upem, upem);

        loadOTTables(hbFace);
        
        hb_font_destroy(hbFont);
    }

    void
    loadOTTables(hb_face_t * hbFace) {
        loadGSUBGPOS(hbFace);
    }


    FXVector<FXOTLookup>
    loadLookups(hb_face_t * hbFace, hb_tag_t tableTag, unsigned int featureIndex) {
        unsigned int lookupCount = hb_ot_layout_feature_get_lookups(
            hbFace,
            tableTag,
            featureIndex,
            0, nullptr, nullptr);
        std::vector<unsigned int> lookups(lookupCount);
        hb_ot_layout_feature_get_lookups(
            hbFace,
            tableTag,
            featureIndex,
            0, &lookupCount, &lookups[0]);

        FXVector<FXOTLookup> otLookups;

        for (size_t i = 0; i < lookups.size(); ++ i) {
            unsigned int lookup_index = lookups[i];

            FXOTLookup lookup;
            lookup.index = lookup_index;
            otLookups.push_back(lookup);
        }
        return otLookups;
    }
    
    FXVector<FXOTFeature>
    loadFeatures(hb_face_t * hbFace, hb_tag_t table, unsigned int scriptIndex, unsigned int languageIndex) {
        unsigned int featureCount = hb_ot_layout_language_get_feature_tags(
            hbFace,
            table,
            scriptIndex,
            languageIndex,
            0, nullptr, nullptr);
        
        std::vector<hb_tag_t> featureTags(featureCount);
        std::vector<unsigned int> featureIndexes(featureCount);
        hb_ot_layout_language_get_feature_tags(
            hbFace,
            table,
            scriptIndex,
            languageIndex,
            0,
            &featureCount,
            &featureTags[0]);
        hb_ot_layout_language_get_feature_indexes(
            hbFace,
            table,
            scriptIndex,
            languageIndex,
            0,
            &featureCount,
            &featureIndexes[0]);
                
        unsigned int  requiredFeatureIndex = 0xFFFF;
        hb_tag_t requiredFeatureTag = 0xFFFF;
        if (!hb_ot_layout_language_get_required_feature(
                hbFace,
                table,
                scriptIndex,
                languageIndex,
                &requiredFeatureIndex,
                &requiredFeatureTag))
            requiredFeatureIndex = 0xFFFF;
        
        FXVector<FXOTFeature> otFeatures;

        for (unsigned int index = 0; index < featureCount; ++ index) {
            hb_tag_t featureTag = featureTags[index];
            unsigned int featureIndex = featureIndexes[index];

            FXOTFeature otFeature;
            otFeature.tag = featureTag;
            otFeature.index = featureIndex;
            otFeature.lookups = loadLookups(hbFace, table, featureIndex);
            otFeatures.push_back(otFeature);
        }
        return otFeatures;
    }

    FXVector<FXOTLanguage>
    loadLanguages(hb_face_t * hbFace, hb_tag_t table, unsigned int scriptIndex) {
        unsigned int languageCount = hb_ot_layout_script_get_language_tags(
            hbFace,
            table,
            scriptIndex,
            0, nullptr, nullptr);
        
        std::vector<hb_tag_t> languageTags(languageCount + 1);
        languageTags[0] = FXTagOTDefaultLanguage;
        
        hb_ot_layout_script_get_language_tags(
            hbFace,
            table,
            scriptIndex,
            0,
            &languageCount,
            &languageTags[1]);
        
        FXVector<FXOTLanguage> otLanguages;
        for (hb_tag_t langTag : languageTags) {
            unsigned int langIndex = FX_OT_LANGUAGE_MISSING_INDEX;
            if (langTag == FXTagOTDefaultLanguage)
                langIndex = FX_OT_LANGUAGE_DEFAUL_INDEX;
            else if (!hb_ot_layout_script_find_language(
                         hbFace,
                         table,
                         scriptIndex,
                         langTag,
                         &langIndex))
                continue;
            
            FXOTLanguage otLang;
            otLang.tag = langTag;
            otLang.index = langIndex;
            otLang.features = loadFeatures(hbFace, table, scriptIndex, langIndex);
            
            otLanguages.push_back(otLang);
        }
        return otLanguages;
    }
    
    FXVector<FXOTScript>
    loadScripts(hb_face_t * hbFace, hb_tag_t table) {
        unsigned int scriptCount = hb_ot_layout_table_get_script_tags(
            hbFace,
            table,
            0, nullptr, nullptr);
        
        FXVector<hb_tag_t> scriptTags(scriptCount);
        hb_ot_layout_table_get_script_tags(
            hbFace,
            table,
            0,
            &scriptCount,
            &scriptTags[0]);

        FXVector<FXOTScript> otScripts;
        for (hb_tag_t scriptTag : scriptTags) {
            unsigned int scriptIndex = 0;
            if (!hb_ot_layout_table_find_script(hbFace, table, scriptTag, &scriptIndex))
                continue;
            
            FXOTScript otScript;
            otScript.tag = scriptTag;
            otScript.index = scriptIndex;
            otScript.languages = loadLanguages(hbFace, table, scriptIndex);
            
            otScripts.push_back(otScript);
        }
        return otScripts;
    }
    
    void
    loadGSUBGPOS(hb_face_t * hbFace) {
        hb_tag_t tables [] = {HB_OT_TAG_GSUB, HB_OT_TAG_GPOS};
        for (hb_tag_t table : tables) {
            FXOTTable otTable;
            otTable.tag = table;
            otTable.scripts = loadScripts(hbFace, table);
            otTables_.push_back(otTable);
        }
    }

    FXVector<FXOTTable> otTables_;
    FXFace  * face_;
};

FXInspector::FXInspector(FXFace * face)
    : imp_(new FXInspectorImp(face)) {
}

