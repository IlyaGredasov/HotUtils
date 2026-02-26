#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>

namespace hot_utils {

template <typename S>
using EnableIfArithmetic = std::enable_if_t<std::is_arithmetic_v<std::decay_t<S>>, int>;

template <typename T, std::size_t N>
struct StreamlinedVector final {
    using value_type = T;
    static constexpr std::size_t size_v = N;

    std::array<T, N> data{};

    constexpr std::size_t size() const noexcept { return N; }

    constexpr auto begin() noexcept { return data.begin(); }
    constexpr auto end() noexcept { return data.end(); }
    constexpr auto begin() const noexcept { return data.begin(); }
    constexpr auto end() const noexcept { return data.end(); }
    constexpr auto cbegin() const noexcept { return data.cbegin(); }
    constexpr auto cend() const noexcept { return data.cend(); }

    constexpr T& operator[](std::size_t index) noexcept { return data[index]; }
    constexpr const T& operator[](std::size_t index) const noexcept { return data[index]; }

    constexpr StreamlinedVector& operator+=(const StreamlinedVector& rhs) {
        std::transform(data.begin(), data.end(), rhs.data.begin(), data.begin(), [](T& lhs, const T& rhs_value) {
            lhs += rhs_value;
            return std::move(lhs);
        });
        return *this;
    }

    constexpr StreamlinedVector& operator-=(const StreamlinedVector& rhs) {
        std::transform(data.begin(), data.end(), rhs.data.begin(), data.begin(), [](T& lhs, const T& rhs_value) {
            lhs -= rhs_value;
            return std::move(lhs);
        });
        return *this;
    }

    constexpr StreamlinedVector& operator*=(const StreamlinedVector& rhs) {
        std::transform(data.begin(), data.end(), rhs.data.begin(), data.begin(), [](T& lhs, const T& rhs_value) {
            lhs *= rhs_value;
            return std::move(lhs);
        });
        return *this;
    }

    constexpr StreamlinedVector& operator/=(const StreamlinedVector& rhs) {
        std::transform(data.begin(), data.end(), rhs.data.begin(), data.begin(), [](T& lhs, const T& rhs_value) {
            lhs /= rhs_value;
            return std::move(lhs);
        });
        return *this;
    }

    template <typename S, EnableIfArithmetic<S> = 0>
    constexpr StreamlinedVector& operator+=(S scalar) {
        std::transform(data.begin(), data.end(), data.begin(), [scalar](T& value) {
            value += scalar;
            return std::move(value);
        });
        return *this;
    }

    template <typename S, EnableIfArithmetic<S> = 0>
    constexpr StreamlinedVector& operator-=(S scalar) {
        std::transform(data.begin(), data.end(), data.begin(), [scalar](T& value) {
            value -= scalar;
            return std::move(value);
        });
        return *this;
    }

    template <typename S, EnableIfArithmetic<S> = 0>
    constexpr StreamlinedVector& operator*=(S scalar) {
        std::transform(data.begin(), data.end(), data.begin(), [scalar](T& value) {
            value *= scalar;
            return std::move(value);
        });
        return *this;
    }

    template <typename S, EnableIfArithmetic<S> = 0>
    constexpr StreamlinedVector& operator/=(S scalar) {
        std::transform(data.begin(), data.end(), data.begin(), [scalar](T& value) {
            value /= scalar;
            return std::move(value);
        });
        return *this;
    }

    constexpr bool operator==(const StreamlinedVector& rhs) const { return data == rhs.data; }
    constexpr bool operator!=(const StreamlinedVector& rhs) const { return !(*this == rhs); }

    void print(std::ostream& os) const {
        os << "{";
        if constexpr (N > 0) {
            std::copy(data.begin(), data.end() - 1, std::ostream_iterator<T>(os, ", "));
            os << data.back();
        }
        os << "}";
    }
};

template <typename T, std::size_t N>
constexpr StreamlinedVector<T, N> operator+(StreamlinedVector<T, N> lhs, const StreamlinedVector<T, N>& rhs) {
    lhs += rhs;
    return lhs;
}

template <typename T, std::size_t N>
constexpr StreamlinedVector<T, N> operator-(StreamlinedVector<T, N> lhs, const StreamlinedVector<T, N>& rhs) {
    lhs -= rhs;
    return lhs;
}

template <typename T, std::size_t N>
constexpr StreamlinedVector<T, N> operator*(StreamlinedVector<T, N> lhs, const StreamlinedVector<T, N>& rhs) {
    lhs *= rhs;
    return lhs;
}

template <typename T, std::size_t N>
constexpr StreamlinedVector<T, N> operator/(StreamlinedVector<T, N> lhs, const StreamlinedVector<T, N>& rhs) {
    lhs /= rhs;
    return lhs;
}

template <typename T, std::size_t N, typename S, EnableIfArithmetic<S> = 0>
constexpr StreamlinedVector<T, N> operator+(StreamlinedVector<T, N> lhs, S scalar) {
    lhs += scalar;
    return lhs;
}

template <typename T, std::size_t N, typename S, EnableIfArithmetic<S> = 0>
constexpr StreamlinedVector<T, N> operator+(S scalar, StreamlinedVector<T, N> rhs) {
    rhs += scalar;
    return rhs;
}

template <typename T, std::size_t N, typename S, EnableIfArithmetic<S> = 0>
constexpr StreamlinedVector<T, N> operator-(StreamlinedVector<T, N> lhs, S scalar) {
    lhs -= scalar;
    return lhs;
}

template <typename T, std::size_t N, typename S, EnableIfArithmetic<S> = 0>
constexpr StreamlinedVector<T, N> operator-(S scalar, StreamlinedVector<T, N> rhs) {
    std::transform(rhs.data.begin(), rhs.data.end(), rhs.data.begin(), [scalar](T& value) { return scalar - value; });
    return rhs;
}

template <typename T, std::size_t N, typename S, EnableIfArithmetic<S> = 0>
constexpr StreamlinedVector<T, N> operator*(StreamlinedVector<T, N> lhs, S scalar) {
    lhs *= scalar;
    return lhs;
}

template <typename T, std::size_t N, typename S, EnableIfArithmetic<S> = 0>
constexpr StreamlinedVector<T, N> operator*(S scalar, StreamlinedVector<T, N> rhs) {
    rhs *= scalar;
    return rhs;
}

template <typename T, std::size_t N, typename S, EnableIfArithmetic<S> = 0>
constexpr StreamlinedVector<T, N> operator/(StreamlinedVector<T, N> lhs, S scalar) {
    lhs /= scalar;
    return lhs;
}

template <typename T, std::size_t N, typename S, EnableIfArithmetic<S> = 0>
constexpr StreamlinedVector<T, N> operator/(S scalar, StreamlinedVector<T, N> rhs) {
    std::transform(rhs.data.begin(), rhs.data.end(), rhs.data.begin(), [scalar](T& value) { return scalar / value; });
    return rhs;
}

} // namespace hot_utils
