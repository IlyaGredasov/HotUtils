#pragma once

#include <cstddef>
#include <cstdio>
#include <string_view>

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

    inline void log_call_impl(const char* file, int line, const char* func) {
        std::fprintf(stderr, "[CALL] %s:%d %s\n", file, line, func);
    }
} // namespace detail

// Compile-time controllable logging.
template <bool Enabled>
inline void log_call(const char* file, int line, const char* func) {
    if constexpr (Enabled) {
        detail::log_call_impl(file, line, func);
    }
}

// Convenience macro for capturing call site.
#define HOT_UTILS_LOG_CALL(Enabled) ::hot_utils::log_call<Enabled>(__FILE__, __LINE__, __func__)

// Debug logging toggled by NDEBUG.
inline void log_debug(std::string_view msg) {
    if constexpr (kDebugEnabled) {
        detail::log_line("DEBUG", msg);
    }
}

} // namespace hot_utils
