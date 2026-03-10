#include "MathFunctions.hpp"

static const std::vector<MathCurve> s_curves = {
    {
        "sin(x)",
        "y = sin(x)",
        [](double x) { return std::sin(x); }
    },
    {
        "cos(x)",
        "y = cos(x)",
        [](double x) { return std::cos(x); }
    },
    {
        "tan(x)",
        "y = tan(x)",
        [](double x) {
            double v = std::tan(x);
            // Treat near-vertical asymptotes as discontinuities
            if (std::abs(v) > 20.0) return std::numeric_limits<double>::quiet_NaN();
            return v;
        }
    },
    {
        "x\u00b2",        // x²
        "y = x\u00b2",
        [](double x) { return x * x; }
    },
    {
        "x\u00b3 \u2212 3x", // x³ − 3x
        "y = x\u00b3 \u2212 3x",
        [](double x) { return x * x * x - 3.0 * x; }
    },
    {
        "sinc(x)",
        "y = sin(x) / x",
        [](double x) {
            return (x == 0.0) ? 1.0 : std::sin(x) / x;
        }
    },
    {
        "damped sin",
        "y = e^(-x\u00b2/8) \u00b7 sin(3x)",
        [](double x) {
            return std::exp(-x * x / 8.0) * std::sin(3.0 * x);
        }
    },
    {
        "|sin(x)|",
        "y = |sin(x)|",
        [](double x) { return std::abs(std::sin(x)); }
    },
    {
        "ln(x)",
        "y = ln(x)",
        [](double x) {
            if (x <= 0.0) return std::numeric_limits<double>::quiet_NaN();
            return std::log(x);
        }
    },
    {
        "sqrt(|x|)",
        "y = \u221a|x|",
        [](double x) { return std::sqrt(std::abs(x)); }
    },
};

const std::vector<MathCurve>& getCurves() {
    return s_curves;
}
