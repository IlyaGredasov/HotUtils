#include <gtest/gtest.h>
#include <vector>

#include "hot_utils/memory_logger.hpp"
#include "hot_utils/scoped_timer.hpp"

TEST(Playground, PlaygroundTest) {
    hot_utils::ScopedTimer t{"MyTimer"};
    for (int i = 0; i < 10e7; ++i) {}
    hot_utils::memory_logger::MemoryLogger l;
    std::vector<int> a(10);
    EXPECT_TRUE(true);
}