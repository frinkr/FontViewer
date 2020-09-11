#pragma once
#include "FX.h"

struct FXUCDBlock {
    FXChar    from;
    FXChar    to;
    FXString  name;

    bool
    contains(FXChar c) const {
        return c >= from && c < to;
    }
};

struct FXCharCategory {
    int        type;      // UCharCategory
    FXString   abbreviation;
    FXString   fullName;
};

enum class FXEastAsianWidth {
    neutral,
    ambiguous,
    halfWidth,
    fullWidth,
    narrow,
    width
};

extern FXUCDBlock FXUCDInvalidBlock;

class FXUCD {
public:
    explicit FXUCD(const FXString & root);

protected:
    const FXVector<FXUCDBlock> &
    blocks() const;

    const FXUCDBlock &
    block(FXChar c) const;

    const FXVector<FXCharCategory> &
    categories() const;

    friend struct FXUnicode;
protected:
    FXString
    file(const FXString & name) const;
protected:
    FXString                          root_;
    mutable FXVector<FXUCDBlock>      blocks_;
    mutable FXVector<FXCharCategory>  categories_;
};

struct FXUnicode {
public:
    static void
    init(const FXString & ucdRoot);
    
    static FXPtr<FXUCD>
    ucd();

    static const FXVector<FXUCDBlock> &
    blocks();

    static const FXUCDBlock &
    block(FXChar c);
  
    static FXString
    name(FXChar c);

    static FXString
    script(FXChar c);

    static bool
    defined(FXChar c);

    static FXString
    age(FXChar c);

    static const FXCharCategory &
    category(FXChar c);

    static FXVector<FXChar>
    decomposition(FXChar c, bool nfkd = false);

    static FXEastAsianWidth
    eastAsianWidth(FXChar c);
    
    static FXVector<uint8_t>
    utf8(FXChar c);

    static FXString
    utf8Str(FXChar c);
    
    static FXVector<uint16_t>
    utf16(FXChar c);

    static std::u32string
    utf8ToUTF32(const FXString & u8);

    static FXString
    utf16ToUTF8(const std::u16string_view& u16);

    static FXString
    utf16BEToUTF8(const std::u16string_view& u16);

    static FXString
    utf32ToUTF8(const std::u32string_view& u32);

private:
    static FXPtr<FXUCD>    ucd_;
};
