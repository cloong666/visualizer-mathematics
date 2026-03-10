'use strict';

/**
 * Plotter - generates sample points and creates Plotly traces
 */
const Plotter = (() => {

  // Math.js scope with common constants
  const BASE_SCOPE = {
    pi: Math.PI,
    e: Math.E,
    tau: 2 * Math.PI,
    phi: (1 + Math.sqrt(5)) / 2,
    Infinity: Infinity,
  };

  function safeEval(expr, scope) {
    try {
      const val = math.evaluate(expr, { ...BASE_SCOPE, ...scope });
      if (typeof val === 'number' && isFinite(val)) return val;
      return null;
    } catch (_) {
      return null;
    }
  }

  function linspace(a, b, n) {
    const arr = new Array(n);
    const step = (b - a) / (n - 1);
    for (let i = 0; i < n; i++) arr[i] = a + i * step;
    return arr;
  }

  /**
   * Generate points for 2D explicit: y = f(x)
   */
  function gen2DExplicit(yExpr, xMin, xMax, samples) {
    const xs = linspace(xMin, xMax, samples);
    const x = [], y = [];
    let prevNull = false;

    for (const xi of xs) {
      const yi = safeEval(yExpr, { x: xi });
      if (yi === null) {
        if (!prevNull) { x.push(xi); y.push(null); }
        prevNull = true;
      } else {
        x.push(xi);
        y.push(yi);
        prevNull = false;
      }
    }
    return { x, y };
  }

  /**
   * Generate points for 2D parametric: x=fx(t), y=fy(t)
   */
  function gen2DParametric(xExpr, yExpr, tMin, tMax, samples) {
    const ts = linspace(tMin, tMax, samples);
    const x = [], y = [];
    let prevNull = false;

    for (const t of ts) {
      const xi = safeEval(xExpr, { t });
      const yi = safeEval(yExpr, { t });
      if (xi === null || yi === null) {
        if (!prevNull) { x.push(null); y.push(null); }
        prevNull = true;
      } else {
        x.push(xi);
        y.push(yi);
        prevNull = false;
      }
    }
    return { x, y };
  }

  /**
   * Generate points for 3D parametric: x=fx(t), y=fy(t), z=fz(t)
   */
  function gen3DParametric(xExpr, yExpr, zExpr, tMin, tMax, samples) {
    const ts = linspace(tMin, tMax, samples);
    const x = [], y = [], z = [];
    let prevNull = false;

    for (const t of ts) {
      const xi = safeEval(xExpr, { t });
      const yi = safeEval(yExpr, { t });
      const zi = safeEval(zExpr, { t });
      if (xi === null || yi === null || zi === null) {
        if (!prevNull) { x.push(null); y.push(null); z.push(null); }
        prevNull = true;
      } else {
        x.push(xi);
        y.push(yi);
        z.push(zi);
        prevNull = false;
      }
    }
    return { x, y, z };
  }

  /**
   * Generate surface data for 3D surface: z = f(x, y)
   */
  function gen3DSurface(zExpr, xMin, xMax, yMin, yMax, gridN) {
    const xs = linspace(xMin, xMax, gridN);
    const ys = linspace(yMin, yMax, gridN);
    const zData = [];

    for (const yi of ys) {
      const row = [];
      for (const xi of xs) {
        const v = safeEval(zExpr, { x: xi, y: yi });
        row.push(v === null ? null : v);
      }
      zData.push(row);
    }
    return { x: xs, y: ys, z: zData };
  }

  /** Plotly layout for 2D */
  function layout2D() {
    return {
      paper_bgcolor: '#0f0f1a',
      plot_bgcolor: '#1a1b2e',
      font: { color: '#c0caf5', size: 12 },
      xaxis: {
        color: '#a9b1d6',
        gridcolor: '#2d2f4a',
        zerolinecolor: '#5c6091',
        zerolinewidth: 2,
        title: { text: 'x', font: { color: '#c0caf5' } },
        tickcolor: '#787c99',
      },
      yaxis: {
        color: '#a9b1d6',
        gridcolor: '#2d2f4a',
        zerolinecolor: '#5c6091',
        zerolinewidth: 2,
        title: { text: 'y', font: { color: '#c0caf5' } },
        tickcolor: '#787c99',
      },
      legend: {
        bgcolor: '#24253a',
        bordercolor: '#383a5c',
        borderwidth: 1,
        font: { color: '#c0caf5', size: 11 },
      },
      margin: { l: 55, r: 25, t: 35, b: 50 },
      hovermode: 'closest',
      showlegend: true,
    };
  }

  /** Plotly layout for 3D */
  function layout3D() {
    const axisStyle = {
      backgroundcolor: '#1a1b2e',
      gridcolor: '#2d2f4a',
      color: '#a9b1d6',
      zerolinecolor: '#5c6091',
    };
    return {
      paper_bgcolor: '#0f0f1a',
      font: { color: '#c0caf5', size: 12 },
      scene: {
        bgcolor: '#1a1b2e',
        xaxis: { ...axisStyle, title: { text: 'x', font: { color: '#c0caf5' } } },
        yaxis: { ...axisStyle, title: { text: 'y', font: { color: '#c0caf5' } } },
        zaxis: { ...axisStyle, title: { text: 'z', font: { color: '#c0caf5' } } },
        aspectmode: 'auto',
      },
      legend: {
        bgcolor: '#24253a',
        bordercolor: '#383a5c',
        borderwidth: 1,
        font: { color: '#c0caf5', size: 11 },
      },
      margin: { l: 0, r: 0, t: 35, b: 0 },
      showlegend: true,
    };
  }

  /**
   * Build a Plotly trace from a curve object and its parsed/generated data.
   * curve: { parsed, color, width, input }
   * mode: '2d' | '3d'
   * config: { xMin, xMax, tMin, tMax, samples, surfGrid }
   */
  function buildTrace(curve, mode, config) {
    const { parsed, color, width, input } = curve;
    const { xMin, xMax, tMin, tMax, samples, surfGrid } = config;
    const name = input.length > 40 ? input.slice(0, 37) + '...' : input;

    if (parsed.type === '2d_explicit') {
      const { x, y } = gen2DExplicit(parsed.y, xMin, xMax, samples);
      return [{
        type: 'scatter',
        mode: 'lines',
        x, y,
        name,
        line: { color, width },
        connectgaps: false,
      }];
    }

    if (parsed.type === '2d_parametric') {
      const { x, y } = gen2DParametric(parsed.x, parsed.y, tMin, tMax, samples);
      return [{
        type: 'scatter',
        mode: 'lines',
        x, y,
        name,
        line: { color, width },
        connectgaps: false,
      }];
    }

    if (parsed.type === '3d_parametric') {
      const { x, y, z } = gen3DParametric(parsed.x, parsed.y, parsed.z, tMin, tMax, samples);
      return [{
        type: 'scatter3d',
        mode: 'lines',
        x, y, z,
        name,
        line: { color, width },
        connectgaps: false,
      }];
    }

    if (parsed.type === '3d_surface') {
      const { x, y, z } = gen3DSurface(parsed.z, xMin, xMax, config.yMin, config.yMax, surfGrid);
      return [{
        type: 'surface',
        x, y, z,
        name,
        colorscale: 'Viridis',
        opacity: 0.85,
        showscale: false,
        contours: {
          z: { show: true, usecolormap: true, highlightcolor: '#42f462', project: { z: false } },
        },
      }];
    }

    return [];
  }

  return {
    buildTrace,
    layout2D,
    layout3D,
    gen2DExplicit,
    gen2DParametric,
    gen3DParametric,
    gen3DSurface,
  };
})();
