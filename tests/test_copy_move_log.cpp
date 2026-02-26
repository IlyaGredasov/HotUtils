#include "gtest/gtest.h"

#include <type_traits>

#include "hot_utils/copy_move_log.hpp"
#include "hot_utils/streamlined_vector.hpp"

TEST(CopyLog, CountsCopyOps) {
    using Log = hot_utils::CopyLog<int>;
    static_assert(!std::is_move_constructible_v<Log>);
    static_assert(!std::is_move_assignable_v<Log>);

    Log::reset();
    Log a;
    Log b = a;
    b = a;

    auto counts = Log::counts();
    EXPECT_EQ(counts.copy_ctor, 1u);
    EXPECT_EQ(counts.copy_assign, 1u);
}

TEST(MoveLog, CountsMoveOps) {
    using Log = hot_utils::MoveLog<int>;
    static_assert(!std::is_copy_constructible_v<Log>);
    static_assert(!std::is_copy_assignable_v<Log>);

    Log::reset();
    Log a;
    Log b = std::move(a);
    b = Log{};

    auto counts = Log::counts();
    EXPECT_EQ(counts.move_ctor, 1u);
    EXPECT_EQ(counts.move_assign, 1u);
}

TEST(CopyMoveLog, CountsBothOps) {
    using Log = hot_utils::CopyMoveLog<int>;
    Log::reset();
    Log a;
    Log b = a;
    b = a;
    Log c = std::move(b);
    c = Log{};

    auto counts = Log::counts();
    EXPECT_EQ(counts.copy_ctor, 1u);
    EXPECT_EQ(counts.copy_assign, 1u);
    EXPECT_EQ(counts.move_ctor, 1u);
    EXPECT_EQ(counts.move_assign, 1u);
}

TEST(CopyMoveLog, SupportsNestedLoggedTypes) {
    using Inner = hot_utils::CopyMoveLog<int>;
    using Vec = hot_utils::StreamlinedVector<Inner, 2>;
    using Outer = hot_utils::CopyMoveLog<Vec>;

    Inner::reset();
    Outer::reset();

    Outer a;
    Outer b = a;
    Outer c = std::move(b);

    const auto outer_counts = Outer::counts();
    EXPECT_EQ(outer_counts.copy_ctor, 1u);
    EXPECT_EQ(outer_counts.move_ctor, 1u);

    const auto inner_counts = Inner::counts();
    EXPECT_EQ(inner_counts.copy_ctor, 2u);
    EXPECT_EQ(inner_counts.move_ctor, 2u);
}
