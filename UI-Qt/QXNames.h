#pragma once
#include <vector>

class QXNames {
public:
    static const char * OS2GetWeightClassName(uint16_t value) {
        switch (value) {
        case 100: return "Thin";
        case 200: return "Extra-light (Ultra-light)";
        case 300: return "Light";
        case 400: return "Normal (Regular)";
        case 500: return "Medium";
        case 600: return "Semi-bold (Demi-bold)";
        case 700: return "Bold";
        case 800: return "Extra-bold (Ultra-bold)";
        case 900: return "Black (Heavy)";
        default: return "Unknown";
        }
    }
        
    static const char * OS2GetWidthClassName(uint16_t value) {
        switch (value) {
        case 1: return "Ultra-condensed";
        case 2: return "Extra-condensed";
        case 3: return "Condensed";
        case 4: return "Semi-condensed";
        case 5: return "Medium (normal)";
        case 6: return "Semi-expanded";
        case 7: return "Expanded";
        case 8: return "Extra-expanded";
        case 9: return "Ultra-expanded";
        default: return "Unknown";
        }
    }
        
    static std::vector<std::string> OS2GetFsTypeDescription(uint16_t value) {
        if (!value)
            return {"Installable Embedding"};
            
        std::vector<std::string> names;
            
        if (value & (1 << 1)) names.push_back("Restricted License Embedding");
        if (value & (1 << 2)) names.push_back("Preview & Print Embedding");
        if (value & (1 << 3)) names.push_back("Editable Embedding");
        if (value & (1 << 8)) names.push_back("No Subsetting");
        if (value & (1 << 9)) names.push_back("Bitmap Embedding Only");
            
        return names;
    }
        
    static const char * OS2GetFamilyClassName(uint16_t value) {
        unsigned short family = ((value & 0xFF00) >> 8);
            
        const char * names [] = {"No classification",
                                 "OldStyle Serifs",
                                 "Transitional Serifs",
                                 "Modern Serifs",
                                 "Clarendon Serifs",
                                 "Slab Serifs",
                                 "Reserved",
                                 "Freeform Serifs",
                                 "Sans Serif",
                                 "Ornamentals",
                                 "Scripts",
                                 "reserved",
                                 "Symbolic",
                                 "reserved",
                                 "reserved",
        };
            
        if (family < sizeof(names)/sizeof(names[0]))
            return names[family];
        return "Unknown Family Class";
    }
        
