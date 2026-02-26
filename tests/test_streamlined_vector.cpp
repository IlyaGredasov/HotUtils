#include <array>
#include <sstream>
#include <type_traits>
#include <utility>

#include "gtest/gtest.h"

#include "hot_utils/copy_move_log.hpp"
#include "hot_utils/streamlined_vector.hpp"

namespace {

template <typename T, std::size_t N>
void expect_vector_eq(const hot_utils::StreamlinedVector<T, N>& actual, const std::array<T, N>& expected) {
    for (std::size_t i = 0; i < N; ++i) {
        EXPECT_EQ(actual.data[i], expected[i]);
    }
}

template <std::size_t N>
using CopyVec = hot_utils::StreamlinedVector<hot_utils::CopyLog<int>, N>;

template <std::size_t N>
using MoveVec = hot_utils::StreamlinedVector<hot_utils::MoveLog<int>, N>;

template <std::size_t N>
using CopyMoveVec = hot_utils::StreamlinedVector<hot_utils::CopyMoveLog<int>, N>;

template <std::size_t N>
void consume_by_value(CopyMoveVec<N>) {}

template <std::size_t N>
void consume_by_ref(CopyMoveVec<N>&) {}

template <std::size_t N>
void consume_by_const_ref(const CopyMoveVec<N>&) {}

template <std::size_t N>
void consume_by_rref(CopyMoveVec<N>&&) {}

struct NumberLog {
    int value = 0;
    hot_utils::CopyMoveLog<int> log{};

    NumberLog() = default;
    explicit NumberLog(int v) : value(v) {}

    NumberLog(const NumberLog&) = default;
    NumberLog(NumberLog&&) noexcept = default;
    NumberLog& operator=(const NumberLog&) = default;
    NumberLog& operator=(NumberLog&&) noexcept = default;

    NumberLog& operator+=(const NumberLog& rhs) {
        value += rhs.value;
        return *this;
    }

    NumberLog& operator-=(const NumberLog& rhs) {
        value -= rhs.value;
        return *this;
    }

    NumberLog& operator*=(const NumberLog& rhs) {
        value *= rhs.value;
        return *this;
    }

    NumberLog& operator/=(const NumberLog& rhs) {
        value /= rhs.value;
        return *this;
    }
};

} // namespace

TEST(StreamlinedVectorTypes, CopyMoveTraitsFollowElementType) {
    static_assert(std::is_copy_constructible_v<CopyVec<2>>);
    static_assert(std::is_move_constructible_v<CopyVec<2>>);

    static_assert(!std::is_copy_constructible_v<MoveVec<2>>);
    static_assert(std::is_move_constructible_v<MoveVec<2>>);

    static_assert(std::is_copy_constructible_v<CopyMoveVec<2>>);
    static_assert(std::is_move_constructible_v<CopyMoveVec<2>>);
}

TEST(StreamlinedVectorTypes, CopyOnlyElementMovesViaCopyFallback) {
    constexpr std::size_t n = 3;
    using Vec = CopyVec<n>;

    hot_utils::CopyLog<int>::reset();
    Vec src{};
    [[maybe_unused]] Vec moved = std::move(src);

    const auto counts = hot_utils::CopyLog<int>::counts();
    EXPECT_EQ(counts.copy_ctor, n);
    EXPECT_EQ(counts.copy_assign, 0u);
}

TEST(StreamlinedVectorTypes, MoveOnlyElementCanMoveWithoutCopy) {
    constexpr std::size_t n = 3;
    using Vec = MoveVec<n>;

    hot_utils::MoveLog<int>::reset();
    Vec src{};
    [[maybe_unused]] Vec moved = std::move(src);

    const auto counts = hot_utils::MoveLog<int>::counts();
    EXPECT_EQ(counts.move_ctor, n);
    EXPECT_EQ(counts.move_assign, 0u);
}

TEST(StreamlinedVectorArithmetic, ElementWiseVectorOps) {
    using Vec = hot_utils::StreamlinedVector<int, 3>;

    const Vec a{std::array<int, 3>{12, 20, 30}};
    const Vec b{std::array<int, 3>{3, 4, 5}};

    expect_vector_eq(a + b, {15, 24, 35});
    expect_vector_eq(a - b, {9, 16, 25});
    expect_vector_eq(a * b, {36, 80, 150});
    expect_vector_eq(a / b, {4, 5, 6});
}

TEST(StreamlinedVectorArithmetic, ScalarOpsBothSides) {
    using Vec = hot_utils::StreamlinedVector<int, 3>;
    const Vec a{std::array<int, 3>{12, 20, 30}};

    expect_vector_eq(a + 2, {14, 22, 32});
    expect_vector_eq(2 + a, {14, 22, 32});
    expect_vector_eq(a - 2, {10, 18, 28});
    expect_vector_eq(40 - a, {28, 20, 10});
    expect_vector_eq(a * 2, {24, 40, 60});
    expect_vector_eq(2 * a, {24, 40, 60});
    expect_vector_eq(a / 2, {6, 10, 15});
    expect_vector_eq(60 / a, {5, 3, 2});
}

