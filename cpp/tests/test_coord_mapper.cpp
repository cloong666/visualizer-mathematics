// Tests for CoordMapper – no external framework required.
// Returns 0 on success; non-zero signals failure to CTest.
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

#include "vm_core/CoordMapper.h"
#include "vm_core/AsciiPlot.h"
#include "vm_core/CsvExport.h"

static int g_failures = 0;

#define CHECK(cond)                                                        \
    do {                                                                   \
        if (!(cond)) {                                                     \
            std::cerr << "FAIL [line " << __LINE__ << "]: " #cond "\n";  \
            ++g_failures;                                                  \
        } else {                                                           \
            std::cout << "pass: " #cond "\n";                             \
        }                                                                  \
    } while (false)

#define CHECK_NEAR(a, b, eps)                                              \
    do {                                                                   \
        double _a = (a), _b = (b), _e = (eps);                            \
        if (std::abs(_a - _b) > _e) {                                      \
            std::cerr << "FAIL [line " << __LINE__ << "]: |"              \
                      << #a << " - " << #b << "| = "                      \
                      << std::abs(_a - _b) << " > " << _e << "\n";        \
            ++g_failures;                                                  \
        } else {                                                           \
            std::cout << "pass: " #a " ≈ " #b "\n";                      \
        }                                                                  \
    } while (false)

// ── CoordMapper: basic forward mapping ────────────────────────────────────
static void test_coord_mapper_forward()
{
    std::cout << "\n-- CoordMapper: forward (math → screen) --\n";

    // Math range [-1, 1] mapped to screen [0, 100]
    vm::CoordMapper m(-1.0, 1.0, 0.0, 100.0);

    // Left boundary: math -1 → screen 0
    CHECK_NEAR(m.toScreen(-1.0), 0.0, 1e-9);

    // Right boundary: math +1 → screen 100
    CHECK_NEAR(m.toScreen(1.0), 100.0, 1e-9);

    // Mid-point: math 0 → screen 50
    CHECK_NEAR(m.toScreen(0.0), 50.0, 1e-9);

    // Quarter: math -0.5 → screen 25
    CHECK_NEAR(m.toScreen(-0.5), 25.0, 1e-9);
}

// ── CoordMapper: inverse mapping ──────────────────────────────────────────
static void test_coord_mapper_inverse()
{
    std::cout << "\n-- CoordMapper: inverse (screen → math) --\n";

    vm::CoordMapper m(-1.0, 1.0, 0.0, 100.0);

    CHECK_NEAR(m.toMath(0.0),   -1.0, 1e-9);
    CHECK_NEAR(m.toMath(100.0),  1.0, 1e-9);
    CHECK_NEAR(m.toMath(50.0),   0.0, 1e-9);
}

// ── CoordMapper: round-trip fidelity ──────────────────────────────────────
static void test_coord_mapper_roundtrip()
{
    std::cout << "\n-- CoordMapper: round-trip --\n";

    vm::CoordMapper m(-3.14, 3.14, 0.0, 800.0);
    const double values[] = {-3.14, -1.0, 0.0, 1.57, 3.14};
    for (double v : values) {
        CHECK_NEAR(m.toMath(m.toScreen(v)), v, 1e-9);
    }
}

// ── CoordMapper: Y-inversion ──────────────────────────────────────────────
static void test_coord_mapper_invert_y()
{
    std::cout << "\n-- CoordMapper: Y-axis inversion --\n";

    // With invertY=true: math max → screen min (row 0 at the top)
    vm::CoordMapper m(0.0, 10.0, 0.0, 9.0, /*invertY=*/true);

    // math 10 (highest) → row 0 (top of screen)
    CHECK_NEAR(m.toScreen(10.0), 0.0, 1e-9);

    // math 0 (lowest) → row 9 (bottom of screen)
    CHECK_NEAR(m.toScreen(0.0), 9.0, 1e-9);

    // math 5 → row 4.5 (midpoint)
    CHECK_NEAR(m.toScreen(5.0), 4.5, 1e-9);
}

// ── CoordMapper: degenerate range (mathMin == mathMax) ────────────────────
static void test_coord_mapper_degenerate()
{
    std::cout << "\n-- CoordMapper: degenerate math range --\n";

    vm::CoordMapper m(5.0, 5.0, 0.0, 100.0);
    // Should return screenMin without division by zero.
    CHECK_NEAR(m.toScreen(5.0), 0.0, 1e-9);
    CHECK_NEAR(m.toScreen(99.0), 0.0, 1e-9);
}

// ── AsciiPlot: smoke test ─────────────────────────────────────────────────
static void test_ascii_plot_smoke()
{
    std::cout << "\n-- AsciiPlot: smoke test (non-empty output) --\n";

    std::vector<vm::Point2D> pts;
    for (int i = 0; i <= 20; ++i) {
        double x = -3.14 + i * (6.28 / 20.0);
        pts.push_back({x, std::sin(x)});
    }

    vm::AsciiPlotOptions opts;
    opts.width  = 40;
    opts.height = 10;

    auto rows = vm::asciiPlotAuto(pts, opts);
    CHECK(rows.size() == 10u);
    for (const auto &row : rows) {
        CHECK(row.size() == 40u);
    }

    // At least one '*' must appear somewhere
    bool found = false;
    for (const auto &row : rows) {
        if (row.find('*') != std::string::npos) { found = true; break; }
    }
    CHECK(found);
}

// ── CsvExport: smoke test ─────────────────────────────────────────────────
static void test_csv_export_smoke()
{
    std::cout << "\n-- CsvExport: smoke test --\n";

    std::vector<vm::Point2D> pts = {{0.0, 0.0}, {1.0, 1.0}, {2.0, 4.0}};
    std::ostringstream oss;
    vm::exportCsv(pts, oss);
    const std::string out = oss.str();

    // Should start with header
    CHECK(out.substr(0, 3) == "x,y");
    // Should contain the first data point
    CHECK(out.find("0,0") != std::string::npos);
}

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    test_coord_mapper_forward();
    test_coord_mapper_inverse();
    test_coord_mapper_roundtrip();
    test_coord_mapper_invert_y();
    test_coord_mapper_degenerate();
    test_ascii_plot_smoke();
    test_csv_export_smoke();

    if (g_failures > 0) {
        std::cerr << "\n" << g_failures << " test(s) FAILED.\n";
        return 1;
    }
    std::cout << "\nAll CoordMapper / AsciiPlot / CsvExport tests passed.\n";
    return 0;
}
