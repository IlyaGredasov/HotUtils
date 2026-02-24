#include "gtest/gtest.h"

#include "hot_utils/scoped_timer.hpp"

namespace {
int g_called = 0;
std::chrono::microseconds g_last{0};

struct TestLogger {
    void operator()(std::string_view, std::chrono::microseconds us) const {
        ++g_called;
        g_last = us;
    }
};
} // namespace

TEST(ScopedTimer, LogsOnDestructionDefaultTypes) {
    hot_utils::ScopedTimer timer("unit");
    for (int i = 0; i < 10e7; ++i) {}
    EXPECT_TRUE(true);
}

TEST(ScopedTimer, SupportsCustomTypes) {
    g_called = 0;

    struct NanoLogger {
        void operator()(std::string_view, std::chrono::nanoseconds) const { ++g_called; }
    };

    {
        hot_utils::ScopedTimer<std::chrono::nanoseconds, std::chrono::steady_clock, NanoLogger> timer("unit",
            NanoLogger{});
    }

    EXPECT_EQ(g_called, 1);
}
