#pragma once
#include <string>
#include <vector>

// ──────────────────────────────────────────────────────────────────────────────
// Built-in function catalogue: each entry has a display name and the
// expression string accepted by ExprParser (variable 'x').
// ──────────────────────────────────────────────────────────────────────────────
struct FunctionDef {
    std::string name;  // Human-readable label shown in the UI
    std::string expr;  // Expression string (uses variable 'x')
};

inline std::vector<FunctionDef> getBuiltinFunctions() {
    return {
        { "sin(x)",              "sin(x)"              },
        { "cos(x)",              "cos(x)"              },
        { "tan(x)",              "tan(x)"              },
        { "x^2",                 "x^2"                 },
        { "x^3 - 3*x",          "x^3 - 3*x"          },
        { "sin(x) / x",         "sin(x) / x"         },
        { "exp(-x^2)",           "exp(-x^2)"           },
        { "sqrt(abs(x))",        "sqrt(abs(x))"        },
        { "1 / (1 + x^2)",      "1 / (1 + x^2)"      },
        { "x * sin(x)",          "x * sin(x)"          },
        { "sin(3*x) * cos(2*x)", "sin(3*x) * cos(2*x)" },
        { "floor(x)",            "floor(x)"            },
    };
}
