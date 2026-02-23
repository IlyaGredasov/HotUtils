#include <gtest/gtest.h>

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
    g_called = 0;
    g_last = std::chrono::microseconds{0};

    { hot_utils::ScopedTimer timer("unit", TestLogger{}); }

    EXPECT_EQ(g_called, 1);
    EXPECT_GE(g_last.count(), 0);
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