    static const char * OS2GetSubFamilyClassName(uint16_t value) {
        uint16_t family = ((value & 0xFF00) >> 8);
        uint16_t subFamily = (value & 0xFF);
            
        std::vector<const char *> subFamilyNames;
        switch (family) {
        case 1: subFamilyNames = {"No classification",
                                  "IBM Rounded Legibility",
                                  "Garalde",
                                  "Venetian",
                                  "Modified Venetian",
                                  "Dutch Modern",
                                  "Dutch Traditional",
                                  "Contemporary",
                                  "Calligraphic",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "Miscellaneous"};
            break;
        case 2: subFamilyNames = {"No Classification",
                                  "Direct Line",
                                  "Script",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "Miscellaneous"};
            break;
        case 3: subFamilyNames = {"No Classification",
                                  "Italian",
                                  "Script",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "Miscellaneous"};
            break;
        case 4: subFamilyNames = {"No Classification",
                                  "Clarendon",
                                  "Modern",
                                  "Traditional",
                                  "Newspaper",
                                  "Stub Serif",
                                  "Monotone",
                                  "Typewriter",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "Miscellaneous"};
            break;
        case 5: subFamilyNames = {"No classification",
                                  "Monotone",
                                  "Humanist",
                                  "Geometric",
                                  "Swiss",
                                  "Typewriter",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "Miscellaneous"};
            break;
        case 7: subFamilyNames = {"No classification",
                                  "Modern",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "reserved",
                                  "Miscellaneous"};
            break;
        case 8: subFamilyNames = {"No classification",
                                  "IBM Neo-grotesque Gothic",
                                  "Humanist",
                                  "Low-x Round Geometric",
                                  "High-x Round Geometric",
                                  "Neo-grotesque Gothic",
                                  "Modified neo-grotesque Gothic",
                                  "Reserved",
                                  "Reserved",
                                  "Typewriter Gothic",
                                  "Matrix",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Miscellaneous"};
            break;
        case 9: subFamilyNames = {"No classification",
                                  "Engraver",
                                  "Black Letter",
                                  "Decorative",
                                  "Three Dimensional",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Reserved",
                                  "Miscellaneous"};
            break;
        case 10: subFamilyNames = {"No classification",
                                   "Uncial",
                                   "Brush Joined",
                                   "Formal Joined",
                                   "Monotone Joined",
                                   "Calligraphic",
                                   "Brush Unjoined",
                                   "Formal Unjoined",
                                   "Monotone Unjoined",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Miscellaneous"};
            break;
        case 12: subFamilyNames = {"No classification",
                                   "Reserved",
                                   "Reserved",
                                   "Mixed Serif",
                                   "Reserved",
                                   "Reserved",
                                   "Oldstyle Serif",
                                   "Neo-grotesque Sans Serif",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Reserved",
                                   "Miscellaneous"};
            break;
                    
        default:
            break;
        }
            
        if (subFamily < subFamilyNames.size())
            return subFamilyNames[subFamily];
            
        return "Unknown SubFamily Class";
    }

    static std::string OS2GetFamilyClassFullName(uint16_t value) {
        return OS2GetFamilyClassName(value) + std::string(OS2GetSubFamilyClassName(value));
    }
        
    static auto OS2GetUnicodeRanges(uint32_t range1, uint32_t range2, uint32_t range3, uint32_t range4) {
        std::vector<std::vector<const char *> > allBlockNames
            = {{"Basic Latin"},
               {"Latin-1 Supplement"},
               {"Latin Extended-A"},
               {"Latin Extended-B"},
               {"IPA Extensions", "Phonetic Extensions", "Phonetic Extensions Supplement"},
               {"Spacing Modifier Letters", "Modifier Tone Letters"},
               {"Combining Diacritical Marks", "Combining Diacritical Marks Supplement"},
               {"Greek and Coptic"},
               {"Coptic"},
               {"Cyrillic", "Cyrillic Supplement", "Cyrillic Extended-A", "Cyrillic Extended-B"},
               {"Armenian"},
               {"Hebrew"},
               {"Vai"},
               {"Arabic", "Arabic Supplement"},
               {"NKo"},
               {"Devanagari"},
               {"Bengali"},
               {"Gurmukhi"},
               {"Gujarati"},
               {"Oriya"},
               {"Tamil"},
               {"Telugu"},
               {"Kannada"},
               {"Malayalam"},
               {"Thai"},
               {"Lao"},
               {"Georgian", "Georgian Supplement"},
               {"Balinese"},
               {"Hangul Jamo"},
               {"Latin Extended Additional", "Latin Extended-C", "Latin Extended-D"},
               {"Greek Extended"},
               {"General Punctuation", "Supplemental Punctuation"},
               {"Superscripts And Subscripts"},
               {"Currency Symbols"},
               {"Combining Diacritical Marks For Symbols"},
               {"Letterlike Symbols"},
               {"Number Forms"},
               {"Arrows", "Supplemental Arrows-A", "Supplemental Arrows-B", "Miscellaneous Symbols and Arrows"},
               {"Mathematical Operators", "Supplemental Mathematical Operators", "Miscellaneous Mathematical Symbols-A", "Miscellaneous Mathematical Symbols-B"},
               {"Miscellaneous Technical"},
               {"Control Pictures"},
               {"Optical Character Recognition"},
               {"Enclosed Alphanumerics"},
               {"Box Drawing"},
               {"Block Elements"},
               {"Geometric Shapes"},
               {"Miscellaneous Symbols"},
               {"Dingbats"},
               {"CJK Symbols And Punctuation"},
               {"Hiragana"},
               {"Katakana", "Katakana Phonetic Extensions"},
               {"Bopomofo", "Bopomofo Extended"},
               {"Hangul Compatibility Jamo"},
               {"Phags-pa"},
               {"Enclosed CJK Letters And Months"},
               {"CJK Compatibility"},
               {"Hangul Syllables"},
               {"High Surrogates", "High Private Use Surrogates", "Low Surrogates"},
               {"Phoenician"},
               {"CJK Unified Ideographs", "CJK Radicals Supplement", "Kangxi Radicals", "Ideographic Description Characters", "CJK Unified Ideographs Extension A", "CJK Unified Ideographs Extension B", "Kanbun"},
               {"Private Use Area"},
               {"CJK Strokes", "CJK Compatibility Ideographs", "CJK Compatibility Ideographs Supplement"},
               {"Alphabetic Presentation Forms"},
               {"Arabic Presentation Forms-A"},
               {"Combining Half Marks"},
               {"Vertical Forms", "CJK Compatibility Forms"},
               {"Small Form Variants"},
               {"Arabic Presentation Forms-B"},
               {"Halfwidth And Fullwidth Forms"},
               {"Specials"},
               {"Tibetan"},
               {"Syriac"},
               {"Thaana"},
               {"Sinhala"},
               {"Myanmar"},
               {"Ethiopic", "Ethiopic Supplement", "Ethiopic Extended"},
               {"Cherokee"},
               {"Unified Canadian Aboriginal Syllabics"},
               {"Ogham"},
               {"Runic"},
               {"Khmer", "Khmer Symbols"},
               {"Mongolian"},
               {"Braille Patterns"},
               {"Yi Syllables", "Yi Radicals"},
               {"Tagalog", "Hanunoo", "Buhid", "Tagbanwa"},
               {"Old Italic"},
               {"Gothic"},
               {"Deseret"},
               {"Byzantine Musical Symbols", "Musical Symbols", "Ancient Greek Musical Notation"},
               {"Mathematical Alphanumeric Symbols"},
               {"Supplementary Private Use Area-A", "Supplementary Private Use Area-B"},
               {"Variation Selectors", "Variation Selectors Supplement"},
               {"Tags"},
               {"Limbu"},
               {"Tai Le"},
               {"New Tai Lue"},
               {"Buginese"},
               {"Glagolitic"},
               {"Tifinagh"},
               {"Yijing Hexagram Symbols"},
               {"Syloti Nagri"},
               {"Linear B Syllabary", "Linear B Ideograms", "Aegean Numbers"},
               {"Ancient Greek Numbers"},
               {"Ugaritic"},
               {"Old Persian"},
               {"Shavian"},
               {"Osmanya"},
               {"Cypriot Syllabary"},
               {"Kharoshthi"},
               {"Tai Xuan Jing Symbols"},
               {"Cuneiform", "Cuneiform Numbers and Punctuation"},
               {"Counting Rod Numerals"},
               {"Sundanese"},
               {"Lepcha"},
               {"Ol Chiki"},
               {"Saurashtra"},
               {"Kayah Li"},
               {"Rejang"},
               {"Cham"},
               {"Ancient Symbols"},
               {"Phaistos Disc"},
               {"Carian", "Lycian", "Lydian"},
               {"Domino Tiles", "Mahjong Tiles"},
        };
            
        std::vector<std::string> blockNames;
        uint32_t range[] = {range1, range2, range3, range4};
        for (size_t i = 0; i < allBlockNames.size(); ++ i) {
            unsigned char k = i % 32;
            unsigned char f = i / 32;
            if (range[f] & (1 << k)) {
                auto & names = allBlockNames[i];
                blockNames.insert(blockNames.end(), names.begin(), names.end());
            }
        }
        return blockNames;
    }
        
    static auto OS2GetFsSelectionDescription(uint16_t value) {
        std::vector<std::string> names;
            
        if (value & (1 << 0)) names.push_back("Italic");
        if (value & (1 << 1)) names.push_back("Underscore");
        if (value & (1 << 2)) names.push_back("Negative");
        if (value & (1 << 3)) names.push_back("Outlined");
        if (value & (1 << 4)) names.push_back("Strikeout");
        if (value & (1 << 5)) names.push_back("Bold");
        if (value & (1 << 6)) names.push_back("Regular");
        if (value & (1 << 7)) names.push_back("Use Typo Metrics");
        if (value & (1 << 8)) names.push_back("WWS");
        if (value & (1 << 9)) names.push_back("Oblique");
            
            
        return names;
    }
        
    static auto OS2GetCodePageRanges(uint32_t range1, uint32_t range2) {
        std::vector<std::pair<const char *, const char *>> allCodePages
            = {
            {"1252" , "Latin 1"},
            {"1250" , "Latin 2: Eastern Europe"},
            {"1251" , "Cyrillic"},
            {"1253" , "Greek"},
            {"1254" , "Turkish"},
            {"1255" , "Hebrew"},
            {"1256" , "Arabic"},
            {"1257" , "Windows Baltic"},
            {"1258" , "Vietnamese"},
            {"udef" , "Reserved for Alternate ANSI"},
            {"udef" , "Reserved for Alternate ANSI"},
            {"udef" , "Reserved for Alternate ANSI"},
            {"udef" , "Reserved for Alternate ANSI"},
            {"udef" , "Reserved for Alternate ANSI"},
            {"udef" , "Reserved for Alternate ANSI"},
            {"udef" , "Reserved for Alternate ANSI"},
            {"874"  , "Thai"},
            {"932"  , "JIS/Japan"},
            {"936"  , "Chinese: Simplified chars--PRC and Singapore"},
            {"949"  , "Korean Wansung"},
            {"950"  , "Chinese: Traditional chars--Taiwan and Hong Kong"},
            {"1361" , "Korean Johab"},
            {"udef" , "Reserved for Alternate ANSI & OEM"},
            {"udef" , "Reserved for Alternate ANSI & OEM"},
            {"udef" , "Reserved for Alternate ANSI & OEM"},
            {"udef" , "Reserved for Alternate ANSI & OEM"},
            {"udef" , "Reserved for Alternate ANSI & OEM"},
            {"udef" , "Reserved for Alternate ANSI & OEM"},
            {"udef" , "Reserved for Alternate ANSI & OEM"},
            {"udef" , "Macintosh Character Set (US Roman)"},
            {"udef" , "OEM Character Set"},
            {"udef" , "Symbol Character Set"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"udef" , "Reserved for OEM"},
            {"869"  , "IBM Greek"},
            {"866"  , "MS-DOS Russian"},
            {"865"  , "MS-DOS Nordic"},
            {"864"  , "Arabic"},
            {"863"  , "MS-DOS Canadian French"},
            {"862"  , "Hebrew"},
            {"861"  , "MS-DOS Icelandic"},
            {"860"  , "MS-DOS Portuguese"},
            {"857"  , "IBM Turkish"},
            {"855"  , "IBM Cyrillic; primarily Russian"},
            {"852"  , "Latin 2"},
            {"775"  , "MS-DOS Baltic"},
            {"737"  , "Greek; former 437 G"},
            {"708"  , "Arabic; ASMO 708"},
            {"850"  , "WE/Latin 1"},
            {"437"  , "US"},
        };
                
        std::vector<std::string> codePages;
                
        uint32_t range[] = {range1, range2};
        for (size_t i = 0; i < allCodePages.size(); ++ i) {
            unsigned char k = i % 32;
            unsigned char f = i / 32;
            if (range[f] & (1 << k)) {
                auto code = allCodePages[i].first;
                auto name = allCodePages[i].second;
                if (atoi(code))
                    codePages.push_back(code + std::string(" ") + name);
                else
                    codePages.push_back(name);
            }
        }
        return codePages;
    }
        
    template <typename T>
    static const char * OS2PanoseSearchInArray(uint8_t value, const std::initializer_list<T> & array) {
            
        auto itr = std::begin(array);
        if (std::distance(itr, std::end(array)) <= value)
            return "";
        std::advance(itr, value);
        return *itr;
    }
        
    static const char * OS2GetPanoseFamilyType(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Text and Display",
                "Script",
                "Decorative",
                "Pictorial",
                
            });
    }
        
