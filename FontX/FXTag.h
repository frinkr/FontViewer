#pragma once
#include "FX.h"

using FXTag = uint32_t;

template <typename T>
constexpr FXTag
FXMakeTag(T a, T b, T c, T d) {
    return FXTag(
        ((static_cast<uint8_t>(a))<<24) |
        ((static_cast<uint8_t>(b))<<16) |
        ((static_cast<uint8_t>(c))<<8) |
        (static_cast<uint8_t>(d))
        );
}

static_assert(FXMakeTag('G', 'S', 'U', 'B') == 'GSUB', "Check FXTag literals!");

constexpr FXTag FXTableCMAP    = 'cmap';
constexpr FXTag FXTableGSUB    = 'GSUB';
constexpr FXTag FXTableGPOS    = 'GPOS';
constexpr FXTag FXTableGDEF    = 'GDEF';
constexpr FXTag FXTagInvalid = static_cast<FXTag>(-1);


template <typename T = FXTag> FXString
FXTag2Str(T tag) {
    char str[] = {
        static_cast<char>(tag >> 24),
        static_cast<char>(tag >> 16),
        static_cast<char>(tag >> 8),
        static_cast<char>(tag),
        0
    };
    return str;
}

template <typename T = FXString> FXTag
FXStr2Tag(const T & str) {
    if (str.length() != 4)
        return FXTagInvalid;
    FXTag tag = 0;
    for (size_t i = 0; i < 4; ++ i) {
        char c = str[i];
        tag += FXTag(c << (24 - i * 8));
    }
    return tag;
}

