#include <gtest/gtest.h>

#include "hot_utils/memory_logger.hpp"

TEST(MemoryLogger, CountsAllocations) {
    hot_utils::memory_logger::reset();
    auto before = hot_utils::memory_logger::counters();

    {
        hot_utils::memory_logger::MemoryLogger guard;
        int* value = new int(7);
        auto mid = hot_utils::memory_logger::counters();
        delete value;

        EXPECT_GE(mid.allocs, before.allocs + 1);
    }

    auto after = hot_utils::memory_logger::counters();
    EXPECT_GE(after.frees, before.frees + 1);
}