    static const char * OS2GetPanoseSerifType(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Cove",
                "Obtuse Cove",
                "Square Cove",
                "Obtuse Square Cove",
                "Square",
                "Thin",
                "Bone",
                "Exaggerated",
                "Triangle",
                "Normal Sans",
                "Obtuse Sans",
                "Perp Sans",
                "Flared",
                "Rounded",
            });
    }
        
    static const char * OS2GetPanoseWeight(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Very Light",
                "Light",
                "Thin",
                "Book",
                "Medium",
                "Demi",
                "Bold",
                "Heavy",
                "Black",
                "Nord",
                
            });
    }
        
    static const char * OS2GetPanoseProportion(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Old Style",
                "Modern",
                "Even Width",
                "Expanded",
                "Condensed",
                "Very Expanded",
                "Very Condensed",
                "Monospaced",
                
            });
    }
    static const char * OS2GetPanoseContrast(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "None",
                "Very Low",
                "Low",
                "Medium Low",
                "Medium",
                "Medium High",
                "High",
                "Very High",
                
            });
    }
    static const char * OS2GetPanoseStrokeVariation(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Gradual/Diagonal",
                "Gradual/Transitional",
                "Gradual/Vertical",
                "Gradual/Horizontal",
                "Rapid/Vertical",
                "Rapid/Horizontal",
                "Instant/Vertical",
                
            });
    }
    static const char * OS2GetPanoseArmStyle(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Straight Arms/Horizontal",
                "Straight Arms/Wedge",
                "Straight Arms/Vertical",
                "Straight Arms/Single Serif",
                "Straight Arms/Double Serif",
                "Non-Straight Arms/Horizontal",
                "Non-Straight Arms/Wedge",
                "Non-Straight Arms/Vertical",
                "Non-Straight Arms/Single Serif",
                "Non-Straight Arms/Double Serif",
                
            });
    }
    static const char * OS2GetPanoseLetterform(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Normal/Contact",
                "Normal/Weighted",
                "Normal/Boxed",
                "Normal/Flattened",
                "Normal/Rounded",
                "Normal/Off Center",
                "Normal/Square",
                "Oblique/Contact",
                "Oblique/Weighted",
                "Oblique/Boxed",
                "Oblique/Flattened",
                "Oblique/Rounded",
                "Oblique/Off Center",
                "Oblique/Square",
                
            });
    }
    static const char * OS2GetPanoseMidline(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Standard/Trimmed",
                "Standard/Pointed",
                "Standard/Serifed",
                "High/Trimmed",
                "High/Pointed",
                "High/Serifed",
                "Constant/Trimmed",
                "Constant/Pointed",
                "Constant/Serifed",
                "Low/Trimmed",
                "Low/Pointed",
                "Low/Serifed",
                
            });
    }
    static const char * OS2GetPanoseXHeight(uint8_t value) {
        return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Constant/Small",
                "Constant/Standard",
                "Constant/Large",
                "Ducking/Small",
                "Ducking/Standard",
                "Ducking/Large",
                
            });
    }

    static const char * PostGetMacintoshGlyphName(size_t index) {
        const char * names [] =
            {
                ".notdef",
                ".null",
                "nonmarkingreturn",
                "space",
                "exclam",
                "quotedbl",
                "numbersign",
                "dollar",
                "percent",
                "ampersand",
                "quotesingle",
                "parenleft",
                "parenright",
                "asterisk",
                "plus",
                "comma",
                "hyphen",
                "period",
                "slash",
                "zero",
                "one",
                "two",
                "three",
                "four",
                "five",
                "six",
                "seven",
                "eight",
                "nine",
                "colon",
                "semicolon",
                "less",
                "equal",
                "greater",
                "question",
                "at",
                "A",
                "B",
                "C",
                "D",
                "E",
                "F",
                "G",
                "H",
                "I",
                "J",
                "K",
                "L",
                "M",
                "N",
                "O",
                "P",
                "Q",
                "R",
                "S",
                "T",
                "U",
                "V",
                "W",
                "X",
                "Y",
                "Z",
                "bracketleft",
                "backslash",
                "bracketright",
                "asciicircum",
                "underscore",
                "grave",
                "a",
                "b",
                "c",
                "d",
                "e",
                "f",
                "g",
                "h",
                "i",
                "j",
                "k",
                "l",
                "m",
                "n",
                "o",
                "p",
                "q",
                "r",
                "s",
                "t",
                "u",
                "v",
                "w",
                "x",
                "y",
                "z",
                "braceleft",
                "bar",
                "braceright",
                "asciitilde",
                "Adieresis",
                "Aring",
                "Ccedilla",
                "Eacute",
                "Ntilde",
                "Odieresis",
                "Udieresis",
                "aacute",
                "agrave",
                "acircumflex",
                "adieresis",
                "atilde",
                "aring",
                "ccedilla",
                "eacute",
                "egrave",
                "ecircumflex",
                "edieresis",
                "iacute",
                "igrave",
                "icircumflex",
                "idieresis",
                "ntilde",
                "oacute",
                "ograve",
                "ocircumflex",
                "odieresis",
                "otilde",
                "uacute",
                "ugrave",
                "ucircumflex",
                "udieresis",
                "dagger",
                "degree",
                "cent",
                "sterling",
                "section",
                "bullet",
                "paragraph",
                "germandbls",
                "registered",
                "copyright",
                "trademark",
                "acute",
                "dieresis",
                "notequal",
                "AE",
                "Oslash",
                "infinity",
                "plusminus",
                "lessequal",
                "greaterequal",
                "yen",
                "mu",
                "partialdiff",
                "summation",
                "product",
                "pi",
                "integral",
                "ordfeminine",
                "ordmasculine",
                "Omega",
                "ae",
                "oslash",
                "questiondown",
                "exclamdown",
                "logicalnot",
                "radical",
                "florin",
                "approxequal",
                "Delta",
                "guillemotleft",
                "guillemotright",
                "ellipsis",
                "nonbreakingspace",
                "Agrave",
                "Atilde",
                "Otilde",
                "OE",
                "oe",
                "endash",
                "emdash",
                "quotedblleft",
                "quotedblright",
                "quoteleft",
                "quoteright",
                "divide",
                "lozenge",
                "ydieresis",
                "Ydieresis",
                "fraction",
                "currency",
                "guilsinglleft",
                "guilsinglright",
                "fi",
                "fl",
                "daggerdbl",
                "periodcentered",
                "quotesinglbase",
                "quotedblbase",
                "perthousand",
                "Acircumflex",
                "Ecircumflex",
                "Aacute",
                "Edieresis",
                "Egrave",
                "Iacute",
                "Icircumflex",
                "Idieresis",
                "Igrave",
                "Oacute",
                "Ocircumflex",
                "apple",
                "Ograve",
                "Uacute",
                "Ucircumflex",
                "Ugrave",
                "dotlessi",
                "circumflex",
                "tilde",
                "macron",
                "breve",
                "dotaccent",
                "ring",
                "cedilla",
                "hungarumlaut",
                "ogonek",
                "caron",
                "Lslash",
                "lslash",
                "Scaron",
                "scaron",
                "Zcaron",
                "zcaron",
                "brokenbar",
                "Eth",
                "eth",
                "Yacute",
                "yacute",
                "Thorn",
                "thorn",
                "minus",
                "multiply",
                "onesuperior",
                "twosuperior",
                "threesuperior",
                "onehalf",
                "onequarter",
                "threequarters",
                "franc",
                "Gbreve",
                "gbreve",
                "Idotaccent",
                "Scedilla",
                "scedilla",
                "Cacute",
                "cacute",
                "Ccaron",
                "ccaron",
                "dcroat",
            };
        return names[index];
    }        
};
