#include <iostream>
#include <fstream>

#include <unicode/uchar.h>
#include <unicode/uscript.h>
#include <unicode/unorm2.h>
#include <unicode/unistr.h>
#include "FXUnicode.h"
#include "FXFS.h"

using namespace std::string_literals;

namespace {
    class FXUCDFileReader {
    public:
        FXUCDFileReader(const FXString & filePath, bool ignoreBlankLine = true, FXChar commentChar = '#')
            : filePath_(filePath)
            , ignoreBlankLine_(ignoreBlankLine)
            , commentChar_(commentChar)
            , stream_(filePath_)
        {}

        bool
        readLine(FXString & line) {
            while(std::getline(stream_, line)) {
                line = FXStringTrim(line);
                if (commentChar_ && line.size() && line[0] == commentChar_)
                    continue;
                if (ignoreBlankLine_ && line.empty())
                    continue;
                return true;
            }

            return false;
        }

        static bool
        parseLine(const FXString & line, FXCharRange & range, FXString & text) {
            FXVector<FXString> strs = FXStringSplit(line, ";"s);
            if (strs.size() == 2) {
                if (parseCharRange(FXStringTrim(strs[0]), range)) {
                    text = FXStringTrim(strs[1]);
                    return true;
                }
            }
            return false;
        }

        static bool
        parseCharRange(const FXString & line, FXCharRange & range) {
            FXVector<FXString> strs = FXStringSplit(line, ".."s);
            
            if (strs.size() == 1) {
                FXChar c;
                if (parseHexChar(strs[0], c)) {
                    range.from = range.to = c;
                    return true;
                }
            }
            else if (strs.size() == 2) {
                FXChar c1, c2;
                if (parseHexChar(strs[0], c1) && parseHexChar(strs[1], c2)) {
                    range.from = c1;
                    range.to = c2;
                    return true;
                }
            }
            return false;
        }

        static bool
        parseHexChar(const FXString & str, FXChar & c) {
            size_t pos = 0;
            FXChar result = std::stoul(str, &pos, 16);
            if (pos != str.length())
                return false;
            c = result;
            return true;
        }
        
    protected:
        FXString      filePath_;
        bool          ignoreBlankLine_;
        FXChar        commentChar_;
        std::ifstream stream_;
    };
}

FXUCDBlock FXUCDInvalidBlock {FXCharInvalid, FXCharInvalid, "Invalid Block"};

FXUCD::FXUCD(const FXString & root)
    : root_(root) {}

const FXVector<FXUCDBlock> &
FXUCD::blocks() const {
    if (blocks_.empty()) {
        // load blocks
        FXUCDFileReader reader(file("Blocks.txt"));
        FXString line;
        
        while (reader.readLine(line)) {
            FXCharRange range;
            FXString text;
            if (reader.parseLine(line, range, text)) {
                FXUCDBlock block {range.from, range.to + 1, text};
                blocks_.push_back(block);
            }
        }
    }
    return blocks_;
}

const FXUCDBlock &
FXUCD::block(FXChar c) const {
    auto const & blks = blocks();
    auto itr = std::find_if(blks.begin(), blks.end(), [c](const FXUCDBlock & block) {
        return block.contains(c);
    });

    if ((itr != blks.end()))
        return *itr;
    return FXUCDInvalidBlock;
}

const FXVector<FXCharCategory> &
FXUCD::categories() const {
    if (categories_.empty()) {
        categories_ = FXVector<FXCharCategory>({
                {U_UPPERCASE_LETTER,       "Lu", "Letter, Uppercase"},
                {U_LOWERCASE_LETTER,       "Ll", "Letter, Lowercase"},
                {U_TITLECASE_LETTER,       "Lt", "Letter, Titlecase"},
                {U_MODIFIER_LETTER,        "Lm", "Letter, Modifier"},
                {U_OTHER_LETTER,           "Lo", "Letter, Other"},
                {U_NON_SPACING_MARK,       "Mn", "Mark, Nonspacing"},
                {U_COMBINING_SPACING_MARK, "Mc", "Mark, Spacing Combining"},
                {U_ENCLOSING_MARK,         "Me", "Mark, Enclosing"},
                {U_DECIMAL_DIGIT_NUMBER,   "Nd", "Number, Decimal Digit"},
                {U_LETTER_NUMBER,          "Nl", "Number, Letter"},
                {U_OTHER_NUMBER,           "No", "Number, Other"},
                {U_CONNECTOR_PUNCTUATION,  "Pc", "Punctuation, Connector"},
                {U_DASH_PUNCTUATION,       "Pd", "Punctuation, Dash"},
                {U_START_PUNCTUATION,      "Ps", "Punctuation, Open"},
                {U_END_PUNCTUATION,        "Pe", "Punctuation, Close"},
                {U_INITIAL_PUNCTUATION,    "Pi", "Punctuation, Initial quote"},
                {U_FINAL_PUNCTUATION,      "Pf", "Punctuation, Final quote"},
                {U_OTHER_PUNCTUATION,      "Po", "Punctuation, Other"},
                {U_MATH_SYMBOL,            "Sm", "Symbol, Math"},
                {U_CURRENCY_SYMBOL,        "Sc", "Symbol, Currency"},
                {U_MODIFIER_SYMBOL,        "Sk", "Symbol, Modifier"},
                {U_OTHER_SYMBOL,           "So", "Symbol, Other"},
                {U_SPACE_SEPARATOR,        "Zs", "Separator, Space"},
                {U_LINE_SEPARATOR,         "Zl", "Separator, Line"},
                {U_PARAGRAPH_SEPARATOR,    "Zp", "Separator, Paragraph"},
                {U_CONTROL_CHAR,           "Cc", "Other, Control"},
                {U_FORMAT_CHAR,            "Cf", "Other, Format"},
                {U_SURROGATE,              "Cs", "Other, Surrogate"},
                {U_PRIVATE_USE_CHAR,       "Co", "Other, Private Use"},
                {U_UNASSIGNED,             "Cn", "Other, Not Assigned"},
                    });
    }

    return categories_;
}

