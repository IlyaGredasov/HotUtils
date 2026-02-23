#include <gtest/gtest.h>

#include <type_traits>

#include "hot_utils/copy_move_log.hpp"

TEST(CopyLog, CountsCopyOps) {
    static_assert(!std::is_move_constructible_v<hot_utils::CopyLog>);
    static_assert(!std::is_move_assignable_v<hot_utils::CopyLog>);

    hot_utils::CopyLog::reset();
    hot_utils::CopyLog a;
    hot_utils::CopyLog b = a;
    b = a;

    auto counts = hot_utils::CopyLog::counts();
    EXPECT_EQ(counts.copy_ctor, 1u);
    EXPECT_EQ(counts.copy_assign, 1u);
}

TEST(MoveLog, CountsMoveOps) {
    static_assert(!std::is_copy_constructible_v<hot_utils::MoveLog>);
    static_assert(!std::is_copy_assignable_v<hot_utils::MoveLog>);

    hot_utils::MoveLog::reset();
    hot_utils::MoveLog a;
    hot_utils::MoveLog b = std::move(a);
    b = hot_utils::MoveLog{};

    auto counts = hot_utils::MoveLog::counts();
    EXPECT_EQ(counts.move_ctor, 1u);
    EXPECT_EQ(counts.move_assign, 1u);
}

TEST(CopyMoveLog, CountsBothOps) {
    hot_utils::CopyMoveLog::reset();
    hot_utils::CopyMoveLog a;
    hot_utils::CopyMoveLog b = a;
    b = a;
    hot_utils::CopyMoveLog c = std::move(b);
    c = hot_utils::CopyMoveLog{};

    auto counts = hot_utils::CopyMoveLog::counts();
    EXPECT_EQ(counts.copy_ctor, 1u);
    EXPECT_EQ(counts.copy_assign, 1u);
    EXPECT_EQ(counts.move_ctor, 1u);
    EXPECT_EQ(counts.move_assign, 1u);
}
