#pragma once

#include <atomic>
#include <cstddef>
#include <cstdio>
#include <string_view>

#include "hot_utils/log_utils.hpp"

namespace hot_utils {

struct LogCounts {
    std::size_t copy_ctor = 0;
    std::size_t copy_assign = 0;
    std::size_t move_ctor = 0;
    std::size_t move_assign = 0;
};

namespace detail {
    inline void log_action(std::string_view type, std::string_view action) {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "%.*s: %.*s", static_cast<int>(type.size()), type.data(),
            static_cast<int>(action.size()), action.data());
        log_debug(buf);
    }
} // namespace detail

class CopyLog {
public:
    CopyLog() = default;
    CopyLog(const CopyLog&) {
        ++copy_ctor_;
        detail::log_action("CopyLog", "copy_ctor");
    }
    CopyLog& operator=(const CopyLog&) {
        ++copy_assign_;
        detail::log_action("CopyLog", "copy_assign");
        return *this;
    }
    CopyLog(CopyLog&&) = delete;
    CopyLog& operator=(CopyLog&&) = delete;

    static void reset() {
        copy_ctor_ = 0;
        copy_assign_ = 0;
    }

    static LogCounts counts() { return LogCounts{copy_ctor_.load(), copy_assign_.load(), 0, 0}; }

private:
    inline static std::atomic<std::size_t> copy_ctor_{0};
    inline static std::atomic<std::size_t> copy_assign_{0};
};

class MoveLog {
public:
    MoveLog() = default;
    MoveLog(MoveLog&&) noexcept {
        ++move_ctor_;
        detail::log_action("MoveLog", "move_ctor");
    }
    MoveLog& operator=(MoveLog&&) noexcept {
        ++move_assign_;
        detail::log_action("MoveLog", "move_assign");
        return *this;
    }
    MoveLog(const MoveLog&) = delete;
    MoveLog& operator=(const MoveLog&) = delete;

    static void reset() {
        move_ctor_ = 0;
        move_assign_ = 0;
    }

    static LogCounts counts() { return LogCounts{0, 0, move_ctor_.load(), move_assign_.load()}; }

private:
    inline static std::atomic<std::size_t> move_ctor_{0};
    inline static std::atomic<std::size_t> move_assign_{0};
};

class CopyMoveLog {
public:
    CopyMoveLog() = default;
    CopyMoveLog(const CopyMoveLog&) {
        ++copy_ctor_;
        detail::log_action("CopyMoveLog", "copy_ctor");
    }
    CopyMoveLog& operator=(const CopyMoveLog&) {
        ++copy_assign_;
        detail::log_action("CopyMoveLog", "copy_assign");
        return *this;
    }
    CopyMoveLog(CopyMoveLog&&) noexcept {
        ++move_ctor_;
        detail::log_action("CopyMoveLog", "move_ctor");
    }
    CopyMoveLog& operator=(CopyMoveLog&&) noexcept {
        ++move_assign_;
        detail::log_action("CopyMoveLog", "move_assign");
        return *this;
    }

    static void reset() {
        copy_ctor_ = 0;
        copy_assign_ = 0;
        move_ctor_ = 0;
        move_assign_ = 0;
    }

    static LogCounts counts() {
        return LogCounts{copy_ctor_.load(), copy_assign_.load(), move_ctor_.load(), move_assign_.load()};
    }

private:
    inline static std::atomic<std::size_t> copy_ctor_{0};
    inline static std::atomic<std::size_t> copy_assign_{0};
    inline static std::atomic<std::size_t> move_ctor_{0};
    inline static std::atomic<std::size_t> move_assign_{0};
};

} // namespace hot_utils
