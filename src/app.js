'use strict';

// ════════════════════════════════════════════════
//  App State
// ════════════════════════════════════════════════

const state = {
  mode: '2d',        // '2d' | '3d'
  curves: [],        // Array of curve objects
  nextId: 0,
  plotInitialized: false,
};

// Default colors cycle
const COLOR_PALETTE = [
  '#7aa2f7', '#9ece6a', '#f7768e', '#e0af68', '#bb9af7',
  '#2ac3de', '#ff9e64', '#73daca', '#cfc9c2', '#41a6b5',
];

function nextColor() {
  return COLOR_PALETTE[state.nextId % COLOR_PALETTE.length];
}

// ════════════════════════════════════════════════
//  Examples
// ════════════════════════════════════════════════

const EXAMPLES = [
  // 2D explicit
  { name: 'Sine Wave', eq: 'y=sin(x)', mode: '2d' },
  { name: 'Parabola', eq: 'y=x^2-4', mode: '2d' },
  { name: 'Damped Sine', eq: 'y=exp(-0.3*x)*sin(3*x)', mode: '2d' },
  { name: 'Sinc Function', eq: 'y=sin(x)/x', mode: '2d' },
  { name: 'Cubic', eq: 'y=x^3-3*x', mode: '2d' },
  { name: 'Absolute Value', eq: 'y=abs(sin(x))', mode: '2d' },
  // 2D parametric
  { name: 'Unit Circle', eq: 'x=cos(t), y=sin(t)', mode: '2d' },
  { name: 'Ellipse', eq: 'x=3*cos(t), y=2*sin(t)', mode: '2d' },
  { name: 'Lissajous', eq: 'x=sin(3*t), y=sin(2*t)', mode: '2d' },
  { name: 'Spiral', eq: 'x=t*cos(t), y=t*sin(t)', mode: '2d' },
  { name: 'Butterfly', eq: 'x=sin(t)*(exp(cos(t))-2*cos(4*t)-sin(t/12)^5), y=cos(t)*(exp(cos(t))-2*cos(4*t)-sin(t/12)^5)', mode: '2d' },
  // 3D parametric
  { name: 'Helix', eq: 'x=cos(t), y=sin(t), z=t/(2*pi)', mode: '3d' },
  { name: 'Trefoil Knot', eq: 'x=sin(t)+2*sin(2*t), y=cos(t)-2*cos(2*t), z=-sin(3*t)', mode: '3d' },
  { name: 'Torus Knot', eq: 'x=(2+cos(3*t/2))*cos(t), y=(2+cos(3*t/2))*sin(t), z=sin(3*t/2)', mode: '3d' },
  { name: '3D Spiral', eq: 'x=t*cos(2*t), y=t*sin(2*t), z=t', mode: '3d' },
  // 3D surface
  { name: 'Ripple Surface', eq: 'z=sin(sqrt(x^2+y^2))', mode: '3d' },
  { name: 'Saddle', eq: 'z=x^2-y^2', mode: '3d' },
  { name: 'Paraboloid', eq: 'z=x^2+y^2', mode: '3d' },
  { name: 'Wavy Surface', eq: 'z=sin(x)*cos(y)', mode: '3d' },
];

// ════════════════════════════════════════════════
//  Mode switching
// ════════════════════════════════════════════════

function switchMode(mode) {
  if (state.mode === mode) return;
  state.mode = mode;
  document.getElementById('btn2d').classList.toggle('active', mode === '2d');
  document.getElementById('btn3d').classList.toggle('active', mode === '3d');
  renderExamples();
  renderAll();
}

// ════════════════════════════════════════════════
//  Add / Remove curves
// ════════════════════════════════════════════════

function addCurveFromInput() {
  const input = document.getElementById('equationInput').value.trim();
  if (!input) {
    showError('Please enter an equation.');
    return;
  }

  const color = document.getElementById('curveColor').value;
  const width = parseInt(document.getElementById('curveWidth').value, 10) || 2;

  addCurve(input, color, width);
}

function addCurve(input, color, width) {
  clearError();

  const parsed = EquationParser.parse(input);

  if (parsed.type === 'error') {
    showError(parsed.message);
    return;
  }

  // Mode mismatch warning (but still add)
  const is3D = parsed.type.startsWith('3d');
  if (is3D && state.mode === '2d') {
    switchMode('3d');
  }
  if (!is3D && state.mode === '3d' && parsed.type !== '3d_surface') {
    // 2D curves in 3D mode - just switch back
    switchMode('2d');
  }

  const curve = {
    id: state.nextId++,
    input,
    parsed,
    color: color || nextColor(),
    width: width || 2,
  };

  state.curves.push(curve);
  renderCurveList();
  renderAll();

  // Clear input
  document.getElementById('equationInput').value = '';
}

function removeCurve(id) {
  state.curves = state.curves.filter(c => c.id !== id);
  renderCurveList();
  renderAll();
}

function clearAll() {
  state.curves = [];
  state.nextId = 0;
  renderCurveList();
  renderAll();
}

// ════════════════════════════════════════════════
//  Render curve list
// ════════════════════════════════════════════════

