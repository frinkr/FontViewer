#include "FXFace.h"
#include "FXInspector.h"
#include "FXFTPrivate.h"
#include "FXHBPrivate.h"

struct FXInspectorImp {

    explicit FXInspectorImp(FXFace * face)
        : face_(face) {
        init();
    }
    void
    init() {        
        hb_font_t * hbFont = nullptr;
        hb_face_t * hbFace = nullptr;
        
        FXHBCreateFontFace(face_, &hbFont, &hbFace);         
        loadOTTables(hbFace);
        hb_font_destroy(hbFont);
    }

    void
    loadOTTables(hb_face_t * hbFace) {
        loadGSUBGPOS(hbFace);
    }


    FXVector<FXInsOTLookup>
    loadLookups(hb_face_t * hbFace, hb_tag_t tableTag, unsigned int featureIndex) {
        unsigned int lookupCount = hb_ot_layout_feature_get_lookups(
            hbFace,
            tableTag,
            featureIndex,
            0, nullptr, nullptr);
        if (!lookupCount)
            return FXVector<FXInsOTLookup>();
        
        std::vector<unsigned int> lookups(lookupCount);
        hb_ot_layout_feature_get_lookups(
            hbFace,
            tableTag,
            featureIndex,
            0, &lookupCount, &lookups[0]);

        FXVector<FXInsOTLookup> otLookups;

        for (size_t i = 0; i < lookups.size(); ++ i) {
            unsigned int lookup_index = lookups[i];

            FXInsOTLookup lookup;
            lookup.index = lookup_index;
            otLookups.push_back(lookup);
        }
        return otLookups;
    }
    
    FXVector<FXInsOTFeature>
    loadFeatures(hb_face_t * hbFace, hb_tag_t table, unsigned int scriptIndex, unsigned int languageIndex) {
        unsigned int featureCount = hb_ot_layout_language_get_feature_tags(
            hbFace,
            table,
            scriptIndex,
            languageIndex,
            0, nullptr, nullptr);
        if (!featureCount)
            return FXVector<FXInsOTFeature>();
        
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
        
        FXVector<FXInsOTFeature> otFeatures;

        for (unsigned int index = 0; index < featureCount; ++ index) {
            hb_tag_t featureTag = featureTags[index];
            unsigned int featureIndex = featureIndexes[index];

            FXInsOTFeature otFeature;
            otFeature.tag = featureTag;
            otFeature.index = featureIndex;
            otFeature.isRequired = (featureTag == requiredFeatureTag);
            otFeature.lookups = loadLookups(hbFace, table, featureIndex);
            otFeatures.push_back(otFeature);
        }
        return otFeatures;
    }

    FXVector<FXInsOTLanguage>
    loadLanguages(hb_face_t * hbFace, hb_tag_t table, unsigned int scriptIndex) {
        unsigned int languageCount = hb_ot_layout_script_get_language_tags(
            hbFace,
            table,
            scriptIndex,
            0, nullptr, nullptr);
        if (!languageCount)
            return FXVector<FXInsOTLanguage>();
        
        std::vector<hb_tag_t> languageTags(languageCount + 1);
        languageTags[0] = FXOT::DEFAULT_LANGUAGE;
        
        hb_ot_layout_script_get_language_tags(
            hbFace,
            table,
            scriptIndex,
            0,
            &languageCount,
            &languageTags[1]);
        
        FXVector<FXInsOTLanguage> otLanguages;
        for (hb_tag_t langTag : languageTags) {
            unsigned int langIndex = 0;
            if (langTag == FXOT::DEFAULT_LANGUAGE)
                langIndex = FXOT::DEFAULT_LANGUAGE_INDEX;
            else if (!hb_ot_layout_script_find_language(
                         hbFace,
                         table,
                         scriptIndex,
                         langTag,
                         &langIndex))
                continue;
            
            FXInsOTLanguage otLang;
            otLang.tag = langTag;
            otLang.index = langIndex;
            otLang.features = loadFeatures(hbFace, table, scriptIndex, langIndex);
            
            otLanguages.push_back(otLang);
        }
        return otLanguages;
    }
    
