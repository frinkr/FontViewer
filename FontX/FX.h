#pragma once

#include <cassert>
#include <algorithm>
#include <memory>
#include <string>

#include <map>
#include <vector>
#include <set>

template <typename T>
using FXPtr = std::shared_ptr<T>;
template <typename T>
using FXWeakPtr = std::weak_ptr<T>;
template <typename T>
using FXVector = std::vector<T>;
template <typename T>
using FXSet = std::set<T>;
template <typename K, typename V>
using FXMap = std::map<K, V>;

using FXString = std::string;

struct FT_LibraryRec_;
struct FT_FaceRec_;
struct hb_face_t;
struct hb_font_t;

using FXFTLibrary = struct FT_LibraryRec_ *;
using FXFTFace    = struct FT_FaceRec_ *;
using FXHBFace    = struct hb_face_t *;
using FXHBFont    = struct hb_font_t *;

using FXChar      = uint32_t;
using FXGlyphID   = uint32_t;

constexpr FXChar    FXUnicodeMax       = 0x10FFFF;
constexpr FXChar    FXCharInvalidBegin = FXChar(-1) - 0x100; // let's reserve some (256) codepoints
constexpr FXChar    FXCharInvalidEnd   = FXChar(-1);
constexpr FXChar    FXCharInvalid      = FXCharInvalidBegin;
constexpr FXGlyphID FXGIDNotDef        = 0;

/**
 * A closed range
 */
struct FXCharRange {
    FXChar from;
    FXChar to;
};

using fu = short;  // font units

enum FXGCharType {
    FXGCharTypeUnicode,
    FXGCharTypeGlyphID,
    FXGCharTypeOther,   // characters in other encoding
};

struct FXGChar {
    FXGCharType type;
    FXChar      value;

    constexpr FXGChar(FXGCharType type = FXGCharTypeUnicode, FXChar value = FXCharInvalid)
        : type(type)
        , value(value){}

    constexpr bool
    operator<(const FXGChar & u) const {
        return u.type == type && value < u.value;
    }
    
    constexpr bool
    operator==(const FXGChar & u) const {
        return u.type == type && u.value == value;
    }
    
    constexpr bool
    operator!=(const FXGChar & u) const {
        return !operator==(u);
    }

    constexpr bool
    isUnicode() const {
        return type == FXGCharTypeUnicode;
    }

    constexpr bool
    isChar() const {
        return type == FXGCharTypeUnicode || type == FXGCharTypeOther;
    }

    constexpr bool
    isGlyphID() const {
        return type == FXGCharTypeGlyphID;
    }

    constexpr bool
    isValid() const {
        return value != FXCharInvalid;
    }
};

constexpr FXGChar FXGCharInvalid(FXGCharTypeUnicode, FXCharInvalid);

template <typename T>
struct FXVec2d {
    T x = {0};
    T y = {0};
};

template <typename T>
constexpr FXVec2d<T>
FXMakeVec2d(T x, T y) {
    FXVec2d<T> v;
    v.x = x;
    v.y = y;
    return v;
}
