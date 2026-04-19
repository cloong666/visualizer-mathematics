#include "vm_core/Sampler.h"
#include "ExprParser.h"

namespace vm {

SampleResult sampleFunction(const std::string &expr,
                             double xMin,
                             double xMax,
                             int    count)
{
    SampleResult result;

    if (count < 2 || xMin >= xMax) {
        result.errorMsg = "Invalid range or sample count (need count >= 2 and xMin < xMax)";
        return result;
    }

    ExprParser parser;
    std::string compileErr;
    if (!parser.compile(expr, compileErr)) {
        result.errorMsg = compileErr;
        return result;
    }

    const double step = (xMax - xMin) / static_cast<double>(count - 1);
    result.points.reserve(static_cast<std::size_t>(count));

    for (int i = 0; i < count; ++i) {
        const double x = xMin + i * step;
        parser.setVar("x", x);
        bool valid = false;
        const double y = parser.evaluate(valid);
        if (valid) {
            result.points.push_back({x, y});
        }
    }

    result.success = true;
    return result;
}

} // namespace vm
