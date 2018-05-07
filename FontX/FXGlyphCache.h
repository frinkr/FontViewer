#pragma once
#include "FXGlyph.h"

class FXGlyphCache {
public:
    explicit FXGlyphCache(size_t capacity = -1);

    bool
    has(FXGlyphID gid) const;
    
    FXGlyph
    get(FXGlyphID gid) const;

    void
    put(FXGlyphID gid, const FXGlyph & g);

    void
    clear();

    void
    gc();

protected:
    struct CacheItem {
        size_t  lastAccess;
        FXGlyph glyph;
    };
    
    FXMap<FXGlyphID, CacheItem>  map_;
};
