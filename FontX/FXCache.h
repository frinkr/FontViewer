#pragma once
#include <time.h>
#include <algorithm>
#include <list>
#include <unordered_map>
#include "FX.h"

/**
 * A LRU cache
 */
template <typename Key, typename Value>
class FXCache {
public:
    using KeyValue = std::pair<Key, Value>;
    
    explicit FXCache(size_t capacity = 10)
        : capacity_(capacity) {
    }

    bool
    has(const Key & key) const {
        auto itr = map_.find(key);
        return itr != map_.end();
    }
    
    const Value &
    get(const Key & key)  {
        assert(has(key));
        auto itr = map_.find(key);
        list_.splice(list_.begin(), list_, itr->second);
        return itr->second->second;
    }

    void
    put(const Key & key, const Value & value) {
        auto itr = map_.find(key);
        list_.push_front({key, value});
        
        if (itr != map_.end()) {
            list_.erase(itr->second);
            map_.erase(itr);
        }
        
        map_[key] = list_.begin();
        if (map_.size() > capacity_) {
            map_.erase(list_.back().first);
            list_.pop_back();
        }
    }
    
    void
    remove(const Key & key) {
        auto itr = map_.find(key);
        list_.erase(itr->second);
        map_.erase(itr);
    }

    void
    clear() {
        list_.clear();
        map_.clear();
    }

protected:
    std::list<KeyValue> list_;
    std::unordered_map<Key, typename std::list<KeyValue>::iterator> map_;

    size_t capacity_ {10};
    
};
