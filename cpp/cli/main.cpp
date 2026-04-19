#include <iostream>
#include <iomanip>
#include <cmath>
#include "vm_core/Sampler.h"
#include "vm_core/Vec2.h"

static constexpr double PI = 3.141592653589793238462643383;

// Print a section banner
static void section(const char *title) {
    std::cout << "\n── " << title << " ──\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════╗\n"
              << "║  vm_cli · Mathematics Visualizer Core Demo       ║\n"
              << "╚══════════════════════════════════════════════════╝\n";

    // ── Demo 1: function sampling ────────────────────────────────
    section("Function sampling: sin(x) over [-π, π] (13 points)");
    {
        auto r = vm::sampleFunction("sin(x)", -PI, PI, 13);
        if (!r.success) {
            std::cerr << "Error: " << r.errorMsg << "\n";
            return 1;
        }
        std::cout << std::fixed << std::setprecision(5);
        for (const auto &p : r.points) {
            std::cout << "  x = " << std::setw(9) << p.x
                      << "   y = " << std::setw(9) << p.y << "\n";
        }
    }

    // ── Demo 2: a function with domain issues ────────────────────
    section("Function sampling: sqrt(x) over [-2, 4] (invalid points skipped)");
    {
        auto r = vm::sampleFunction("sqrt(x)", -2.0, 4.0, 7);
        if (!r.success) {
            std::cerr << "Error: " << r.errorMsg << "\n";
            return 1;
        }
        std::cout << "  " << r.points.size() << " valid point(s) out of 7 requested:\n";
        for (const auto &p : r.points) {
            std::cout << "  x = " << std::setw(9) << p.x
                      << "   y = " << std::setw(9) << p.y << "\n";
        }
    }

    // ── Demo 3: Vec2 operations ──────────────────────────────────
    section("Vec2 operations");
    {
        vm::Vec2 a{3.0, 4.0};
        vm::Vec2 b{1.0, 2.0};
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "  a           = (" << a.x << ", " << a.y << ")\n";
        std::cout << "  b           = (" << b.x << ", " << b.y << ")\n";
        std::cout << "  a + b       = (" << (a+b).x << ", " << (a+b).y << ")\n";
        std::cout << "  a - b       = (" << (a-b).x << ", " << (a-b).y << ")\n";
        std::cout << "  a * 2       = (" << (a*2).x << ", " << (a*2).y << ")\n";
        std::cout << "  |a|         = " << a.length() << "   (expected 5.0)\n";
        std::cout << "  a · b       = " << a.dot(b)   << "   (expected 11.0)\n";
        std::cout << "  a.normalize = ("
                  << a.normalize().x << ", " << a.normalize().y << ")\n";
    }

    std::cout << "\nDone.\n";
    return 0;
}
