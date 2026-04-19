#pragma once
#include <cmath>

namespace vm {

/// Lightweight 2-D vector with the most common arithmetic operations.
struct Vec2 {
    double x{0.0};
    double y{0.0};

    Vec2 operator+(const Vec2 &o) const noexcept { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2 &o) const noexcept { return {x - o.x, y - o.y}; }
    Vec2 operator*(double s)      const noexcept { return {x * s,   y * s};   }
    /// Divides both components by s.  Returns (0, 0) when s is zero.
    Vec2 operator/(double s) const noexcept {
        return s != 0.0 ? Vec2{x / s, y / s} : Vec2{0.0, 0.0};
    }

    Vec2 &operator+=(const Vec2 &o) noexcept { x += o.x; y += o.y; return *this; }
    Vec2 &operator-=(const Vec2 &o) noexcept { x -= o.x; y -= o.y; return *this; }

    double dot(const Vec2 &o)  const noexcept { return x * o.x + y * o.y; }
    double lengthSq()          const noexcept { return x * x + y * y; }
    double length()            const noexcept { return std::sqrt(lengthSq()); }

    /// Returns a unit vector, or (0, 0) when length is zero.
    Vec2 normalize() const noexcept {
        double l = length();
        return l > 0.0 ? Vec2{x / l, y / l} : Vec2{0.0, 0.0};
    }
};

inline Vec2 operator*(double s, const Vec2 &v) noexcept { return v * s; }

} // namespace vm
