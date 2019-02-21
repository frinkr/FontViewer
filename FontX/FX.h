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
using FXFTSize    = struct FT_SizeRec_ *;
using FXHBFace    = struct hb_face_t *;
using FXHBFont    = struct hb_font_t *;

using FXFixed     = signed long;     // 16.16
using FXChar      = uint32_t;
using FXGlyphID   = uint32_t;

constexpr FXChar    FXUnicodeMax       = 0x10FFFF;
constexpr FXChar    FXCharInvalidBegin = FXChar(-1) - 0x100; // let's reserve some (256) codepoints
constexpr FXChar    FXCharInvalidEnd   = FXChar(-1);
constexpr FXChar    FXCharInvalid      = FXCharInvalidBegin;
constexpr FXGlyphID FXGIDNotDef        = 0;

constexpr double    FXDefaultFontSize  = 150;
constexpr double    FXDefaultDPI       = 72;

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

    constexpr FXGChar(FXChar value = FXCharInvalid, FXGCharType type = FXGCharTypeUnicode)
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

constexpr FXGChar FXGCharInvalid;

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

template <typename T>
struct FXRect {
    T left = {0};
    T right = {0};
    T top = {0};
    T bottom = {0};

    T
    width() const {
        return right - left;
    }

    T
    height() const {
        return top - bottom;
    }
};

template <typename T>
constexpr FXRect<T>
FXMakeRect(T left, T top, T right, T bottom) {
    FXRect<T> rect;
    rect.left = left;
    rect.right = right;
    rect.top = top;
    rect.bottom = bottom;
    return rect;
}

class FXStream {
public:
    virtual ~FXStream() {}

    virtual size_t size() const = 0;
    virtual size_t pos() const = 0;
    virtual bool   seek(size_t pos) = 0;
    virtual size_t read(unsigned char * buffer, size_t count) = 0;
    virtual void   close() = 0;
};

class FXMemoryStream : public FXStream {
public:
    FXMemoryStream(unsigned char * buffer, size_t length, size_t pos = 0)
        : buffer_(buffer)
        , length_(length)
        , pos_(pos) {
    }

    size_t
    size() const override {
        return length_;
    }

    size_t
    pos() const override {
        return pos_;
    }

    bool
    seek(size_t pos) override {
        pos_ = pos;
        return true;
    }

    size_t
    read(unsigned char * buffer, size_t count) override {
        size_t c = std::min(count, length_ - pos_);
        if (c) 
            memcpy(buffer, buffer_ + pos_, c);
        return c;
    }

    void
    close() override {
    }

protected:
    unsigned char * buffer_;
    size_t          length_;
    size_t          pos_;
};
