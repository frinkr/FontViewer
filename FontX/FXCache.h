#pragma once
#include <time.h>
#include <algorithm>
#include "FX.h"

template <typename Key, typename Value>
class FXCache {
public:
    explicit FXCache(size_t capacity = 0)
        : capacity_(capacity) {
    }

    bool
    has(Key key) const {
        auto itr = map_.find(key);
        return itr != map_.end();
    }
    
    const Value &
    get(Key key) const {
        assert(has(key));
        auto itr = map_.find(key);
        itr->second.lastAccess = time(0);
        itr->second.accessCount += 1;
        return itr->second.value;
    }

    Value &
    get(Key key) {
        return const_cast<Value&>(const_cast<const FXCache<Key, Value>*>(this)->get(key));
    }

    void
    put(Key key, Value value) {
        map_[key].value = value;
        map_[key].lastAccess = time(0);
    }
    
    void
    remove(Key key) {
       map_.erase(key);
    }

    void
    clear() {
        map_.clear();
    }

    void
    gc() {
        if (capacity_ == 0 || map_.size() <= capacity_)
            return;
        
        time_t now = time(0);
        
        // Sort the keys
        FXVector<Key> keys;
        for (auto & kv: map_)
            keys.emplace_back(kv.first);
        std::sort(keys.begin(), keys.end(), [this, now](auto && keyA, auto && keyB) {
            auto & itemA = map_[keyA];
            auto & itemB = map_[keyB];
            int scoreA = int(now - itemA.lastAccess) * -9 + itemA.accessCount * 1;
            int scoreB = int(now - itemB.lastAccess) * -9 + itemB.accessCount * 1;
            return scoreA < scoreB;
        });
        
        // Remove low score items
        for (size_t i = 0; i < (map_.size() - capacity_); ++ i)
            remove(keys[i]);
    }
protected:
    struct CacheItem {
        Value  value {};
        time_t lastAccess {0};
        size_t accessCount {1};
    };

    size_t capacity_ {10};
    mutable FXMap<Key, CacheItem>  map_;
};
