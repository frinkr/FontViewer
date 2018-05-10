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


constexpr FXTag FXTagCMAP = 'cmap';
constexpr FXTag FXTagGUSB = 'GSUB';
constexpr FXTag FXTagGPOS = 'GPOS';
constexpr FXTag FXTagGDEF = 'GDEF';

constexpr FXTag FXTagOTDefaultScript   = 'DLFT';
constexpr FXTag FXTagOTDefaultLanguage = 'dlft';



