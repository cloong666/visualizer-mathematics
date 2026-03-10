# Mathematics Curve Visualizer

A **2D/3D Cartesian coordinate curve visualizer** with two independent implementations:

1. **Web / Electron app** — HTML/CSS/JavaScript, Plotly.js, math.js (cross-platform, see below)
2. **Native Windows EXE (SFML)** — C++17 + SFML, interactive real-time math visualizer (see [SFML Visualizer](#sfml-interactive-math-visualizer-windows-exe) section)

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

Open `src/index.html` directly in any modern browser (the vendor libraries are bundled locally — no internet required):

```bash
# e.g. with a simple local server
npx serve src
# then open http://localhost:3000
```

Or simply double-click `src/index.html` in your file manager.

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

### Updating Vendor Libraries

The bundled Plotly.js and math.js files live in `src/vendor/`. To update them to newer versions, edit `package.json`, run `npm install`, then:

```bash
npm run vendor:update
```

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
| **[Plotly.js](https://plotly.com/javascript/)** (bundled locally) | Interactive 2D/3D plotting |
| **[math.js](https://mathjs.org/)** (bundled locally) | Safe math expression evaluation |
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
├── sfml-visualizer/          # Native Windows EXE (C++ + SFML)
│   ├── CMakeLists.txt        # CMake build (FetchContent SFML)
│   ├── assets/               # Runtime assets (font, auto-populated by CMake)
│   └── src/
│       ├── main.cpp          # Entry point
│       ├── App.hpp/.cpp      # Application loop & state
│       ├── Renderer.hpp/.cpp # SFML drawing (grid, axes, curves, HUD)
│       ├── InputHandler.hpp/.cpp  # Mouse/keyboard events
│       ├── MathFunctions.hpp/.cpp # Curve catalogue
│       └── Constants.hpp     # All configuration constants
└── .github/
    └── workflows/
        ├── build.yml         # CI/CD for Electron EXE / AppImage
        └── build-sfml.yml    # CI/CD for SFML Windows EXE
```

---

## SFML Interactive Math Visualizer (Windows EXE)

A standalone **C++ + SFML** application that opens a windowed, interactive math curve plotter when double-clicked — no install required.

### Features

- 10 built-in curves: `sin(x)`, `cos(x)`, `tan(x)`, `x²`, `x³−3x`, `sinc(x)`, damped sine, `|sin(x)|`, `ln(x)`, `√|x|`
- Real-time coordinate grid with adaptive spacing
- X / Y axes with tick labels
- **Mouse drag** — pan the view
- **Mouse scroll** — zoom in / out around the cursor
- **← / →** (or A / D) — cycle to previous / next curve
- **↑ / ↓** — adjust amplitude (±0.1 per step)
- **+ / −** — adjust frequency (±0.1 per step)
- **Space** — toggle between single-curve and all-curves overlay
- **R** — reset view to default zoom / position
- **Esc** — quit
- HUD with current formula, amplitude, frequency, zoom level, and control hints

### Prerequisites

| Tool | Minimum version | Notes |
|---|---|---|
| **CMake** | 3.16 | [cmake.org/download](https://cmake.org/download/) |
| **C++ compiler** | MSVC 2019 / MinGW-w64 / Clang | C++17 required |
| **Git** | any | CMake uses FetchContent (git clone) to download SFML |
| **Internet** | — | Required during first CMake configure to fetch SFML |

### Build (Windows)

```powershell
# From the repository root:
cmake -B build -S sfml-visualizer -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The output `EXE` is placed at:

```
build/Release/MathVisualizer.exe
build/Release/assets/          ← font files copied here automatically
```

> **First build note:** CMake will download SFML 2.6.1 (~40 MB) via git. Subsequent builds are fast.

### Run

Double-click `build/Release/MathVisualizer.exe` — no DLLs or installers needed (SFML is statically linked).

### CI / Pre-built Download

Every push to the feature branch builds the EXE automatically via GitHub Actions ([`.github/workflows/build-sfml.yml`](.github/workflows/build-sfml.yml)). Download the `MathVisualizer-Windows-x64` artifact from the Actions run.

### Known Limitations

- Windows only for now (Linux/macOS builds require OpenGL and suitable system libs — not configured in CI)
- Text rendering is disabled gracefully if no font is found (falls back to Windows system fonts automatically)
- `tan(x)` discontinuities are approximated by clamping large values rather than exact asymptote detection

---

## License

MIT
