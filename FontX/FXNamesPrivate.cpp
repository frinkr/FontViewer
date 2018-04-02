#include "FXNamesPrivate.h"

std::string
FTGetPlatformName(FT_UShort platformId) {
    switch (platformId) {
        case TT_PLATFORM_APPLE_UNICODE: return  "Unicode";
        case TT_PLATFORM_MACINTOSH: return "Macintosh";
        case TT_PLATFORM_ISO: return "ISO";
        case TT_PLATFORM_MICROSOFT: return "Microsoft";
        case TT_PLATFORM_ADOBE: return "Adobe";
        default: return "Unknown Platform";
    }
}

std::string
FTGetEncodingName(FT_UShort platformId, FT_UShort encodingId) {

    std::map<FT_UShort, std::string> encodings;
    switch (platformId) {
        case TT_PLATFORM_APPLE_UNICODE:
            encodings =  {
                {TT_APPLE_ID_DEFAULT,                  "1.0"},
                {TT_APPLE_ID_UNICODE_1_1,             "1.1"},
                {TT_APPLE_ID_ISO_10646,                "ISO/IEC 10646"},
                {TT_APPLE_ID_UNICODE_2_0,              "2.0, BMP only"},
                {TT_APPLE_ID_UNICODE_32,               "2.0, full repertoire"},
                {TT_APPLE_ID_VARIANT_SELECTOR,         "Variation Sequences"},
                {6,                                    "full repertoire"},
            };
            break;

        case TT_PLATFORM_MACINTOSH:
            encodings = {
                {TT_MAC_ID_ROMAN,                       "Roman"},
                {TT_MAC_ID_JAPANESE,                    "Japanese"},
                {TT_MAC_ID_TRADITIONAL_CHINESE,         "Chinese (Traditional)"},
                {TT_MAC_ID_KOREAN,                      "Korean"},
                {TT_MAC_ID_ARABIC,                      "Arabic"},
                {TT_MAC_ID_HEBREW,                      "Hebrew"},
                {TT_MAC_ID_GREEK,                       "Greek"},
                {TT_MAC_ID_RUSSIAN,                     "Russian"},
                {TT_MAC_ID_RSYMBOL,                     "RSymbol"},
                {TT_MAC_ID_DEVANAGARI,                  "Devanagari"},
                {TT_MAC_ID_GURMUKHI,                    "Gurmukhi"},
                {TT_MAC_ID_GUJARATI,                    "Gujarati"},
                {TT_MAC_ID_ORIYA,                       "Oriya"},
                {TT_MAC_ID_BENGALI,                     "Bengali"},
                {TT_MAC_ID_TAMIL,                       "Tamil"},
                {TT_MAC_ID_TELUGU,                      "Telugu"},
                {TT_MAC_ID_KANNADA,                     "Kannada"},
                {TT_MAC_ID_MALAYALAM,                   "Malayalam"},
                {TT_MAC_ID_SINHALESE,                   "Sinhalese"},
                {TT_MAC_ID_BURMESE,                     "Burmese"},
                {TT_MAC_ID_KHMER,                       "Khmer"},
                {TT_MAC_ID_THAI,                        "Thai"},
                {TT_MAC_ID_LAOTIAN,                     "Laotian"},
                {TT_MAC_ID_GEORGIAN,                    "Georgian"},
                {TT_MAC_ID_ARMENIAN,                    "Armenian"},
                {TT_MAC_ID_SIMPLIFIED_CHINESE,          "Chinese (Simplified)"},
                {TT_MAC_ID_TIBETAN,                     "Tibetan"},
                {TT_MAC_ID_MONGOLIAN,                   "Mongolian"},
                {TT_MAC_ID_GEEZ,                        "Geez"},
                {TT_MAC_ID_SLAVIC,                      "Slavic"},
                {TT_MAC_ID_VIETNAMESE,                  "Vietnamese"},
                {TT_MAC_ID_SINDHI,                      "Sindhi"},
                {TT_MAC_ID_UNINTERP,                    "Uninterpreted"},
            };
            break;
            
        case TT_PLATFORM_ISO:
            encodings = {
                {TT_ISO_ID_7BIT_ASCII,                  "7-bit ASCII"},
                {TT_ISO_ID_10646,                       "10646"},
                {TT_ISO_ID_8859_1,                      "8859-1"},
            };
            break;
            
        case TT_PLATFORM_MICROSOFT:
            encodings = {
                {TT_MS_ID_SYMBOL_CS,                    "Symbol"},
                {TT_MS_ID_UNICODE_CS,                   "Unicode BMP"},
                {TT_MS_ID_SJIS,                         "ShiftJIS"},
                {TT_MS_ID_GB2312,                       "RPC"},
                {TT_MS_ID_WANSUNG,                      "Big5"},
                {TT_MS_ID_WANSUNG,                      "Wansung"},
                {TT_MS_ID_JOHAB,                        "Johab"},
                {7,                                     "Reserved"},
                {8,                                     "Reserved"},
                {9,                                     "Reserved"},
                {TT_MS_ID_UCS_4,                        "Unicode UCS-4"},
            };
            break;
        case TT_PLATFORM_ADOBE:
            encodings = {
                {TT_ADOBE_ID_STANDARD,                  "Standard"},
                {TT_ADOBE_ID_EXPERT,                    "Expert"},
                {TT_ADOBE_ID_CUSTOM,                    "Custom"},
                {TT_ADOBE_ID_LATIN_1,                   "Latin 1"},
            };
            break;

        default:
            break;
    }

    auto itr = encodings.find(encodingId);
    if (itr != encodings.end())
        return itr->second;
    
    return "Unknown Encoding";
}
