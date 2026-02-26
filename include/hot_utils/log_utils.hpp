#pragma once

#include <cstddef>
#include <cstdio>
#include <string_view>
#include <utility>

namespace hot_utils {

#ifdef NDEBUG
inline constexpr bool kDebugEnabled = false;
#else
inline constexpr bool kDebugEnabled = true;
#endif

namespace detail {
    inline void log_line(const char* level, std::string_view msg) {
        std::fprintf(stderr, "[%s] %.*s\n", level, static_cast<int>(msg.size()), msg.data());
    }

    inline thread_local std::size_t call_depth = 0;

    inline void log_call_impl(const char* file, int line, const char* func, const char* expr, std::size_t depth) {
        std::fprintf(stderr, "[CALL] %*s%s:%d %s -> %s\n", static_cast<int>(depth * 2), "", file, line, func, expr);
    }

    struct CallDepthGuard {
        explicit CallDepthGuard(bool enabled)
            : enabled_(enabled)
            , depth_(call_depth) {
            if (enabled_) {
                ++call_depth;
            }
        }

        ~CallDepthGuard() {
            if (enabled_) {
                --call_depth;
            }
        }

        std::size_t depth() const { return depth_; }

    private:
        bool enabled_ = false;
        std::size_t depth_ = 0;
    };

    template <bool Enabled, typename F>
    decltype(auto) log_call_expr(const char* file, int line, const char* func, const char* expr, F&& f) {
        CallDepthGuard guard(Enabled);
        if constexpr (Enabled) {
            log_call_impl(file, line, func, expr, guard.depth());
        }
        return std::forward<F>(f)();
    }
} // namespace detail

// Compile-time controllable logging.
template <bool Enabled>
inline void log_call(const char* file, int line, const char* func) {
    if constexpr (Enabled) {
        detail::log_call_impl(file, line, func, "<manual>", detail::call_depth);
    }
}

// Convenience macros for capturing call site without changing expression semantics.
#define HOT_UTILS_LOG_CALL(expr)                                                                                       \
    ::hot_utils::detail::log_call_expr<::hot_utils::kDebugEnabled>(__FILE__, __LINE__, __func__, #expr, [&]()       \
                                                                    -> decltype(auto) { return (expr); })

#define HOT_UTILS_LOG_CALL_IF(Enabled, expr)                                                                          \
    ::hot_utils::detail::log_call_expr<Enabled>(__FILE__, __LINE__, __func__, #expr, [&]() -> decltype(auto) {      \
        return (expr);                                                                                                \
    })

// Debug logging toggled by NDEBUG.
inline void log_debug(std::string_view msg) {
    if constexpr (kDebugEnabled) {
        detail::log_line("DEBUG", msg);
    }
}

} // namespace hot_utils