TEST(StreamlinedVectorArithmetic, ReverseDivisionScalarByVector) {
    using Vec = hot_utils::StreamlinedVector<double, 3>;
    const Vec a{std::array<double, 3>{1.0, 2.0, 3.0}};

    expect_vector_eq(2.0 / a, {2.0 / 1.0, 2.0 / 2.0, 2.0 / 3.0});
}

TEST(StreamlinedVectorIteration, SupportsRangeBasedFor) {
    using Vec = hot_utils::StreamlinedVector<int, 4>;
    Vec v{std::array<int, 4>{1, 2, 3, 4}};

    int sum = 0;
    for (const auto& value : v) {
        sum += value;
    }
    EXPECT_EQ(sum, 10);

    for (auto& value : v) {
        value += 1;
    }
    expect_vector_eq(v, {2, 3, 4, 5});
}

TEST(StreamlinedVectorPrint, UsesOutputStreamIterator) {
    using Vec = hot_utils::StreamlinedVector<int, 3>;
    const Vec v{std::array<int, 3>{1, 2, 3}};

    std::ostringstream os;
    v.print(os);

    EXPECT_EQ(os.str(), "{1, 2, 3}");
}

TEST(StreamlinedVectorConstMove, MovingFromConstFallsBackToCopy) {
    constexpr std::size_t n = 4;
    using Vec = CopyMoveVec<n>;

    hot_utils::CopyMoveLog<int>::reset();
    const Vec const_vec{};
    [[maybe_unused]] Vec moved_from_const = std::move(const_vec);

    const auto counts = hot_utils::CopyMoveLog<int>::counts();
    EXPECT_EQ(counts.copy_ctor, n);
    EXPECT_EQ(counts.move_ctor, 0u);
}

TEST(StreamlinedVectorConstMove, MovingNonConstUsesMoveCtor) {
    constexpr std::size_t n = 4;
    using Vec = CopyMoveVec<n>;

    hot_utils::CopyMoveLog<int>::reset();
    Vec vec{};
    [[maybe_unused]] Vec moved = std::move(vec);

    const auto counts = hot_utils::CopyMoveLog<int>::counts();
    EXPECT_EQ(counts.copy_ctor, 0u);
    EXPECT_EQ(counts.move_ctor, n);
}

TEST(StreamlinedVectorConstMove, ReusingSourceInMultiplePlacesCopiesEachTime) {
    constexpr std::size_t n = 4;
    using Vec = CopyMoveVec<n>;

    hot_utils::CopyMoveLog<int>::reset();
    const Vec source{};
    [[maybe_unused]] Vec c1 = source;
    [[maybe_unused]] Vec c2 = source;

    const auto counts = hot_utils::CopyMoveLog<int>::counts();
    EXPECT_EQ(counts.copy_ctor, 2 * n);
    EXPECT_EQ(counts.move_ctor, 0u);
}

TEST(StreamlinedVectorConstMove, PlusWithRvalueLhsAvoidsCopiesOfLhsElements) {
    constexpr std::size_t n = 3;
    using Vec = hot_utils::StreamlinedVector<NumberLog, n>;

    hot_utils::CopyMoveLog<int>::reset();
    Vec lhs{std::array<NumberLog, n>{NumberLog{1}, NumberLog{2}, NumberLog{3}}};
    const Vec rhs{std::array<NumberLog, n>{NumberLog{4}, NumberLog{5}, NumberLog{6}}};
    [[maybe_unused]] auto out = std::move(lhs) + rhs;

    const auto counts = hot_utils::CopyMoveLog<int>::counts();
    EXPECT_EQ(counts.copy_ctor, 0u);
    EXPECT_GE(counts.move_ctor, n);
}

TEST(StreamlinedVectorFunctionParams, ByValueCopiesLvalueAndMovesRvalue) {
    constexpr std::size_t n = 5;
    using Vec = CopyMoveVec<n>;

    Vec vec{};

    hot_utils::CopyMoveLog<int>::reset();
    consume_by_value<n>(vec);
    auto counts = hot_utils::CopyMoveLog<int>::counts();
    EXPECT_EQ(counts.copy_ctor, n);
    EXPECT_EQ(counts.move_ctor, 0u);

    hot_utils::CopyMoveLog<int>::reset();
    consume_by_value<n>(std::move(vec));
    counts = hot_utils::CopyMoveLog<int>::counts();
    EXPECT_EQ(counts.copy_ctor, 0u);
    EXPECT_EQ(counts.move_ctor, n);
}

TEST(StreamlinedVectorFunctionParams, RefConstRefAndRRefDoNotCopyByThemselves) {
    constexpr std::size_t n = 5;
    using Vec = CopyMoveVec<n>;

    Vec vec{};
    const Vec const_vec{};

    hot_utils::CopyMoveLog<int>::reset();
    consume_by_ref<n>(vec);
    consume_by_const_ref<n>(const_vec);
    consume_by_rref<n>(std::move(vec));

    const auto counts = hot_utils::CopyMoveLog<int>::counts();
    EXPECT_EQ(counts.copy_ctor, 0u);
    EXPECT_EQ(counts.copy_assign, 0u);
    EXPECT_EQ(counts.move_ctor, 0u);
    EXPECT_EQ(counts.move_assign, 0u);
}
