#pragma once

#include "FX.h"

using FXARGB = uint32_t;
using FXGray = uint8_t;

template <typename COLOR = FXARGB, typename POSITION = int>
class FXBitmap {
        
public:
    using ColorType  = COLOR;
    using PosType    = POSITION;
    
public:
    FXBitmap()
        : width(0)
        , height(0)
        , pitch(0)
        , buffer(nullptr)
        , ref(0){}

    FXBitmap(PosType w, PosType h)
        : width(w)
        , height(h)
        , buffer(nullptr)
        , ref(nullptr)
    {
        pitch = width * sizeof(ColorType);

        if (w && h) {
            ref = new size_t(1);
            buffer = new ColorType[width * height]();
        }
    }

    FXBitmap(const FXBitmap & other)
        : width(other.width)
        , height(other.height)
        , pitch(other.pitch)
        , buffer(other.buffer)
        , ref(other.ref)
    {
        if (ref)
            ++ *ref;
    }

    FXBitmap(FXBitmap && other)
        : width(other.width)
        , height(other.height)
        , pitch(other.pitch)
        , buffer(other.buffer)
        , ref(other.ref)
    {
        other.ref = nullptr;
        other.buffer = nullptr;
    }
        
    ~FXBitmap() {
        if (!ref)
            return;

        -- *ref;
        if (!*ref) {
            delete ref;
            delete[] buffer;
        }
    }

    FXBitmap & operator=(FXBitmap && other) {
        FXBitmap m(std::move(other));
        return swap(m);
    }
    
    FXBitmap & operator=(const FXBitmap & other) {
        FXBitmap m(other);
        return swap(m);
    }

    FXBitmap &
    swap(FXBitmap & other) {
        std::swap(width, other.width);
        std::swap(height, other.height);
        std::swap(pitch, other.pitch);
        std::swap(ref, other.ref);
        std::swap(buffer, other.buffer);
        return *this;
    }
    
    void
    clear(ColorType color) {
#if __APPLE__
        memset_pattern4(buffer, &color, width * height * 4);
#else
        for (pos_type y = 0; y < height; ++ y)
            for (pos_type x = 0; x < width; ++ x)
                set_pixel(x, y, color);
#endif        
    }

    bool
    empty() const {
        return !width || !height || !buffer;
    }
        
    void
    setPixel(PosType x, PosType y, ColorType color) {
        if (!validPosition(x, y))
            return;
        
        *(buffer + y * width + x) = color;
    }

    ColorType
    pixel(PosType x, PosType y) const {
        if (!validPosition(x, y))
            return 0;
        
        return *(buffer + y * width + x);
    }
        
    bool
    validPosition(PosType x, PosType y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
        
public:
    PosType     width {};
    PosType     height {};
    PosType     pitch {};
    ColorType * buffer {};

protected:
    size_t  * ref {};
};


inline constexpr FXARGB
makeARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return (a << 24) + (r << 16) + (g << 8) + (b << 0);
}

inline constexpr FXARGB
makeARGB(uint8_t a, const FXARGB & rgb) {
    return (a << 24) + (0x00ffffff & rgb);
}

inline constexpr FXARGB
makeRGB(uint8_t r, uint8_t g, uint8_t b) {
    return makeARGB(0xff, r, g, b);
}

inline  void
get_argb(FXARGB argb, uint8_t &a, uint8_t &r, uint8_t &g, uint8_t &b) {
    a = (0xff000000 & argb) >> 24;
    r = (0x00ff0000 & argb) >> 16;
    g = (0x0000ff00 & argb) >> 8;
    b = (0x000000ff & argb) >> 0;
}

constexpr FXARGB FXRed    = makeRGB(0xff, 0, 0);
constexpr FXARGB FXGreen  = makeRGB(0, 0xff, 0);
constexpr FXARGB FXBlue   = makeRGB(0, 0, 0xff);
constexpr FXARGB FXBlack  = makeRGB(0, 0, 0);
constexpr FXARGB FXWhite  = makeRGB(0xff, 0xff, 0xff);

using FXBitmapARGB = FXBitmap<FXARGB>;
