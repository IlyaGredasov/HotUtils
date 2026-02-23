#include <gtest/gtest.h>

#include "hot_utils/log_utils.hpp"

TEST(LogUtils, LogCallCompiles) {
    HOT_UTILS_LOG_CALL(true);
    HOT_UTILS_LOG_CALL(false);
    EXPECT_TRUE(true);
}

TEST(LogUtils, LogDebugCompiles) {
    hot_utils::log_debug("hello");
    EXPECT_TRUE(true);
}
