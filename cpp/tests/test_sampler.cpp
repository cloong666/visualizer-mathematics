// Self-contained test runner – no external framework required.
// Returns 0 on success; non-zero exit code signals failure to CTest.
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include "vm_core/Sampler.h"
#include "vm_core/Vec2.h"

// ── minimal CHECK macro ────────────────────────────────────────────
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

// ── test groups ────────────────────────────────────────────────────

static void test_sampler_basic() {
    std::cout << "\n-- Sampler: basic sin(x) --\n";
    const double pi = 3.141592653589793;
    auto r = vm::sampleFunction("sin(x)", 0.0, pi, 5);

    CHECK(r.success);
    CHECK(r.points.size() == 5u);
    CHECK(r.errorMsg.empty());

    // sin(0) == 0
    CHECK_NEAR(r.points[0].x, 0.0,   1e-12);
    CHECK_NEAR(r.points[0].y, 0.0,   1e-9);

    // sin(π/2) == 1  (index 2 of 5 evenly-spaced points)
    CHECK_NEAR(r.points[2].x, pi / 2.0, 1e-12);
    CHECK_NEAR(r.points[2].y, 1.0,      1e-9);

    // sin(π) ≈ 0
    CHECK_NEAR(r.points[4].x, pi,  1e-12);
    CHECK_NEAR(r.points[4].y, 0.0, 1e-9);
}

static void test_sampler_invalid_expr() {
    std::cout << "\n-- Sampler: invalid expression --\n";
    auto r = vm::sampleFunction("??garbage??", 0.0, 1.0, 10);
    CHECK(!r.success);
    CHECK(!r.errorMsg.empty());
    CHECK(r.points.empty());
}

static void test_sampler_bad_range() {
    std::cout << "\n-- Sampler: bad range (xMin >= xMax) --\n";
    auto r = vm::sampleFunction("x", 5.0, 1.0, 10);
    CHECK(!r.success);

    auto r2 = vm::sampleFunction("x", 1.0, 1.0, 10);   // equal bounds
    CHECK(!r2.success);
}

static void test_sampler_bad_count() {
    std::cout << "\n-- Sampler: count < 2 --\n";
    auto r = vm::sampleFunction("x", 0.0, 1.0, 1);
    CHECK(!r.success);
}

static void test_sampler_domain_skip() {
    std::cout << "\n-- Sampler: domain errors skipped (sqrt(x) over [-1, 1]) --\n";
    // sqrt(x) is undefined for x < 0; those points should be silently dropped.
    auto r = vm::sampleFunction("sqrt(x)", -1.0, 1.0, 3);
    CHECK(r.success);
    // x = -1 (invalid), x = 0 (valid, y=0), x = 1 (valid, y=1)
    CHECK(r.points.size() == 2u);
    CHECK_NEAR(r.points[0].y, 0.0, 1e-9);
    CHECK_NEAR(r.points[1].y, 1.0, 1e-9);
}

static void test_sampler_constant_expr() {
    std::cout << "\n-- Sampler: constant expression --\n";
    auto r = vm::sampleFunction("pi", -1.0, 1.0, 5);
    CHECK(r.success);
    CHECK(r.points.size() == 5u);
    const double pi = 3.141592653589793;
    for (const auto &p : r.points) {
        CHECK_NEAR(p.y, pi, 1e-12);
    }
}

static void test_vec2_length() {
    std::cout << "\n-- Vec2: length --\n";
    vm::Vec2 a{3.0, 4.0};
    CHECK_NEAR(a.length(), 5.0, 1e-12);
    CHECK_NEAR(a.lengthSq(), 25.0, 1e-12);

    vm::Vec2 zero{0.0, 0.0};
    CHECK_NEAR(zero.length(), 0.0, 1e-12);
}

static void test_vec2_arithmetic() {
    std::cout << "\n-- Vec2: arithmetic --\n";
    vm::Vec2 a{1.0, 2.0};
    vm::Vec2 b{3.0, 4.0};

    auto sum = a + b;
    CHECK_NEAR(sum.x, 4.0, 1e-12);
    CHECK_NEAR(sum.y, 6.0, 1e-12);

    auto diff = b - a;
    CHECK_NEAR(diff.x, 2.0, 1e-12);
    CHECK_NEAR(diff.y, 2.0, 1e-12);

    auto scaled = a * 3.0;
    CHECK_NEAR(scaled.x, 3.0, 1e-12);
    CHECK_NEAR(scaled.y, 6.0, 1e-12);

    auto scaled2 = 3.0 * a;
    CHECK_NEAR(scaled2.x, 3.0, 1e-12);
    CHECK_NEAR(scaled2.y, 6.0, 1e-12);
}

static void test_vec2_dot() {
    std::cout << "\n-- Vec2: dot product --\n";
    vm::Vec2 a{3.0, 4.0};
    vm::Vec2 b{1.0, 2.0};
    CHECK_NEAR(a.dot(b), 11.0, 1e-12);   // 3*1 + 4*2 = 11

    vm::Vec2 ex{1.0, 0.0};
    vm::Vec2 ey{0.0, 1.0};
    CHECK_NEAR(ex.dot(ey), 0.0, 1e-12);  // orthogonal
}

static void test_vec2_normalize() {
    std::cout << "\n-- Vec2: normalize --\n";
    vm::Vec2 a{3.0, 4.0};
    auto n = a.normalize();
    CHECK_NEAR(n.length(), 1.0, 1e-12);
    CHECK_NEAR(n.x, 0.6, 1e-12);
    CHECK_NEAR(n.y, 0.8, 1e-12);

    vm::Vec2 zero{0.0, 0.0};
    auto nz = zero.normalize();
    CHECK_NEAR(nz.x, 0.0, 1e-12);
    CHECK_NEAR(nz.y, 0.0, 1e-12);
}

static void test_vec2_division() {
    std::cout << "\n-- Vec2: division (including zero-divisor guard) --\n";
    vm::Vec2 a{6.0, 4.0};
    auto d = a / 2.0;
    CHECK_NEAR(d.x, 3.0, 1e-12);
    CHECK_NEAR(d.y, 2.0, 1e-12);

    // Division by zero must return (0, 0) rather than UB / inf.
    auto dz = a / 0.0;
    CHECK_NEAR(dz.x, 0.0, 1e-12);
    CHECK_NEAR(dz.y, 0.0, 1e-12);
}

// ── main ───────────────────────────────────────────────────────────

int main() {
    std::cout << "=== vm_core unit tests ===\n";

    test_sampler_basic();
    test_sampler_invalid_expr();
    test_sampler_bad_range();
    test_sampler_bad_count();
    test_sampler_domain_skip();
    test_sampler_constant_expr();

    test_vec2_length();
    test_vec2_arithmetic();
    test_vec2_dot();
    test_vec2_normalize();
    test_vec2_division();

    std::cout << "\n";
    if (g_failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }
    std::cerr << g_failures << " test(s) FAILED.\n";
    return 1;
}
