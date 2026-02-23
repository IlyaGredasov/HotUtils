#pragma once

#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>

namespace hot_utils::memory_logger {

struct Counters {
    std::size_t allocs = 0;
    std::size_t frees = 0;
    std::size_t bytes = 0;
};

inline std::atomic<std::size_t> g_allocs{0};
inline std::atomic<std::size_t> g_frees{0};
inline std::atomic<std::size_t> g_bytes{0};
inline std::atomic<unsigned> g_guard_depth{0};

inline bool is_enabled() { return g_guard_depth.load(std::memory_order_relaxed) > 0; }

class MemoryLogger {
public:
    MemoryLogger() { g_guard_depth.fetch_add(1, std::memory_order_relaxed); }
    ~MemoryLogger() { g_guard_depth.fetch_sub(1, std::memory_order_relaxed); }

    MemoryLogger(const MemoryLogger&) = delete;
    MemoryLogger& operator=(const MemoryLogger&) = delete;
};

inline void reset() {
    g_allocs = 0;
    g_frees = 0;
    g_bytes = 0;
}

inline Counters counters() { return Counters{g_allocs.load(), g_frees.load(), g_bytes.load()}; }

inline void log_alloc(std::size_t size, std::size_t align, void* ptr) {
    if (!is_enabled()) {
        return;
    }
    std::fprintf(stderr, "[ALLOC] size=%zu align=%zu ptr=%p\n", size, align, ptr);
}

inline void log_free(void* ptr) {
    if (!is_enabled()) {
        return;
    }
    std::fprintf(stderr, "[FREE] ptr=%p\n", ptr);
}

inline void on_alloc(std::size_t size, std::size_t align, void* ptr) {
    g_allocs.fetch_add(1, std::memory_order_relaxed);
    g_bytes.fetch_add(size, std::memory_order_relaxed);
    log_alloc(size, align, ptr);
}

inline void on_free(void* ptr) {
    g_frees.fetch_add(1, std::memory_order_relaxed);
    log_free(ptr);
}

inline std::size_t round_up(std::size_t size, std::size_t alignment) {
    const std::size_t mask = alignment - 1;
    return (size + mask) & ~mask;
}

#if defined(_MSC_VER)
inline void* aligned_alloc_impl(std::size_t size, std::size_t alignment) { return _aligned_malloc(size, alignment); }
inline void aligned_free_impl(void* ptr) { _aligned_free(ptr); }
#else
inline void* aligned_alloc_impl(std::size_t size, std::size_t alignment) {
    size = round_up(size, alignment);
    return std::aligned_alloc(alignment, size);
}
inline void aligned_free_impl(void* ptr) { std::free(ptr); }
#endif

inline void* alloc(std::size_t size, std::size_t align) {
    if (align <= alignof(std::max_align_t)) {
        void* ptr = std::malloc(size);
        if (!ptr)
            return nullptr;
        on_alloc(size, align, ptr);
        return ptr;
    }
    void* ptr = aligned_alloc_impl(size, align);
    if (!ptr)
        return nullptr;
    on_alloc(size, align, ptr);
    return ptr;
}

inline void dealloc(void* ptr, std::size_t align) noexcept {
    if (!ptr)
        return;
    on_free(ptr);
    if (align <= alignof(std::max_align_t)) {
        std::free(ptr);
    } else {
        aligned_free_impl(ptr);
    }
}

} // namespace hot_utils::memory_logger

// Global operator new/delete overrides.
inline void* operator new(std::size_t size) {
    if (void* ptr = ::hot_utils::memory_logger::alloc(size, alignof(std::max_align_t))) {
        return ptr;
    }
    throw std::bad_alloc();
}

inline void operator delete(void* ptr) noexcept { ::hot_utils::memory_logger::dealloc(ptr, alignof(std::max_align_t)); }

inline void* operator new[](std::size_t size) { return ::operator new(size); }

inline void operator delete[](void* ptr) noexcept { ::operator delete(ptr); }

inline void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    return ::hot_utils::memory_logger::alloc(size, alignof(std::max_align_t));
}

inline void operator delete(void* ptr, const std::nothrow_t&) noexcept {
    ::hot_utils::memory_logger::dealloc(ptr, alignof(std::max_align_t));
}

inline void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    return ::operator new(size, std::nothrow);
}

inline void operator delete[](void* ptr, const std::nothrow_t&) noexcept { ::operator delete(ptr, std::nothrow); }

inline void operator delete(void* ptr, std::size_t) noexcept { ::operator delete(ptr); }

inline void operator delete[](void* ptr, std::size_t) noexcept { ::operator delete[](ptr); }

#if defined(__cpp_aligned_new)
inline void* operator new(std::size_t size, std::align_val_t align) {
    const auto a = static_cast<std::size_t>(align);
    if (void* ptr = ::hot_utils::memory_logger::alloc(size, a)) {
        return ptr;
    }
    throw std::bad_alloc();
}

inline void operator delete(void* ptr, std::align_val_t align) noexcept {
    ::hot_utils::memory_logger::dealloc(ptr, static_cast<std::size_t>(align));
}

inline void* operator new[](std::size_t size, std::align_val_t align) { return ::operator new(size, align); }

inline void operator delete[](void* ptr, std::align_val_t align) noexcept { ::operator delete(ptr, align); }

inline void* operator new(std::size_t size, std::align_val_t align, const std::nothrow_t&) noexcept {
    return ::hot_utils::memory_logger::alloc(size, static_cast<std::size_t>(align));
}

inline void operator delete(void* ptr, std::align_val_t align, const std::nothrow_t&) noexcept {
    ::hot_utils::memory_logger::dealloc(ptr, static_cast<std::size_t>(align));
}

inline void* operator new[](std::size_t size, std::align_val_t align, const std::nothrow_t&) noexcept {
    return ::operator new(size, align, std::nothrow);
}

inline void operator delete[](void* ptr, std::align_val_t align, const std::nothrow_t&) noexcept {
    ::operator delete(ptr, align, std::nothrow);
}

inline void operator delete(void* ptr, std::size_t, std::align_val_t align) noexcept { ::operator delete(ptr, align); }

inline void operator delete[](void* ptr, std::size_t, std::align_val_t align) noexcept {
    ::operator delete[](ptr, align);
}
#endif
