#pragma once

#include <atomic>
#include <cstddef>
#include <type_traits>

namespace hot_utils {

namespace detail {
#if defined(_MSC_VER)
#pragma intrinsic(_ReadWriteBarrier)
    inline void compiler_barrier() { _ReadWriteBarrier(); }
#elif defined(__GNUC__) || defined(__clang__)
    inline void compiler_barrier() { __asm__ __volatile__("" ::: "memory"); }
#else
    inline void compiler_barrier() { std::atomic_signal_fence(std::memory_order_seq_cst); }
#endif
} // namespace detail

template <class T>
inline void do_not_optimize(const T& value) {
    detail::compiler_barrier();
    auto* volatile ptr = &value;
    (void)ptr;
    detail::compiler_barrier();
}

template <class T>
inline void do_not_optimize(T&& value) {
    detail::compiler_barrier();
    auto* volatile ptr = &value;
    (void)ptr;
    detail::compiler_barrier();
}

} // namespace hot_utils
