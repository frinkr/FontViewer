#pragma once
#include "FX.h"

template <typename T>
class FXIterator {
public:
    virtual ~FXIterator() {}

    virtual FXOpt<size_t>
    size() const {
        return FXNone<size_t>;
    }

    virtual bool
    reset() {
        return false;
    }

    virtual FXOpt<T>
    next() {
        return FXNone<T>;
    }
};

template <template<class> class Container, typename T>
class FXContainerIterator : public FXIterator<T> {
public:
    FXContainerIterator() {}
    
    explicit FXContainerIterator(Container<T> && list) {
        setList(std::forward<Container<T>>(list));
    }

    const Container<FXString> &
    list() const {
        return list_;
    }

    void
    setList(Container<T> && list) {
        list_ = std::forward<Container<T>>(list);
        itr_ = std::cbegin(list_);
    }

    FXOpt<size_t>
    size() const override {
        return std::distance(std::cbegin(list_), std::cend(list_));
    }

    bool
    reset() override {
        itr_ = std::cbegin(list_);
        return true;
    }

    FXOpt<T>
    next() override {
        if (itr_ == std::cend(list_))
            return FXNone<T>;
        
        return *(itr_++);
    }
        
private:
    Container<T> list_;
    using ListItr = typename Container<T>::const_iterator;
    ListItr itr_ {};
};

template <typename T>
class FXCompositeIterator: public FXIterator<T> {
public:
    FXCompositeIterator(const FXVector<FXPtr<FXIterator<T>>> & iterators)
        : iterators_(iterators) {}

    virtual FXOpt<size_t>
    size() const {
        size_t c {};
        for (auto itr: iterators_) {
            if (auto size = itr->size())
                c += *size;
            else
                return FXNone<size_t>;
        }
        return c;
    }

    virtual bool
    reset() {
        currentIterator_ = 0;
        for (auto itr: iterators_)
            if (!itr->reset())
                return false;
        return true;
    }

    virtual FXOpt<T>
    next() {
        if (currentIterator_ >= iterators_.size())
            return FXNone<T>;
        if (auto value = iterators_[currentIterator_]->next())
            return value;
        else {
            ++ currentIterator_;
            if (currentIterator_ < iterators_.size())
                return iterators_[currentIterator_]->next();
            else
                return FXNone<T>;
        }
    }
private:
    size_t currentIterator_ {};
    FXVector<FXPtr<FXIterator<T>>> iterators_;
};
