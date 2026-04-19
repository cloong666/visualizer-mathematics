#pragma once

namespace vm {

/// Maps a mathematical coordinate range to a screen/canvas pixel range.
///
/// Given a "math window" [mathMin, mathMax] and a "screen window"
/// [screenMin, screenMax], any math value is linearly transformed into
/// the corresponding screen value.  The mapping is invertible.
///
/// Screen Y is typically inverted relative to math Y (origin at bottom
/// vs top-left); pass invertY = true to apply the flip automatically.
class CoordMapper {
public:
    /// @param mathMin    Left / bottom math coordinate.
    /// @param mathMax    Right / top math coordinate.  Must differ from mathMin.
    /// @param screenMin  Left / top pixel coordinate (often 0).
    /// @param screenMax  Right / bottom pixel coordinate (e.g. width − 1).
    /// @param invertY    When true the screen axis grows in the opposite
    ///                   direction to the math axis (typical for screen Y).
    CoordMapper(double mathMin, double mathMax,
                double screenMin, double screenMax,
                bool   invertY = false) noexcept
        : m_mathMin(mathMin),   m_mathMax(mathMax)
        , m_screenMin(screenMin), m_screenMax(screenMax)
        , m_invertY(invertY)
    {}

    /// Convert a math coordinate value to a screen coordinate.
    double toScreen(double mathVal) const noexcept {
        if (m_mathMax == m_mathMin) return m_screenMin;
        double t = (mathVal - m_mathMin) / (m_mathMax - m_mathMin);
        if (m_invertY) t = 1.0 - t;
        return m_screenMin + t * (m_screenMax - m_screenMin);
    }

    /// Convert a screen coordinate value back to a math coordinate.
    double toMath(double screenVal) const noexcept {
        if (m_screenMax == m_screenMin) return m_mathMin;
        double t = (screenVal - m_screenMin) / (m_screenMax - m_screenMin);
        if (m_invertY) t = 1.0 - t;
        return m_mathMin + t * (m_mathMax - m_mathMin);
    }

    // Accessors
    double mathMin()   const noexcept { return m_mathMin;   }
    double mathMax()   const noexcept { return m_mathMax;   }
    double screenMin() const noexcept { return m_screenMin; }
    double screenMax() const noexcept { return m_screenMax; }

private:
    double m_mathMin,   m_mathMax;
    double m_screenMin, m_screenMax;
    bool   m_invertY;
};

} // namespace vm