    FXVector<FXInsOTScript>
    loadScripts(hb_face_t * hbFace, hb_tag_t table) {
        unsigned int scriptCount = hb_ot_layout_table_get_script_tags(
            hbFace,
            table,
            0, nullptr, nullptr);

        if (!scriptCount)
            return FXVector<FXInsOTScript>();
        
        FXVector<hb_tag_t> scriptTags(scriptCount);
        hb_ot_layout_table_get_script_tags(
            hbFace,
            table,
            0,
            &scriptCount,
            &scriptTags[0]);

        FXVector<FXInsOTScript> otScripts;
        for (hb_tag_t scriptTag : scriptTags) {
            unsigned int scriptIndex = 0;
            if (!hb_ot_layout_table_find_script(hbFace, table, scriptTag, &scriptIndex))
                continue;
            
            FXInsOTScript otScript;
            otScript.tag = scriptTag;
            otScript.index = scriptIndex;
            otScript.languages = loadLanguages(hbFace, table, scriptIndex);
            
            otScripts.push_back(otScript);
        }
        return otScripts;
    }

    FXVector<FXTag>
    loadFeatureTags(hb_face_t * hbFace, hb_tag_t table) {
        unsigned int featureCount = hb_ot_layout_table_get_feature_tags(hbFace, table, 0, nullptr, nullptr);
        if (!featureCount)
            return FXVector<FXTag>();
        
        std::vector<hb_tag_t> featureTags(featureCount);
        hb_ot_layout_table_get_feature_tags(hbFace, table, 0, &featureCount, &featureTags[0]);

        FXVector<FXTag> otFeatureTags;
        for (hb_tag_t tag : featureTags) 
            otFeatureTags.push_back(tag);

        return otFeatureTags;
        
    }
    void
    loadGSUBGPOS(hb_face_t * hbFace) {
        hb_tag_t tables [] = {HB_OT_TAG_GSUB, HB_OT_TAG_GPOS};
        for (hb_tag_t table : tables) {
            FXInsOTTable otTable;
            otTable.tag = table;
            otTable.scripts = loadScripts(hbFace, table);
            otTable.featureTags = loadFeatureTags(hbFace, table);
            otTables_.push_back(otTable);
        }
    }


    template <typename Func>
    void enumurateTables(const FXVector<FXTag> & tableTags, Func f) const {
        for (FXTag tag : tableTags) {
            for (const FXInsOTTable & otTable: otTables_) {
                if (otTable.tag == tag)
                    f(otTable);
            }
        }
    }

    
    FXVector<FXInsOTTable> otTables_;
    FXFace  * face_;
};

namespace {
    FXVector<FXTag>
    getTables(FXTag tag) {
        if (tag == FXOT::MERGED_GSUBGPOS) 
            return {FXTableGSUB, FXTableGPOS};
        else
            return {tag};
    }
    
    FXVector<FXTag> &
    sortAndRemoveDuplicates(FXVector<FXTag> & tags) {
        FXSet<FXTag> set(tags.begin(), tags.end());
        tags.assign(set.begin(), set.end());
        return tags;
    }
}
FXInspector::FXInspector(FXFace * face)
    : imp_(new FXInspectorImp(face)) {
}

FXVector<FXTag>
FXInspector::otScripts(FXTag table) const {
    FXVector<FXTag> scripts;
    const FXVector<FXTag> tables = getTables(table);
    imp_->enumurateTables(tables, [&scripts](const FXInsOTTable & otTable) {
        for (const FXInsOTScript & otScript : otTable.scripts)
            scripts.push_back(otScript.tag);
    });
    return sortAndRemoveDuplicates(scripts);
}

FXVector<FXTag>
FXInspector::otLanguages(FXTag script, FXTag table) const {
    FXVector<FXTag> languages;
    const FXVector<FXTag> tables = getTables(table);
    imp_->enumurateTables(tables, [&](const FXInsOTTable & otTable) {
        for (const FXInsOTScript & otScript : otTable.scripts) {
            if (otScript.tag != script) continue;
            for (const FXInsOTLanguage & otLang : otScript.languages)
                languages.push_back(otLang.tag);
        }
    });
    return sortAndRemoveDuplicates(languages);
}

FXVector<FXTag>
FXInspector::otFeatures(FXTag script, FXTag language, FXTag table) const {
    FXVector<FXTag> features;
    const FXVector<FXTag> tables = getTables(table);
    imp_->enumurateTables(tables, [&](const FXInsOTTable & otTable) {
        for (const FXInsOTScript & otScript : otTable.scripts) {
            if (otScript.tag != script) continue;
            for (const FXInsOTLanguage & otLang : otScript.languages) {
                if (otLang.tag != language) continue;
                for (const FXInsOTFeature & otFeature : otLang.features) 
                    features.push_back(otFeature.tag);
            }
        }
    });
    return sortAndRemoveDuplicates(features);
}
