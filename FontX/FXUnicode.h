#pragma once
#include "FX.h"

struct FXUCDBlock {
    FXChar    from;
    FXChar    to;
    FXString  name;

    bool
    contains(FXChar c) const {
        return c >= from && c <= to;
    }
};

struct FXUCDData {
  FXChar     ch;
  
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

    friend struct FXUnicode;
protected:
    FXString
    file(const FXString & name) const;
protected:
    FXString                      root_;
    mutable FXVector<FXUCDBlock>  blocks_;
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

private:
    static FXPtr<FXUCD>    ucd_;
};
