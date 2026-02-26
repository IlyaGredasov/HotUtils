#include "gtest/gtest.h"

#include "hot_utils/log_utils.hpp"

TEST(LogUtils, LogCallCompiles) {
    int x = 0;
    HOT_UTILS_LOG_CALL(++x);
    HOT_UTILS_LOG_CALL_IF(true, ++x);
    HOT_UTILS_LOG_CALL_IF(false, ++x);
    EXPECT_EQ(x, 3);
}

TEST(LogUtils, LogFunctionCompiles) {
    auto f = [](int i) { return i; };
    const int v = HOT_UTILS_LOG_CALL(f(42));
    EXPECT_EQ(v, 42);
}

TEST(LogUtils, LogCallPreservesReferenceSemantics) {
    int x = 10;
    auto& ref = HOT_UTILS_LOG_CALL(x);
    ref += 5;
    EXPECT_EQ(x, 15);
}

TEST(LogUtils, LogDebugCompiles) {
    hot_utils::log_debug("hello");
    EXPECT_TRUE(true);
}
