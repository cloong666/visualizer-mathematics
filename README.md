# 2D / 3D Curve Visualizer

A desktop application written in **C++ / Qt** that renders mathematical curves in 2D and 3D Cartesian coordinate systems.  
Supports explicit functions, 2D parametric equations, and 3D parametric equations – all with an interactive, zoom-able, pan-able / rotate-able canvas.

---

## Screenshots

> _After building, launch the app and select any built-in example from the **Examples** dropdown to see it rendered immediately._

| 2D Explicit (`y = sin(x)`) | 2D Parametric (circle) | 3D Parametric (helix) |
|:-:|:-:|:-:|
| _(curve appears in 2D canvas)_ | _(circle drawn with pan/zoom)_ | _(helix with 3D rotation)_ |

---

## Features

### Curve input modes

| Mode | Example input |
|---|---|
| **2D Explicit** `y = f(x)` | `sin(x)`, `x^2 + 1`, `exp(-x^2)` |
| **2D Parametric** `x(t), y(t)` | `cos(t)`, `sin(t)` |
| **3D Parametric** `x(t), y(t), z(t)` | `cos(t)`, `sin(t)`, `t/5` |

### Supported expression syntax

| Category | Tokens |
|---|---|
| Arithmetic | `+`, `-`, `*`, `/`, `^` (right-assoc. power) |
| Grouping | `(`, `)` |
| Functions | `sin cos tan asin acos atan atan2 sinh cosh tanh` |
| | `exp log ln log10 log2 sqrt cbrt abs ceil floor round` |
| | `sign min max pow` |
| Constants | `pi`, `e` |
| Variables | `x` (explicit mode) · `t` (parametric modes) |

### Interaction

| Widget | Interaction |
|---|---|
| **2D canvas** | Left-drag: pan · Scroll: zoom · Double-click: reset view |
| **3D canvas** | Left-drag: rotate · Right-drag / Shift+drag: pan · Scroll: zoom · Double-click: reset |

### Error handling

* Parse errors → message shown in the log panel with the problematic token highlighted.
* Domain errors (e.g. `sqrt(-1)`, `log(0)`) → invalid points are **silently skipped**; the line is broken rather than crashing or drawing a garbage segment.
* `NaN` / `Inf` points are skipped individually; valid segments remain visible.

---

## Built-in examples

The **Examples** dropdown contains ready-to-run curves:

* **2D Explicit:** sine wave, parabola, cubic, Gaussian, damped sine, |cos(x)|
* **2D Parametric:** circle, ellipse, Lissajous figure, Archimedean spiral, rose
* **3D Parametric:** helix, conical helix, trefoil knot, Viviani curve

---

## Building from source

### Prerequisites

| Requirement | Version |
|---|---|
| C++ compiler | MSVC 2019+, GCC 11+, or Clang 13+ |
| CMake | ≥ 3.16 |
| Qt | 6.x (Widgets component) · Qt 5.15 also supported |

### Windows (MSVC + Qt 6)

```powershell
# 1. Install Qt via the online installer or aqtinstall, then set Qt6_DIR:
#    e.g. C:\Qt\6.7.3\msvc2019_64\lib\cmake\Qt6

# 2. Open a "Developer Command Prompt for VS 2019/2022"

# 3. Configure & build
cmake -B build -S . -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.7.3\msvc2019_64"
cmake --build build --config Release

# 4. Deploy Qt DLLs so the .exe can run standalone
windeployqt --release build\CurveVisualizer.exe

# 5. (Optional) Bundle into a ZIP
Compress-Archive -Path build\* -DestinationPath CurveVisualizer-Windows.zip
```

### Linux / macOS (GCC/Clang + Qt 6)

```bash
# Install Qt6 via your package manager, e.g.:
#   Ubuntu:  sudo apt install qt6-base-dev
#   macOS:   brew install qt

cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/CurveVisualizer
```

### Alternative: Qt5

If Qt6 is not available, Qt5 works identically:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/path/to/Qt/5.15.x/gcc_64"
cmake --build build
```

---

## CI / Windows artifact

A GitHub Actions workflow (`.github/workflows/build-windows.yml`) runs on every push / pull request and:

1. Installs Qt 6.7.3 (MSVC 64-bit) via `jurplel/install-qt-action`.
2. Builds with `NMake` under a MSVC Developer shell.
3. Runs `windeployqt` to bundle all required Qt DLLs.
4. Uploads the result as a **downloadable artifact** named `CurveVisualizer-Windows-x64`.

### Downloading the artifact

1. Go to the **Actions** tab in this repository.
2. Click the latest successful workflow run.
3. Under **Artifacts**, click `CurveVisualizer-Windows-x64` to download the ZIP.
4. Extract the ZIP and run `CurveVisualizer.exe` — no installation required.

### Creating a GitHub Release

To publish an official release:

```bash
# Tag the commit you want to release
git tag v1.0.0
git push origin v1.0.0
```

Then on GitHub go to **Releases → Draft a new release**, select the tag, and publish.  
The workflow will automatically attach the Windows ZIP to the release.

---

## Project structure

```
.
├── CMakeLists.txt                    # CMake build file
├── README.md
├── .github/
│   └── workflows/
│       └── build-windows.yml         # CI workflow
└── src/
    ├── main.cpp                      # Application entry point
    ├── ExprParser.h / .cpp           # Recursive-descent expression parser
    ├── CurveData.h                   # Shared data structures (Point2D/3D, Curve2D/3D, CurveStyle)
    ├── Plot2DWidget.h / .cpp         # 2D canvas (QPainter, pan/zoom)
    ├── Plot3DWidget.h / .cpp         # 3D canvas (QPainter + perspective projection, rotate/pan/zoom)
    └── MainWindow.h / .cpp           # Main window UI
```

---

## Expression parser details

The built-in parser is a hand-written **recursive-descent parser** that:

* Tokenises the input string character-by-character.
* Builds an **AST** (abstract syntax tree).
* Evaluates the tree for every sample point, substituting variable values at runtime.
* Reports **exact error positions** (e.g. "Unexpected character '?' at position 4") for syntax errors.
* Silently marks domain errors (division by zero, log of negative, sqrt of negative, invalid inverse-trig argument) as invalid so the renderer can skip those points.

No third-party expression-parser library is required.

---

## License

MIT – see [LICENSE](LICENSE) if present, or feel free to use this code as you see fit.
