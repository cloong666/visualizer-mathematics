#pragma once
#include "vm_core/Sampler.h"
#include "vm_core/CoordMapper.h"
#include <string>
#include <vector>

namespace vm {

/// Options controlling the appearance of an ASCII plot.
struct AsciiPlotOptions {
    int    width    = 80;   ///< Terminal columns (including axes and labels).
    int    height   = 24;   ///< Terminal rows.
    char   curve    = '*';  ///< Character used to draw the curve.
    char   axisH    = '-';  ///< Horizontal axis character.
    char   axisV    = '|';  ///< Vertical axis character.
    char   origin   = '+';  ///< Character at axis intersection.
    bool   showAxes = true; ///< Draw X and Y axes when in range.
};

/// Renders a set of 2-D sample points as an ASCII grid.
///
/// The grid maps the bounding box of the sample data (or the mapper
/// range when provided) to a character grid and prints each row as a
/// std::string.  The result can be joined with newlines and written to
/// any stream.
///
/// @param points   The sampled (x, y) pairs – must not be empty.
/// @param mapX     Coordinate mapper for the X axis (math → column index).
/// @param mapY     Coordinate mapper for the Y axis (math → row index),
///                 typically constructed with invertY = true so that
///                 larger Y values appear higher on the terminal.
/// @param opts     Appearance options (width, height, characters).
/// @return         One string per row, ready to print.
std::vector<std::string> asciiPlot(
    const std::vector<Point2D>  &points,
    const CoordMapper            &mapX,
    const CoordMapper            &mapY,
    const AsciiPlotOptions       &opts = {});

/// Convenience overload: infers the math bounds from the sample data
/// and constructs the mappers automatically.
///
/// @param points   The sampled (x, y) pairs – must not be empty.
/// @param opts     Appearance options.
/// @return         One string per row, ready to print.
std::vector<std::string> asciiPlotAuto(
    const std::vector<Point2D>  &points,
    const AsciiPlotOptions       &opts = {});

} // namespace vm
