#pragma once
#include "FX.h"

using FXTag = uint32_t;

template <typename T>
constexpr FXTag
FXMakeTag(T a, T b, T c, T d) {
    return FXTag(
        (((uint8_t)(a))<<24) |
        (((uint8_t)(b))<<16) |
        (((uint8_t)(c))<<8) |
        ((uint8_t)(d))
        );
}

static_assert(FXMakeTag('G', 'S', 'U', 'B') == 'GSUB', "Check FXTag literals!");

constexpr FXTag FXTableCMAP    = 'cmap';
constexpr FXTag FXTableGUSB    = 'GSUB';
constexpr FXTag FXTableGPOS    = 'GPOS';
constexpr FXTag FXTableGDEF    = 'GDEF';
constexpr FXTag FXTagInvalid = -1;


template <typename T = FXTag> FXString
FXTag2Str(T tag) {
    char str[] = {
        (char)(tag >> 24),
        (char)(tag >> 16),
        (char)(tag >> 8),
        (char)(tag),
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




