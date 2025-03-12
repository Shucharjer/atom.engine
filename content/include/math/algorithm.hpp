#pragma once
#include <cstdint>
#include <core/langdef.hpp>

namespace atom::engine::math {

template <typename Ty>
ATOM_FORCE_INLINE constexpr Ty abs(const Ty x) noexcept(noexcept(-x)) {
    if (x < 0) {
        return -x;       
    }
    else {
        return x;
    }
}

template <>
ATOM_FORCE_INLINE constexpr int8_t abs(const int8_t x)  noexcept {
    constexpr uint32_t magic = 31;
    auto y = (int8_t)(x >> magic);
    return (int8_t)((x + y) ^ y);
}

template <>
ATOM_FORCE_INLINE constexpr int16_t abs(const int16_t x)  noexcept {
    constexpr uint32_t magic = 31;
    auto y = (int16_t)(x >> magic);
    return (int16_t)((x + y) ^ y);
}

template <>
ATOM_FORCE_INLINE constexpr int32_t abs(const int32_t x)  noexcept {
    constexpr uint32_t magic = 31;
    int32_t y = x >> magic;
    return (x + y) ^ y;
}

template <>
ATOM_FORCE_INLINE constexpr int64_t abs(const int64_t x ) noexcept {
    constexpr uint32_t magic = 63;
    int64_t y = x >> magic;
    return (x + y) ^ y;
}

template <>
ATOM_FORCE_INLINE float abs(const float x) noexcept {
    auto* ix = (int32_t*)&x;
    auto res = abs(*ix);
    return *(float*)(&res);
}

template <>
ATOM_FORCE_INLINE double abs(const double x) noexcept {
    auto* ix = (int64_t*)&x;
    auto res = abs(*ix);
    return *(double*)(&res);
}

template <typename Ty>
ATOM_FORCE_INLINE constexpr Ty max(const Ty lhs, const Ty rhs) noexcept(noexcept(lhs < rhs)) {
    return lhs < rhs ? lhs : rhs;
} 

template <>
ATOM_FORCE_INLINE constexpr int32_t max(const int32_t lhs, const int32_t rhs) noexcept {
    constexpr uint32_t magic = 31;
    int32_t minus1 = lhs - rhs;
    int32_t minus2 = lhs - rhs;
    return rhs & (minus1 >> magic) | lhs & ~(minus2 >> magic);
}

template <>
ATOM_FORCE_INLINE constexpr int64_t max(const int64_t lhs, const int64_t rhs) noexcept {
    constexpr uint32_t magic = 63;
    int64_t minus1 = lhs - rhs;
    int64_t minus2 = lhs - rhs;
    return rhs & (minus1 >> magic) | lhs & ~(minus2 >> magic);
}

template <typename Ty>
ATOM_FORCE_INLINE constexpr Ty averange(const Ty lhs, const Ty rhs) noexcept {
    return lhs / 2 + rhs / 2;
}

template <typename Ty>
ATOM_FORCE_INLINE constexpr Ty sqrt(const Ty x) noexcept {

}

template <typename Ty>
ATOM_FORCE_INLINE constexpr Ty inv_sqrt(const Ty x) noexcept {

}

}