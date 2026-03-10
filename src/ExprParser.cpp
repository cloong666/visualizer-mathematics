#include "ExprParser.h"
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <vector>

// MSVC does not define M_PI / M_E without _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E  2.71828182845904523536
#endif

// ============================================================
//  AST node implementations
// ============================================================

struct ExprParser::NumberNode : Node {
    double value;
    explicit NumberNode(double v) : value(v) {}
    double eval(const ExprParser &, bool &) const override { return value; }
};

struct ExprParser::VarNode : Node {
    std::string name;
    explicit VarNode(std::string n) : name(std::move(n)) {}
    double eval(const ExprParser &ctx, bool &valid) const override {
        return ctx.lookupVar(name, valid);
    }
};

struct ExprParser::UnaryNode : Node {
    char op;
    NodePtr child;
    UnaryNode(char o, NodePtr c) : op(o), child(std::move(c)) {}
    double eval(const ExprParser &ctx, bool &valid) const override {
        double v = child->eval(ctx, valid);
        return (op == '-') ? -v : v;
    }
};

struct ExprParser::BinaryNode : Node {
    char op;
    NodePtr left, right;
    BinaryNode(char o, NodePtr l, NodePtr r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    double eval(const ExprParser &ctx, bool &valid) const override {
        double l = left->eval(ctx, valid);
        double r = right->eval(ctx, valid);
        if (!valid) return 0.0;
        switch (op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/':
            if (r == 0.0) { valid = false; return 0.0; }
            return l / r;
        case '^': return std::pow(l, r);
        }
        return 0.0;
    }
};

struct ExprParser::FuncNode : Node {
    std::string name;
    std::vector<NodePtr> args;
    FuncNode(std::string n) : name(std::move(n)) {}
    double eval(const ExprParser &ctx, bool &valid) const override {
        auto evalArg = [&](int i) {
            return (i < (int)args.size()) ? args[i]->eval(ctx, valid) : 0.0;
        };
        double a = evalArg(0);
        if (!valid) return 0.0;
        if (name == "sin")   return std::sin(a);
        if (name == "cos")   return std::cos(a);
        if (name == "tan")   return std::tan(a);
        if (name == "asin")  { if (a < -1 || a > 1) { valid = false; return 0; } return std::asin(a); }
        if (name == "acos")  { if (a < -1 || a > 1) { valid = false; return 0; } return std::acos(a); }
        if (name == "atan")  return std::atan(a);
        if (name == "atan2") { double b = evalArg(1); return std::atan2(a, b); }
        if (name == "sinh")  return std::sinh(a);
        if (name == "cosh")  return std::cosh(a);
        if (name == "tanh")  return std::tanh(a);
        if (name == "exp")   return std::exp(a);
        if (name == "log" || name == "ln") {
            if (a <= 0) { valid = false; return 0; }
            return std::log(a);
        }
        if (name == "log10") { if (a <= 0) { valid = false; return 0; } return std::log10(a); }
        if (name == "log2")  { if (a <= 0) { valid = false; return 0; } return std::log2(a); }
        if (name == "sqrt")  { if (a < 0) { valid = false; return 0; } return std::sqrt(a); }
        if (name == "cbrt")  return std::cbrt(a);
        if (name == "abs")   return std::abs(a);
        if (name == "ceil")  return std::ceil(a);
        if (name == "floor") return std::floor(a);
        if (name == "round") return std::round(a);
        if (name == "sign")  return (a > 0) ? 1.0 : (a < 0) ? -1.0 : 0.0;
        if (name == "min")   { double b = evalArg(1); return std::min(a, b); }
        if (name == "max")   { double b = evalArg(1); return std::max(a, b); }
        if (name == "pow")   { double b = evalArg(1); return std::pow(a, b); }
        // Unknown function – mark invalid
        valid = false;
        return 0.0;
    }
};

// ============================================================
//  ExprParser member functions
// ============================================================

ExprParser::ExprParser() {
    m_vars["pi"] = M_PI;
    m_vars["e"]  = M_E;
}

void ExprParser::setVar(const std::string &name, double value) {
    m_vars[name] = value;
}

bool ExprParser::compile(const std::string &expr, std::string &errorMsg) {
    m_src = expr;
    m_pos = 0;
    m_root.reset();
    try {
        skipWS();
        m_root = parseExpr();
        skipWS();
        if (m_pos != m_src.size()) {
            errorMsg = std::string("Unexpected character '") + m_src[m_pos] + "' at position " + std::to_string(m_pos);
            return false;
        }
        errorMsg.clear();
        return true;
    } catch (const std::exception &ex) {
        errorMsg = ex.what();
        return false;
    }
}

double ExprParser::evaluate(bool &valid) const {
    if (!m_root) { valid = false; return 0.0; }
    valid = true;
    double result = m_root->eval(*this, valid);
    if (std::isnan(result) || std::isinf(result)) { valid = false; return result; }
    return result;
}

double ExprParser::eval(const std::string &expr, bool &valid, std::string &errorMsg) {
    if (!compile(expr, errorMsg)) { valid = false; return 0.0; }
    return evaluate(valid);
}

double ExprParser::lookupVar(const std::string &name, bool &valid) const {
    auto it = m_vars.find(name);
    if (it == m_vars.end()) {
        valid = false;
        return 0.0;
    }
    return it->second;
}

// ---------- lexer ----------

void ExprParser::skipWS() {
    while (m_pos < m_src.size() && std::isspace((unsigned char)m_src[m_pos]))
        ++m_pos;
}

char ExprParser::peek() const {
    if (m_pos >= m_src.size()) return '\0';
    return m_src[m_pos];
}

char ExprParser::get() {
    return (m_pos < m_src.size()) ? m_src[m_pos++] : '\0';
}

bool ExprParser::match(char c) {
    if (peek() == c) { ++m_pos; return true; }
    return false;
}

// ---------- grammar ----------

ExprParser::NodePtr ExprParser::parseExpr()        { return parseAdditive(); }

ExprParser::NodePtr ExprParser::parseAdditive() {
    NodePtr left = parseMultiplicative();
    skipWS();
    while (peek() == '+' || peek() == '-') {
        char op = get();
        skipWS();
        NodePtr right = parseMultiplicative();
        left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
        skipWS();
    }
    return left;
}

ExprParser::NodePtr ExprParser::parseMultiplicative() {
    NodePtr left = parsePower();
    skipWS();
    while (peek() == '*' || peek() == '/') {
        char op = get();
        skipWS();
        NodePtr right = parsePower();
        left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
        skipWS();
    }
    return left;
}

ExprParser::NodePtr ExprParser::parsePower() {
    NodePtr base = parseUnary();
    skipWS();
    if (peek() == '^') {
        get(); // consume '^'
        skipWS();
        NodePtr exp = parsePower(); // right-associative
        return std::make_unique<BinaryNode>('^', std::move(base), std::move(exp));
    }
    return base;
}

ExprParser::NodePtr ExprParser::parseUnary() {
    skipWS();
    if (peek() == '+') { get(); skipWS(); return parseUnary(); }
    if (peek() == '-') {
        get(); skipWS();
        NodePtr child = parseUnary();
        return std::make_unique<UnaryNode>('-', std::move(child));
    }
    return parsePrimary();
}

ExprParser::NodePtr ExprParser::parsePrimary() {
    skipWS();
    char c = peek();

    // Number literal
    if (std::isdigit((unsigned char)c) || c == '.') {
        std::size_t start = m_pos;
        while (m_pos < m_src.size() &&
               (std::isdigit((unsigned char)m_src[m_pos]) || m_src[m_pos] == '.'))
            ++m_pos;
        // Optional exponent: e+3, e-2, e5
        if (m_pos < m_src.size() && (m_src[m_pos] == 'e' || m_src[m_pos] == 'E')) {
            ++m_pos;
            if (m_pos < m_src.size() && (m_src[m_pos] == '+' || m_src[m_pos] == '-'))
                ++m_pos;
            while (m_pos < m_src.size() && std::isdigit((unsigned char)m_src[m_pos]))
                ++m_pos;
        }
        double val = std::stod(m_src.substr(start, m_pos - start));
        return std::make_unique<NumberNode>(val);
    }

    // Identifier (variable, constant, or function)
    if (std::isalpha((unsigned char)c) || c == '_') {
        std::size_t start = m_pos;
        while (m_pos < m_src.size() &&
               (std::isalnum((unsigned char)m_src[m_pos]) || m_src[m_pos] == '_'))
            ++m_pos;
        std::string ident = m_src.substr(start, m_pos - start);
        skipWS();

        // Function call?
        if (peek() == '(') {
            get(); // consume '('
            skipWS();
            auto fn = std::make_unique<FuncNode>(ident);
            if (peek() != ')') {
                fn->args.push_back(parseExpr());
                skipWS();
                while (peek() == ',') {
                    get(); skipWS();
                    fn->args.push_back(parseExpr());
                    skipWS();
                }
            }
            if (!match(')'))
                throw std::runtime_error("Expected ')' after function arguments in '" + ident + "'");
            return fn;
        }

        // Constant or variable
        return std::make_unique<VarNode>(ident);
    }

    // Parenthesised expression
    if (c == '(') {
        get(); // consume '('
        skipWS();
        NodePtr inner = parseExpr();
        skipWS();
        if (!match(')'))
            throw std::runtime_error("Expected closing ')'");
        return inner;
    }

    if (c == '\0')
        throw std::runtime_error("Unexpected end of expression");
    throw std::runtime_error(std::string("Unexpected character '") + c + "' at position " + std::to_string(m_pos));
}
