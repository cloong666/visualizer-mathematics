#pragma once
#include "vm_core/Sampler.h"
#include <ostream>
#include <string>

namespace vm {

/// Write sampled points as CSV to an output stream.
///
/// The first line is the header "x,y".  Each subsequent line contains
/// one point formatted as "x_value,y_value".
///
/// @param points  Points to export (may be empty – only header is written).
/// @param out     Target stream (e.g. std::cout or a std::ofstream).
/// @param sep     Field separator character (default ',').
inline void exportCsv(const std::vector<Point2D> &points,
                      std::ostream               &out,
                      char                        sep = ',')
{
    out << "x" << sep << "y\n";
    for (const auto &p : points) {
        out << p.x << sep << p.y << "\n";
    }
}

} // namespace vm
