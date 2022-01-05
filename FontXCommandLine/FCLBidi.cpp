#include <fribidi/fribidi.h>

#include <unicode/uchar.h>
#include <unicode/uscript.h>
#include <unicode/unorm2.h>
#include <unicode/unistr.h>
#include <unicode/ubidi.h>

#include <vector>

extern "C" void tryBidi() 
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
            
