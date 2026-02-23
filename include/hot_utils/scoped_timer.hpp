#pragma once

#include <chrono>
#include <cstdio>
#include <string_view>
#include <utility>

#include "hot_utils/log_utils.hpp"

namespace hot_utils {

struct DefaultTimerLogger {
    void operator()(std::string_view label, std::chrono::milliseconds us) const {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "TIMER %.*s: %lld ms", static_cast<int>(label.size()), label.data(),
            static_cast<long long>(us.count()));
        detail::log_line("TIME", buf);
    }
};

template <class Duration = std::chrono::milliseconds, class Clock = std::chrono::high_resolution_clock,
    class Logger = DefaultTimerLogger>
class ScopedTimer {
public:
    explicit ScopedTimer(std::string_view label = "", Logger logger = Logger{})
        : label_(label), logger_(std::move(logger)), start_(Clock::now()) {}

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

    ~ScopedTimer() { log(); }

private:
    void log() const {
        const auto end = Clock::now();
        const auto elapsed = std::chrono::duration_cast<Duration>(end - start_);
        logger_(label_, elapsed);
    }

    std::string_view label_;
    Logger logger_;
    typename Clock::time_point start_;
};

} // namespace hot_utils
