#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include "vm_core/Sampler.h"
#include "vm_core/Vec2.h"
#include "vm_core/CoordMapper.h"
#include "vm_core/AsciiPlot.h"
#include "vm_core/CsvExport.h"

static constexpr double PI = 3.141592653589793238462643383;

// Print a section banner
static void section(const char *title) {
    std::cout << "\n── " << title << " ──\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════╗\n"
              << "║  vm_cli · Mathematics Visualizer Core Demo       ║\n"
              << "║  Version 1.0 – 2D Curve Visualization           ║\n"
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

    // ── Demo 4: CoordMapper ──────────────────────────────────────
    section("CoordMapper: math [-π, π] → screen [0, 79]");
    {
        vm::CoordMapper mapX(-PI, PI, 0.0, 79.0);
        vm::CoordMapper mapY(-1.0, 1.0, 0.0, 23.0, /*invertY=*/true);
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  math x=-π → col " << mapX.toScreen(-PI)  << "\n";
        std::cout << "  math x= 0 → col " << mapX.toScreen(0.0)  << "\n";
        std::cout << "  math x= π → col " << mapX.toScreen(PI)   << "\n";
        std::cout << "  math y= 1 → row " << mapY.toScreen(1.0)  << "  (top)\n";
        std::cout << "  math y= 0 → row " << mapY.toScreen(0.0)  << "  (mid)\n";
        std::cout << "  math y=-1 → row " << mapY.toScreen(-1.0) << "  (bottom)\n";
    }

    // ── Demo 5: ASCII plot of sin(x) ─────────────────────────────
    section("ASCII plot: sin(x) over [-π, π]");
    {
        auto r = vm::sampleFunction("sin(x)", -PI, PI, 200);
        if (!r.success) {
            std::cerr << "Error: " << r.errorMsg << "\n";
            return 1;
        }
        vm::AsciiPlotOptions opts;
        opts.width  = 72;
        opts.height = 20;
        auto rows = vm::asciiPlotAuto(r.points, opts);
        for (const auto &row : rows) {
            std::cout << row << "\n";
        }
    }

    // ── Demo 6: CSV export of x² over [0, 5] ────────────────────
    section("CSV export: x^2 over [0, 5] (10 points) → x_squared.csv");
    {
        auto r = vm::sampleFunction("x^2", 0.0, 5.0, 10);
        if (!r.success) {
            std::cerr << "Error: " << r.errorMsg << "\n";
            return 1;
        }
        std::ofstream f("x_squared.csv");
        if (f.is_open()) {
            vm::exportCsv(r.points, f);
            std::cout << "  Written " << r.points.size()
                      << " rows to x_squared.csv\n";
        } else {
            // Fall back to stdout if file cannot be created
            std::cout << "  (file unavailable – printing to stdout)\n";
            vm::exportCsv(r.points, std::cout);
        }
    }

    std::cout << "\nDone.\n";
    return 0;
}
