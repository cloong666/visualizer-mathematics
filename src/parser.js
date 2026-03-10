'use strict';

/**
 * Equation Parser
 * Parses user equation input into structured objects for the plotter.
 *
 * Supported formats:
 *   2D Explicit:    y=sin(x)   f(x)=x^2+1
 *   2D Parametric:  x=cos(t), y=sin(t)
 *   3D Parametric:  x=cos(t), y=sin(t), z=t/5
 *   3D Surface:     z=sin(sqrt(x^2+y^2))
 */

const EquationParser = (() => {

  /**
   * Preprocess a math expression for math.js evaluation.
   * Handles: implicit multiplication (2x, 2pi), ^ for power
   */
  function preprocess(expr) {
    if (!expr || typeof expr !== 'string') return '';
    return expr
      .trim()
      // Common constant aliases
      .replace(/\btau\b/gi, '(2*pi)')
      .replace(/\bphi\b/gi, '((1+sqrt(5))/2)')
      // Implicit multiplication: 2x, 2(, )(, )x
      .replace(/(\d)\s*([a-df-wyzA-DF-WYZ(])/g, '$1*$2')
      .replace(/\)\s*([a-zA-Z(])/g, ')*$1')
      // Remove spaces around operators for cleaner output
      .trim();
  }

  /**
   * Safely extract a right-hand side after "var =" pattern
   */
  function extractRHS(text, varName) {
    const pattern = new RegExp(
      // Matches: x=, x =, f(x)=, etc.
      `(?:^|[,;\\n])\\s*${varName}\\s*(?:\\([^)]*\\))?\\s*=\\s*([^,;\\n]+)`,
      'i'
    );
    const m = text.match(pattern);
    return m ? m[1].trim() : null;
  }

  /**
   * Parse an equation string into a structured object.
   * Returns: { type, ...fields, raw }
   * type: '2d_explicit' | '2d_parametric' | '3d_parametric' | '3d_surface' | 'error'
   */
  function parse(input) {
    if (!input || typeof input !== 'string') {
      return { type: 'error', message: 'Empty input' };
    }
    const raw = input;
    const s = input.trim().replace(/\s+/g, ' ');

    // ── 3D Parametric: has x=, y=, z= with t ──
    const hasXeq = /\bx\s*=/i.test(s);
    const hasYeq = /\by\s*=/i.test(s);
    const hasZeq = /\bz\s*=/i.test(s);

    if (hasXeq && hasYeq && hasZeq) {
      const xExpr = extractRHS(s, 'x');
      const yExpr = extractRHS(s, 'y');
      const zExpr = extractRHS(s, 'z');
      if (xExpr && yExpr && zExpr) {
        return {
          type: '3d_parametric',
          x: preprocess(xExpr),
          y: preprocess(yExpr),
          z: preprocess(zExpr),
          raw,
        };
      }
    }

    // ── 2D Parametric: has x= and y= ──
    if (hasXeq && hasYeq) {
      const xExpr = extractRHS(s, 'x');
      const yExpr = extractRHS(s, 'y');
      if (xExpr && yExpr) {
        return {
          type: '2d_parametric',
          x: preprocess(xExpr),
          y: preprocess(yExpr),
          raw,
        };
      }
    }

    // ── 3D Surface: z=f(x,y) ──
    if (hasZeq && !hasXeq && !hasYeq) {
      const zExpr = extractRHS(s, 'z');
      if (zExpr) {
        // Must reference x or y (otherwise it's just a constant)
        return {
          type: '3d_surface',
          z: preprocess(zExpr),
          raw,
        };
      }
    }

    // ── 2D Explicit: y=f(x) or f(x)=expr ──
    // Various forms: y=, Y=, f(x)=, g(x)=, F(X)=, etc.
    const explicitMatch = s.match(
      /^(?:[a-zA-Z]\([a-zA-Z]\)\s*=|[yY]\s*=)\s*(.+)$/
    );
    if (explicitMatch) {
      return {
        type: '2d_explicit',
        y: preprocess(explicitMatch[1].trim()),
        raw,
      };
    }

    // ── Fallback: treat as 2D explicit y = <expr> ──
    // Check that it's a valid-looking expression
    const hasOperators = /[+\-*/^()]|sin|cos|tan|log|exp|sqrt/.test(s);
    const hasX = /\bx\b/i.test(s);
    if (hasX || hasOperators) {
      return {
        type: '2d_explicit',
        y: preprocess(s),
        raw,
      };
    }

    return {
      type: 'error',
      message: `Cannot parse: "${input}". Try formats like y=sin(x) or x=cos(t), y=sin(t).`,
    };
  }

  /**
   * Validate an expression by trying to evaluate it at a sample point.
   * Returns { ok: true } or { ok: false, message: '...' }
   */
  function validate(expr, scope) {
    try {
      const result = math.evaluate(expr, { ...scope });
      if (result === undefined || result === null) {
        return { ok: false, message: `Expression "${expr}" returned undefined.` };
      }
      return { ok: true };
    } catch (e) {
      return { ok: false, message: `Invalid expression "${expr}": ${e.message}` };
    }
  }

  return { parse, validate, preprocess };
})();