function renderCurveList() {
  const el = document.getElementById('curveList');
  const count = document.getElementById('curveCount');
  count.textContent = state.curves.length;

  if (state.curves.length === 0) {
    el.innerHTML = '<div class="curve-list-empty">No curves added yet</div>';
    return;
  }

  el.innerHTML = state.curves.map(c => `
    <div class="curve-item" data-id="${c.id}">
      <div class="curve-swatch" style="background:${c.color}"></div>
      <div class="curve-name" title="${escHtml(c.input)}">${escHtml(c.input)}</div>
      <button class="curve-remove" onclick="removeCurve(${c.id})" title="Remove">✕</button>
    </div>
  `).join('');
}

function escHtml(s) {
  return s
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;');
}

// ════════════════════════════════════════════════
//  Error display
// ════════════════════════════════════════════════

function showError(msg) {
  const el = document.getElementById('errorBox');
  el.textContent = msg;
  el.classList.remove('hidden');
}

function clearError() {
  const el = document.getElementById('errorBox');
  el.classList.add('hidden');
  el.textContent = '';
}

// ════════════════════════════════════════════════
//  Get current config
// ════════════════════════════════════════════════

function getConfig() {
  const xMin = parseFloat(document.getElementById('xMin').value) || -10;
  const xMax = parseFloat(document.getElementById('xMax').value) || 10;
  return {
    xMin,
    xMax,
    // Y surface range mirrors X range (no separate Y inputs in the UI)
    yMin: xMin,
    yMax: xMax,
    tMin: parseFloat(document.getElementById('tMin').value) || -Math.PI * 2,
    tMax: parseFloat(document.getElementById('tMax').value) || Math.PI * 2,
    samples: parseInt(document.getElementById('samples').value, 10) || 1000,
    surfGrid: parseInt(document.getElementById('surfGrid').value, 10) || 60,
  };
}

// ════════════════════════════════════════════════
//  Render all curves (Plotly)
// ════════════════════════════════════════════════

function renderAll() {
  const plotEl = document.getElementById('plot');
  const emptyEl = document.getElementById('plotEmpty');
  const config = getConfig();

  if (state.curves.length === 0) {
    emptyEl.classList.remove('hidden');
    if (state.plotInitialized) {
      Plotly.purge(plotEl);
      state.plotInitialized = false;
    }
    return;
  }
  emptyEl.classList.add('hidden');

  // Build traces
  const traces = [];
  const errors = [];
  for (const curve of state.curves) {
    try {
      const t = Plotter.buildTrace(curve, state.mode, config);
      traces.push(...t);
    } catch (e) {
      errors.push(`"${curve.input}": ${e.message}`);
    }
  }

  if (errors.length) showError(errors.join('\n'));

  const layout = state.mode === '2d' ? Plotter.layout2D() : Plotter.layout3D();
  const plotConfig = {
    responsive: true,
    displaylogo: false,
    modeBarButtonsToRemove: ['sendDataToCloud'],
  };

  if (state.plotInitialized) {
    Plotly.react(plotEl, traces, layout, plotConfig);
  } else {
    Plotly.newPlot(plotEl, traces, layout, plotConfig);
    state.plotInitialized = true;
  }
}

function replotAll() {
  clearError();
  state.plotInitialized = false;
  renderAll();
}

// ════════════════════════════════════════════════
//  Render examples
// ════════════════════════════════════════════════

function renderExamples() {
  const el = document.getElementById('exampleList');
  const filtered = EXAMPLES.filter(ex => ex.mode === state.mode || ex.mode === 'any');

  el.innerHTML = filtered.map(ex => `
    <button class="example-btn" data-eq="${escHtml(ex.eq)}">
      <span class="example-name">${escHtml(ex.name)}</span>
      <span class="example-eq">${escHtml(ex.eq)}</span>
    </button>
  `).join('');
}

function loadExample(eq) {
  document.getElementById('equationInput').value = eq;
  document.getElementById('curveColor').value = COLOR_PALETTE[state.nextId % COLOR_PALETTE.length];
  addCurveFromInput();
}

// ════════════════════════════════════════════════
//  Keyboard shortcuts
// ════════════════════════════════════════════════

document.addEventListener('keydown', e => {
  if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') {
    if (e.key === 'Enter' && (e.ctrlKey || e.metaKey)) {
      addCurveFromInput();
    }
    return;
  }
  if (e.key === '2') switchMode('2d');
  if (e.key === '3') switchMode('3d');
});

// ════════════════════════════════════════════════
//  Init
// ════════════════════════════════════════════════

function init() {
  document.getElementById('btnClearAll').addEventListener('click', clearAll);

  // Event delegation for example buttons (avoids inline onclick + XSS risk)
  document.getElementById('exampleList').addEventListener('click', e => {
    const btn = e.target.closest('.example-btn');
    if (btn) loadExample(btn.dataset.eq);
  });

  renderCurveList();
  renderExamples();

  // Add a demo curve on load
  addCurve('y=sin(x)', '#7aa2f7', 2);
}

// Wait for DOM + CDN scripts to load
if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', init);
} else {
  // DOM already ready, but wait for CDN scripts
  window.addEventListener('load', init);
}
