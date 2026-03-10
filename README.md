# Mathematics Curve Visualizer

A **2D/3D Cartesian coordinate curve visualizer** built with HTML/CSS/JavaScript, Plotly.js, and math.js — packaged as a cross-platform desktop application with Electron.

---

## Features

- **2D Explicit curves** — `y = f(x)` (e.g. `y=sin(x)`, `y=x^2-4`)
- **2D Parametric curves** — `x = f(t), y = g(t)` (e.g. circles, spirals, Lissajous figures)
- **3D Parametric curves** — `x = f(t), y = g(t), z = h(t)` (e.g. helices, knots)
- **3D Surfaces** — `z = f(x, y)` (e.g. ripple, saddle, paraboloid)
- Multiple curves overlaid on a single plot with individual colors and line widths
- Interactive pan, zoom, and rotate (powered by Plotly.js)
- Dark theme UI with a left sidebar for controls and a full-width plot canvas
- Keyboard shortcuts: `Ctrl+Enter` to add a curve, `2`/`3` to switch modes
- 18+ built-in examples covering all equation types
- Configurable X range, T range, sample count, and surface grid resolution

---

## Screenshots

The application features a **dark-themed split-panel layout**:

- **Left sidebar** — equation input textarea, color picker, line-width selector, curve list with remove buttons, range/sampling controls, example gallery, and input-format reference
- **Right plot area** — interactive Plotly canvas that fills the remaining space; supports pan, zoom, hover tooltips, and 3D rotation

---

## Running the App

### In a Web Browser (no install required)

Open `src/index.html` directly in any modern browser. The CDN scripts for Plotly.js and math.js will load automatically (requires internet access).

```bash
# e.g. with a simple local server
npx serve src
# then open http://localhost:3000
```

### As a Desktop App (Electron)

```bash
# Install dependencies
npm install

# Launch in development mode
npm start
```

---

## Building the Desktop App

### Windows (.exe installer + portable)

```bash
npm run build:win
```

Output files appear in `dist/`:
- `Mathematics Curve Visualizer Setup x.x.x.exe` — NSIS installer
- `Mathematics Curve Visualizer x.x.x.exe` — portable executable

### Linux (AppImage)

```bash
npm run build:linux
```

### macOS (DMG)

```bash
npm run build:mac
```

### CI/CD via GitHub Actions

Push a tag matching `v*.*.*` (e.g. `v1.0.0`) to trigger the automated build workflow. Windows EXE and Linux AppImage artifacts will be uploaded to the GitHub Actions run.

```bash
git tag v1.0.0
git push origin v1.0.0
```

You can also trigger a build manually from the **Actions** tab using *workflow_dispatch*.

---

## Equation Input Formats

| Type | Format | Example |
|---|---|---|
| 2D Explicit | `y=<expr>` or `f(x)=<expr>` | `y=sin(x)` |
| 2D Parametric | `x=<expr>, y=<expr>` | `x=cos(t), y=sin(t)` |
| 3D Parametric | `x=<expr>, y=<expr>, z=<expr>` | `x=cos(t), y=sin(t), z=t/(2*pi)` |
| 3D Surface | `z=<expr>` | `z=sin(sqrt(x^2+y^2))` |

### Supported Functions

`sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `exp`, `log`, `log2`, `log10`, `sqrt`, `abs`, `ceil`, `floor`, `round`, `sign`, `factorial`

### Constants

| Name | Value |
|---|---|
| `pi` | π ≈ 3.14159 |
| `e` | e ≈ 2.71828 |
| `tau` | 2π ≈ 6.28318 |
| `phi` | φ ≈ 1.61803 (golden ratio) |

### Power Operator

Both `x^2` and `x**2` are accepted.

---

## Built-in Examples

**2D** — Sine Wave, Parabola, Damped Sine, Sinc Function, Cubic, Absolute Value, Unit Circle, Ellipse, Lissajous, Spiral, Butterfly Curve

**3D** — Helix, Trefoil Knot, Torus Knot, 3D Spiral, Ripple Surface, Saddle, Paraboloid, Wavy Surface

---

## Tech Stack

| Technology | Purpose |
|---|---|
| **HTML/CSS/JavaScript** | UI and application logic (no framework) |
| **[Plotly.js](https://plotly.com/javascript/)** (CDN) | Interactive 2D/3D plotting |
| **[math.js](https://mathjs.org/)** (CDN) | Safe math expression evaluation |
| **[Electron](https://www.electronjs.org/)** | Desktop packaging (Windows, Linux, macOS) |
| **[electron-builder](https://www.electron.build/)** | Cross-platform installer generation |
| **GitHub Actions** | CI/CD for automated EXE and AppImage builds |

---

## Project Structure

```
visualizer-mathematics/
├── main.js                   # Electron entry point
├── package.json              # Dependencies and build config
├── src/
│   ├── index.html            # Main application page
│   ├── styles.css            # Dark-theme stylesheet
│   ├── parser.js             # Equation string parser
│   ├── plotter.js            # Point generation and Plotly trace builder
│   └── app.js                # Application controller and state management
└── .github/
    └── workflows/
        └── build.yml         # CI/CD workflow for Windows EXE and Linux AppImage
```

---

## License

MIT
