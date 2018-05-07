#include "FXGlyphCache.h"

FXGlyphCache::FXGlyphCache(size_t capacity) {}

bool
FXGlyphCache::has(FXGlyphID gid) const {
    auto itr = map_.find(gid);
    return itr != map_.end();
}
    
FXGlyph
FXGlyphCache::get(FXGlyphID gid) const {
    assert(has(gid));
    auto itr = map_.find(gid);
    return itr->second.glyph;
}

void
FXGlyphCache::put(FXGlyphID gid, const FXGlyph & g) {
    map_[gid].glyph = g;
}

void
FXGlyphCache::clear() {
    map_.clear();
}

void
FXGlyphCache::gc() {
    
}
