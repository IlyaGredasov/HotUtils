#include "gtest/gtest.h"

#include "hot_utils/do_not_optimize.hpp"

TEST(DoNotOptimize, CompilesAndAcceptsValues) {
    int value = 123;
    hot_utils::do_not_optimize(value);
    hot_utils::do_not_optimize(456);
    EXPECT_EQ(value, 123);
}
