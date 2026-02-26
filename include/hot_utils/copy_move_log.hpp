#pragma once

#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>

#if defined(__GNUG__)
#include <cxxabi.h>
#endif

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

    inline void strip_hot_utils_namespace(std::string& type) {
        static constexpr std::string_view prefix = "hot_utils::";
        std::size_t pos = 0;
        while ((pos = type.find(prefix, pos)) != std::string::npos) {
            type.erase(pos, prefix.size());
        }
    }

    template <typename T>
    inline const std::string& type_name() {
        static const std::string name = [] {
#if defined(__GNUG__)
            int status = 0;
            const char* const raw = typeid(T).name();
            char* const demangled = abi::__cxa_demangle(raw, nullptr, nullptr, &status);
            std::string out = (status == 0 && demangled != nullptr) ? demangled : raw;
            std::free(demangled);
            strip_hot_utils_namespace(out);
            return out;
#else
            std::string out = typeid(T).name();
            strip_hot_utils_namespace(out);
            return out;
#endif
        }();
        return name;
    }

    template <typename T>
    inline void log_action_for(std::string_view wrapper, std::string_view action) {
        const auto& type = type_name<T>();
        char buf[256];
        std::snprintf(buf, sizeof(buf), "%.*s<%.*s>: %.*s", static_cast<int>(wrapper.size()), wrapper.data(),
            static_cast<int>(type.size()), type.data(), static_cast<int>(action.size()), action.data());
        log_debug(buf);
    }
} // namespace detail

template <typename T = int>
class CopyLog {
public:
    CopyLog() = default;
    explicit CopyLog(const T& value)
        : value_(value) {}
    explicit CopyLog(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(value)) {}

    CopyLog(const CopyLog& other)
        : value_(other.value_) {
        ++copy_ctor_;
        detail::log_action_for<T>("CopyLog", "copy_ctor");
    }
    CopyLog& operator=(const CopyLog& other) {
        value_ = other.value_;
        ++copy_assign_;
        detail::log_action_for<T>("CopyLog", "copy_assign");
        return *this;
    }
    CopyLog(CopyLog&&) = delete;
    CopyLog& operator=(CopyLog&&) = delete;

    static void reset() {
        copy_ctor_ = 0;
        copy_assign_ = 0;
    }

    static LogCounts counts() { return LogCounts{copy_ctor_.load(), copy_assign_.load(), 0, 0}; }

    T& value() & { return value_; }
    const T& value() const & { return value_; }
    T&& value() && { return std::move(value_); }

private:
    T value_{};
    inline static std::atomic<std::size_t> copy_ctor_{0};
    inline static std::atomic<std::size_t> copy_assign_{0};
};

template <typename T = int>
class MoveLog {
public:
    MoveLog() = default;
    explicit MoveLog(const T& value)
        : value_(value) {}
    explicit MoveLog(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(value)) {}

    MoveLog(MoveLog&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(other.value_)) {
        ++move_ctor_;
        detail::log_action_for<T>("MoveLog", "move_ctor");
    }
    MoveLog& operator=(MoveLog&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
        value_ = std::move(other.value_);
        ++move_assign_;
        detail::log_action_for<T>("MoveLog", "move_assign");
        return *this;
    }
    MoveLog(const MoveLog&) = delete;
    MoveLog& operator=(const MoveLog&) = delete;

    static void reset() {
        move_ctor_ = 0;
        move_assign_ = 0;
    }

    static LogCounts counts() { return LogCounts{0, 0, move_ctor_.load(), move_assign_.load()}; }

    T& value() & { return value_; }
    const T& value() const & { return value_; }
    T&& value() && { return std::move(value_); }

private:
    T value_{};
    inline static std::atomic<std::size_t> move_ctor_{0};
    inline static std::atomic<std::size_t> move_assign_{0};
};

template <typename T = int>
class CopyMoveLog {
public:
    CopyMoveLog() = default;
    explicit CopyMoveLog(const T& value)
        : value_(value) {}
    explicit CopyMoveLog(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(value)) {}

    CopyMoveLog(const CopyMoveLog& other)
        : value_(other.value_) {
        ++copy_ctor_;
        detail::log_action_for<T>("CopyMoveLog", "copy_ctor");
    }
    CopyMoveLog& operator=(const CopyMoveLog& other) {
        value_ = other.value_;
        ++copy_assign_;
        detail::log_action_for<T>("CopyMoveLog", "copy_assign");
        return *this;
    }
    CopyMoveLog(CopyMoveLog&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(other.value_)) {
        ++move_ctor_;
        detail::log_action_for<T>("CopyMoveLog", "move_ctor");
    }
    CopyMoveLog& operator=(CopyMoveLog&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
        value_ = std::move(other.value_);
        ++move_assign_;
        detail::log_action_for<T>("CopyMoveLog", "move_assign");
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

    T& value() & { return value_; }
    const T& value() const & { return value_; }
    T&& value() && { return std::move(value_); }

private:
    T value_{};
    inline static std::atomic<std::size_t> copy_ctor_{0};
    inline static std::atomic<std::size_t> copy_assign_{0};
    inline static std::atomic<std::size_t> move_ctor_{0};
    inline static std::atomic<std::size_t> move_assign_{0};
};

} // namespace hot_utils
