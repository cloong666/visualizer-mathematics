#pragma once

#include <functional>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

/// Represents a single plottable math curve y = amplitude * f(frequency * x)
struct MathCurve {
    std::string name;    ///< Short display name  e.g. "sin(x)"
    std::string formula; ///< Full formula string e.g. "y = sin(x)"
    std::function<double(double)> fn; ///< Pure function of x
};

/// Returns the catalogue of all built-in curves.
const std::vector<MathCurve>& getCurves();
