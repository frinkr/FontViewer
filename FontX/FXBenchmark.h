#pragma once
#include <chrono>
#include "FX.h"

class FXBenchmark {
public:
    using Seconds = std::chrono::duration<double>;
    using TimePoint = typename std::chrono::high_resolution_clock::time_point;

public:
    explicit FXBenchmark(bool autoStart = true) {
        if (autoStart)
            start();
    }

    ~FXBenchmark() {}
    
    TimePoint 
    start() {
        start_ = now();
        stop_ = FXNone<TimePoint>;
        return *start_;
    }

    TimePoint
    stop() {
        assert(start_);
        stop_ = now();
        return *stop_;
    }
    
    Seconds
    time() const {
        if (!start_)
            return Seconds{};
        if (stop_)
            return std::chrono::duration_cast<Seconds>(*stop_ - *start_);
        else
            return std::chrono::duration_cast<Seconds>(now() - *start_);
    }

private:
    TimePoint now() const {
        return std::chrono::high_resolution_clock::now();
    }
private:
    FXOpt<TimePoint>  start_{};
    FXOpt<TimePoint>  stop_ {};
};
