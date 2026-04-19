#include "vm_core/AsciiPlot.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace vm {

// ─────────────────────────────────────────────────────────────────────────────
// asciiPlot
// ─────────────────────────────────────────────────────────────────────────────
std::vector<std::string> asciiPlot(
    const std::vector<Point2D>  &points,
    const CoordMapper            &mapX,
    const CoordMapper            &mapY,
    const AsciiPlotOptions       &opts)
{
    if (points.empty()) return {};

    const int W = std::max(opts.width,  4);
    const int H = std::max(opts.height, 4);

    // Allocate a W×H grid of spaces.
    std::vector<std::string> grid(static_cast<std::size_t>(H),
                                  std::string(static_cast<std::size_t>(W), ' '));

    // Helper: clamp column / row to grid bounds.
    auto clampCol = [W](int c) { return std::max(0, std::min(W - 1, c)); };
    auto clampRow = [H](int r) { return std::max(0, std::min(H - 1, r)); };

    // ── Draw axes (if enabled and in range) ──────────────────────────────────
    if (opts.showAxes) {
        // X axis: the row where math Y == 0
        double rowD = mapY.toScreen(0.0);
        if (rowD >= 0.0 && rowD < static_cast<double>(H)) {
            int row = clampRow(static_cast<int>(std::round(rowD)));
            for (int c = 0; c < W; ++c) grid[static_cast<std::size_t>(row)][static_cast<std::size_t>(c)] = opts.axisH;
        }

        // Y axis: the column where math X == 0
        double colD = mapX.toScreen(0.0);
        if (colD >= 0.0 && colD < static_cast<double>(W)) {
            int col = clampCol(static_cast<int>(std::round(colD)));
            for (int r = 0; r < H; ++r) grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(col)] = opts.axisV;
        }

        // Origin character at intersection (if both axes are visible)
        double rowD2 = mapY.toScreen(0.0);
        double colD2 = mapX.toScreen(0.0);
        if (rowD2 >= 0.0 && rowD2 < static_cast<double>(H) &&
            colD2 >= 0.0 && colD2 < static_cast<double>(W)) {
            int row = clampRow(static_cast<int>(std::round(rowD2)));
            int col = clampCol(static_cast<int>(std::round(colD2)));
            grid[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] = opts.origin;
        }
    }

    // ── Plot each sample point ────────────────────────────────────────────────
    for (const auto &p : points) {
        int col = clampCol(static_cast<int>(std::round(mapX.toScreen(p.x))));
        int row = clampRow(static_cast<int>(std::round(mapY.toScreen(p.y))));
        grid[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] = opts.curve;
    }

    return grid;
}

// ─────────────────────────────────────────────────────────────────────────────
// asciiPlotAuto
// ─────────────────────────────────────────────────────────────────────────────
std::vector<std::string> asciiPlotAuto(
    const std::vector<Point2D>  &points,
    const AsciiPlotOptions       &opts)
{
    if (points.empty()) return {};

    double xMin = points[0].x, xMax = points[0].x;
    double yMin = points[0].y, yMax = points[0].y;
    for (const auto &p : points) {
        xMin = std::min(xMin, p.x);
        xMax = std::max(xMax, p.x);
        yMin = std::min(yMin, p.y);
        yMax = std::max(yMax, p.y);
    }

    // Avoid degenerate ranges (flat line or single point).
    if (xMax == xMin) { xMin -= 1.0; xMax += 1.0; }
    if (yMax == yMin) { yMin -= 1.0; yMax += 1.0; }

    // Add 5 % padding
    double xPad = (xMax - xMin) * 0.05;
    double yPad = (yMax - yMin) * 0.05;
    xMin -= xPad; xMax += xPad;
    yMin -= yPad; yMax += yPad;

    const int W = std::max(opts.width,  4);
    const int H = std::max(opts.height, 4);

    // Y axis is inverted: row 0 is the top of the terminal (high Y).
    CoordMapper mapX(xMin, xMax, 0.0, static_cast<double>(W - 1), false);
    CoordMapper mapY(yMin, yMax, 0.0, static_cast<double>(H - 1), true);

    return asciiPlot(points, mapX, mapY, opts);
}

} // namespace vm
