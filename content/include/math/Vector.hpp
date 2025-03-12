#pragma once
#include <concepts>
#include "math/algorithm.hpp"
#include <core/langdef.hpp>
#include <type_traits>

namespace atom::engine::math {

template <typename Ty>
concept arithmetic = std::is_arithmetic_v<Ty>;

// NOTE: This would be done at the future if I'am alive.
// We will try simd as possible, especially for Vector4.

namespace v2 {

template <arithmetic Ty = float>
struct Vector2 {
    Ty x;
    Ty y;

    constexpr Vector2() noexcept : x(), y() {}
    constexpr Vector2(Ty scaling) noexcept : x(scaling), y(scaling) {}
    constexpr Vector2(Ty x, Ty y) noexcept : x(x), y(y) {}

    constexpr Vector2& operator*=(Ty val) noexcept {
        x *= val;
        y *= val;
        return *this;
    }

    constexpr Vector2 operator*(Ty val) const noexcept {
        return Vector2(x * val, y * val);
    }

    constexpr Vector2& operator/=(Ty val) noexcept {
        x /= val;
        y /= val;
        return *this;
    }

    constexpr Vector2 operator/(Ty val) const noexcept {
        return Vector2(x / val, y / val);
    }

    constexpr Vector2& operator+=(const Vector2& that) noexcept {
        x += that.x;
        y += that.y;
        return *this;
    }

    constexpr Vector2 operator+(const Vector2& that) noexcept {
        return Vector2(x + that.x, y + that.y);
    }

    constexpr Vector2& operator-=(const Vector2& that) noexcept {
        x -= that.x;
        y -= that.y;
        return *this;
    }

    constexpr Vector2 operator-(const Vector2& that) noexcept {
        return Vector2(x - that.x, y - that.y);
    }

    constexpr Ty length() const noexcept {
        return sqrt(x * x + y * y);
    }

    constexpr Vector2 normalize() noexcept {
        const auto inv = inv_sqrt(x * x + y * y);
        return Vector2(x * inv, y * inv);
    }
};

template <arithmetic Ty = float>
struct Vector3 : public Vector2<Ty> {
    Ty z;

    constexpr Vector3() noexcept : Vector2<Ty>(), z() {}
    constexpr Vector3(Ty scaling) noexcept : Vector2<Ty>(scaling) {}
    constexpr Vector3(Ty x, Ty y, Ty z) noexcept : Vector2<Ty>(x, y), z(z) {}

    constexpr Ty length() const noexcept {

    }

    constexpr Vector3 normalize() const noexcept {
        const auto inv = inv_sqrt(Vector2<>::x * Vector2<>::x + Vector2<>::y * Vector2<>::y );
        return Vector3(Vector2<>::x * inv, Vector2<>::y * inv, z * inv);
    }

};

template <arithmetic Ty = float>
struct Vector4 : public Vector3<Ty> {
    Ty w;

    constexpr Vector4() noexcept : Vector3<Ty>(), w() {}
    constexpr Vector4(Ty scaling) noexcept : Vector3<Ty>(scaling) {}
    constexpr Vector4(Ty x, Ty y, Ty z, Ty w) noexcept : Vector3<Ty>(x, y, z), w(w) {}

    constexpr Ty base() const noexcept {
        return inv_sqrt();
    }

    constexpr void normalize() noexcept {
        const auto base = Vector4::base();
        Vector3<Ty>::x *= base;
        Vector3<Ty>::y *= base;
        Vector3<Ty>::z *= base;
        w *= base;
    }
};

template <arithmetic Ty = float>
struct alignas(num_sixteen) AlignedVector2 : public Vector2<Ty> {
    constexpr AlignedVector2() noexcept : Vector2<Ty>() {}
    constexpr AlignedVector2(Ty scaling) noexcept : Vector2<Ty>(scaling) {}
    constexpr AlignedVector2(Ty x, Ty y) noexcept : Vector2<Ty>(x, y) {}
};

template <arithmetic Ty = float>
struct alignas(num_thirty_two) AlignedVector3 : public Vector3<Ty> {
    constexpr AlignedVector3() noexcept : Vector3<Ty>() {}
    constexpr AlignedVector3(Ty scaling) noexcept : Vector3<Ty>(scaling) {}
    constexpr AlignedVector3(Ty x, Ty y, Ty z) noexcept : Vector3<Ty>(x, y, z) {}
};

template <arithmetic Ty = float>
struct alignas(num_thirty_two) AlignedVector4 : public Vector4<Ty> {
    constexpr AlignedVector4() noexcept : Vector4<Ty>() {}
    constexpr AlignedVector4(Ty scaling) noexcept : Vector4<Ty>(scaling) {}
    constexpr AlignedVector4(Ty x, Ty y, Ty z, Ty w) noexcept : Vector4<Ty>(x, y, z, w) {}
};

} // namespace v2

} // namespace atom::engine::math
