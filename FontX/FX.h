#pragma once

#include <cstring>
#include <cmath>
#include <algorithm>
#include <any>
#include <cassert>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#if defined(__APPLE__)
#  define FX_MAC  1
#elif defined(_WIN32)
#  define FX_WIN  1
#elif defined(__linux__)
#  define FX_LINUX 1
#endif

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
template <typename T>
using FXOpt = std::optional<T>;
template <typename T>
FXOpt<T> FXNone {};

using FXString = std::string;
using FXStringView = std::string_view;

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
    using Byte = unsigned char;
    virtual ~FXStream() {}

    virtual size_t size() const = 0;
    virtual size_t pos() const = 0;
    virtual bool   seek(size_t pos) = 0;
    virtual size_t read(Byte * buffer, size_t count) = 0;
    virtual void   close() = 0;
};

class FXMemoryStream : public FXStream {
public:
    using Deallocator = std::function<void(const Byte *)>;

    FXMemoryStream(const Byte * buffer,
                   size_t length,
                   Deallocator deallocator = {nullptr},
                   size_t pos = 0)
        : buffer_(buffer)
        , length_(length)
        , deallocator_(deallocator)
        , pos_(pos) {
    }

    FXMemoryStream & operator=(const FXMemoryStream &) = delete;

    ~FXMemoryStream() {
        if (deallocator_) 
            deallocator_(buffer_);
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
    read(Byte * buffer, size_t count) override {
        size_t c = std::min(count, length_ - pos_);
        if (c) 
            memcpy(buffer, buffer_ + pos_, c);
        return c;
    }

    void
    close() override {
    }

protected:
    const Byte    * buffer_;
    size_t          length_;
    Deallocator     deallocator_;
    size_t          pos_;
};


class FXDict {
public:
    using data_type = std::map<std::string, std::any>;
    FXDict() {}
        
    FXDict(std::initializer_list<typename data_type::value_type> init) {
        for (const auto & p: init) 
            set(p.first, p.second);
    }
    FXDict(const std::string & key, const std::any & value) {
        set(key, value);
    }
public:
    bool
    has(const std::string & key) const {
        return data_.find(key) != data_.end();
    }
        
    template <typename T> T
    get(const std::string & key) const {
        auto itr = data_.find(key);
        assert(itr != data_.end());
        return std::any_cast<T>(itr->second);
    }
        
    template <typename T> FXDict &
    set(const std::string & key, const T & value) {
        data_[key] = std::any(value);
        return * this;
    }

    void
    clear(const std::string & key) {
        auto itr = data_.find(key);
        if (itr != data_.end())
            data_.erase(itr);
    }

protected:
    std::map<std::string, std::any> data_;
};


template <typename S = FXString>
FXVector<S> FXStringSplit(S line, const S & delimiter) {
    FXVector<S> strs;
    size_t pos {};
    while ((pos = line.find(delimiter)) != std::string::npos) {
        strs.push_back(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
    strs.push_back(std::move(line));
    return strs;
}

template <typename S = FXString>
S FXStringTrimLeft(S s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    return s;
}

template <typename S = FXString>
S FXStringTrimRight(S s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}

template <typename S = FXString>
S FXStringTrim(S s) {
    return FXStringTrimRight(FXStringTrimLeft(s));
}


template <auto Val, auto Message>
void safe_static_assert( ) {
    static_assert(Val);
}

template <class...> constexpr std::false_type FXDependentFalse{};

template <typename T> T
FXSwapBytes(T value) {
    if constexpr (sizeof(T) == 1)
        return value;
    else if constexpr (sizeof(T) == 2)
        return static_cast<T>(((value & 0xff) << 8) + ((value & 0xff00) >> 8));
    else
        static_assert(FXDependentFalse<T>, "not implemented");
}
