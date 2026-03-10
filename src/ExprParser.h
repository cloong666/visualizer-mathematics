#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <cmath>

// Recursive-descent expression parser supporting:
//   variables (x, t, and any user-defined name)
//   constants  (pi, e)
//   operators  + - * / ^ (right-associative power)
//   unary      + -
//   functions  sin cos tan asin acos atan atan2 sinh cosh tanh
//              exp log log10 log2 sqrt cbrt abs ceil floor sign
class ExprParser {
public:
    ExprParser();

    // Set (or update) a variable value before calling evaluate().
    void setVar(const std::string &name, double value);

    // Compile the expression string.
    // Returns true on success; sets errorMsg on failure.
    bool compile(const std::string &expr, std::string &errorMsg);

    // Evaluate the compiled expression.
    // Sets valid=false (and returns 0) on division-by-zero / domain error.
    double evaluate(bool &valid) const;

    // Quick one-shot helper: compile + evaluate in one call.
    double eval(const std::string &expr, bool &valid, std::string &errorMsg);

private:
    // ---------- AST nodes ----------
    struct Node {
        virtual ~Node() = default;
        virtual double eval(const ExprParser &ctx, bool &valid) const = 0;
    };
    using NodePtr = std::unique_ptr<Node>;

    struct NumberNode;
    struct VarNode;
    struct UnaryNode;
    struct BinaryNode;
    struct FuncNode;

    // ---------- parser state ----------
    std::string               m_src;
    std::size_t               m_pos{0};
    NodePtr                   m_root;
    std::unordered_map<std::string, double> m_vars;

    // ---------- lexer helpers ----------
    void skipWS();
    char peek() const;
    char get();
    bool match(char c);

    // ---------- recursive-descent rules ----------
    NodePtr parseExpr();          // lowest precedence: additive
    NodePtr parseAdditive();
    NodePtr parseMultiplicative();
    NodePtr parsePower();         // right-associative
    NodePtr parseUnary();
    NodePtr parsePrimary();

    double lookupVar(const std::string &name, bool &valid) const;
};
