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

extern FXUCDBlock FXUCDInvalidBlock;

class FXUCD {
public:
    explicit FXUCD(const FXString & root);

    const FXVector<FXUCDBlock> &
    blocks() const;

    const FXUCDBlock &
    block(FXChar c) const;

protected:
    FXString
    file(const FXString & name) const;
protected:
    FXString                      root_;
    mutable FXVector<FXUCDBlock>  blocks_;
};
