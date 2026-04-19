#pragma once
#include <string>
#include <vector>

namespace vm {

/// A 2D point produced by sampling a mathematical function.
struct Point2D {
    double x{0.0};
    double y{0.0};
};

/// Result of a sampling operation.
struct SampleResult {
    std::vector<Point2D> points;  ///< Valid sample points
    bool        success{false};   ///< false when compilation or range fails
    std::string errorMsg;         ///< Non-empty when success == false
};

/// Sample  f(x) = expr  over the closed interval [xMin, xMax] using
/// `count` uniformly spaced steps.
///
/// Points where the expression evaluates to an invalid value (e.g.
/// division by zero, sqrt of negative number) are silently skipped so
/// that the caller always receives a clean sequence of finite points.
///
/// @param expr   Expression string accepted by the built-in parser
///               (supports +−*/^, sin/cos/tan/exp/log/sqrt/abs/…, constants pi/e).
/// @param xMin   Left boundary of the sampling interval.
/// @param xMax   Right boundary; must be strictly greater than xMin.
/// @param count  Number of evaluation points (minimum 2).
/// @return       SampleResult with success==true on valid input.
SampleResult sampleFunction(const std::string &expr,
                             double xMin,
                             double xMax,
                             int    count = 100);

} // namespace vm