FXString
FXUCD::file(const FXString & name) const {
    return FXFS::pathJoin({root_, name});
}

/////////////////////////////////////////////////////////////////////////////////////
//               FXUNICODE

FXPtr<FXUCD> FXUnicode::ucd_;

void
FXUnicode::init(const FXString & ucdRoot) {
    ucd_.reset(new FXUCD(ucdRoot));
}
    
FXPtr<FXUCD>
FXUnicode::ucd() {
    return ucd_;
}

const FXVector<FXUCDBlock> &
FXUnicode::blocks() {
    return ucd()->blocks();
}

const  FXUCDBlock &
FXUnicode::block(FXChar c) {
    return ucd()->block(c);
}
  
FXString
FXUnicode::name(FXChar c) {
    char name[1024] = {0};
    UErrorCode error = U_ZERO_ERROR;
    u_charName(c, U_UNICODE_CHAR_NAME, name, sizeof(name), &error);
    return name;
}

FXString
FXUnicode::script(FXChar c) {
    UErrorCode error = U_ZERO_ERROR;

    UScriptCode script = uscript_getScript(c, &error);
    if (!error) 
        return uscript_getName(script);

    return FXString();
}

bool
FXUnicode::defined(FXChar c) {
    return u_isdefined(c);
}

FXString
FXUnicode::age(FXChar c) {
    UVersionInfo version;
    u_charAge(c, version);

    const UVersionInfo v1_0_0 = {1, 0, 0, 0};
    const UVersionInfo v1_0_1 = {1, 0, 1, 0};

    if (!memcmp(version, v1_0_0, sizeof(UVersionInfo)))
        return "1.0.0";
    if (!memcmp(version, v1_0_1, sizeof(UVersionInfo)))
        return "1.0.1";

    return std::to_string(version[0]) + "." + std::to_string(version[1]);
}

const FXCharCategory &
FXUnicode::category(FXChar c) {
    const auto & cats = ucd()->categories();
    int t = u_charType(c);
    for (const auto & cat : cats) {
        if (cat.type == t)
            return cat;
    }
    static FXCharCategory dummy;
    return dummy;
}

FXVector<FXChar>
FXUnicode::decomposition(FXChar c, bool nfkd) {
    UErrorCode error = U_ZERO_ERROR;
    const UNormalizer2 * nfdNormalizer = unorm2_getNFDInstance(&error);
    const UNormalizer2 * nfkdNormalizer = unorm2_getNFKDInstance(&error);

    UChar components[16] = {0};
    int32_t len = unorm2_getDecomposition(
        nfkd? nfkdNormalizer: nfdNormalizer,
        c,
        components,
        sizeof(components),
        &error);
    
    FXVector<FXChar> d;
    if (len > 0) {
        for (int32_t i = 0; i < len; ++ i)
            d.push_back(components[i]);
    }

    return d;
}

FXVector<uint8_t>
FXUnicode::utf8(FXChar c) {
    auto str = utf8Str(c);
    const uint8_t * buf = (const uint8_t*) &str[0];
    return FXVector<uint8_t>(buf, buf + str.size());
}

FXString
FXUnicode::utf8Str(FXChar c) {
    icu::UnicodeString u((UChar32)c);
    std::string str;
    u.toUTF8String(str);
    return str;
}

FXVector<uint16_t>
FXUnicode::utf16(FXChar c) {
    uint16_t h{}, l{};
    if (c < 0x10000) {
        l = c;
    }
    else {
        auto t = c - 0x10000;
        h = (((t << 12) >> 22) + 0xD800);
        l = (((t << 22) >> 22) + 0xDC00);
    }
    if (h)
        return { h, l };
    else
        return { l };
}

std::u32string
FXUnicode::utf8ToUTF32(const FXString & u8) {
    icu::UnicodeString u = icu::UnicodeString::fromUTF8(u8);

    UErrorCode error;
    std::u32string u32(u8.size(), 0);
    auto length = u.toUTF32((UChar32*)u32.data(), u32.size(), error);
    if (!length || U_FAILURE(error))
        return std::u32string();
    if (length != u32.size())
        u32.resize(length);
    return u32;
}

FXString
FXUnicode::utf16ToUTF8(const std::u16string_view& u16) {
    icu::UnicodeString uStr((const uint16_t*)(u16.data()), static_cast<int32_t>(u16.size()));
    FXString u8;
    return uStr.toUTF8String(u8);
}

FXString
FXUnicode::utf16BEToUTF8(const std::u16string_view& u16BE) {
    std::u16string u16(u16BE.size(), 0);
    for (size_t i = 0; i < u16BE.size(); ++i) 
        u16[i] = FXSwapBytes(u16BE[i]);
    return utf16ToUTF8(u16);
}
