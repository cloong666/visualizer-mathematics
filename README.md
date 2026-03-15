# 2D / 3D Curve Visualizer

This repository provides **two** standalone Windows desktop applications that visualise mathematical curves:

| App | Technology | EXE name |
|---|---|---|
| **SFML Math Visualizer** *(new)* | C++ + [SFML 2.6](https://www.sfml-dev.org/) | `MathVisualizer.exe` |
| Qt Curve Visualizer *(original)* | C++ + Qt 6 / Qt 5 | `CurveVisualizer.exe` |

(It's worth to point out that the code based on SFML could available more function)

---


## Exe availability check

This repository stores source code and does **not** include prebuilt `.exe` files by default.  
You can quickly check whether a local build already produced an exe:

```bash
python check_exe.py
python check_exe.py --type qt
```

Expected locations:

- `build/MathVisualizer.exe` (SFML)
- `build/CurveVisualizer.exe` (Qt)

## SFML Math Visualizer (Recommended – double-click to run)

A fully self-contained interactive 2D function plotter built with **SFML 2.6**.  
No Qt installation required. The font is embedded in the binary so no extra files are needed.

### What you can do

| Interaction | How |
|---|---|
| **Pan view** | Hold left mouse button + drag |
| **Zoom in / out** | Mouse scroll wheel |
| **Switch function** | `←` / `→` arrow keys (or `A` / `D`) |
| **Reset view** | Press `R` |
| **Quit** | Press `Esc` or close window |

### Built-in functions (12 total)

`sin(x)` · `cos(x)` · `tan(x)` · `x²` · `x³−3x` · `sin(x)/x` ·  
`exp(−x²)` · `√|x|` · `1/(1+x²)` · `x·sin(x)` · `sin(3x)·cos(2x)` · `floor(x)`

### Building the SFML visualizer on Windows

#### Prerequisites

| Tool | Version |
|---|---|
| C++ compiler | MSVC 2019 or 2022 (x64) |
| CMake | ≥ 3.16 |
| Git | any recent version |
| Internet access | required once (FetchContent downloads SFML 2.6.2 automatically) |

#### Steps

```powershell
# 1. Open a "Developer Command Prompt for VS 2019/2022" (x64 Native)

# 2. Clone the repo (if not already done)
git clone https://github.com/cloong666/visualizer-mathematics.git
cd visualizer-mathematics

# 3. Configure  – CMake will automatically download SFML via FetchContent
cmake -B build -S . `
  -G "NMake Makefiles" `
  -DCMAKE_BUILD_TYPE=Release `
  -DBUILD_QT_VISUALIZER=OFF `
  -DBUILD_SFML_VISUALIZER=ON

# 4. Build
cmake --build build --config Release --target MathVisualizer

# 5. The executable is at:
#      build\MathVisualizer.exe
#    Any required SFML DLLs are copied next to it automatically.
```

Double-click `build\MathVisualizer.exe` – a window will open showing the visualizer.

#### CMake options

| Option | Default | Description |
|---|---|---|
| `BUILD_SFML_VISUALIZER` | `ON` | Build `MathVisualizer.exe` |
| `BUILD_QT_VISUALIZER` | `ON` | Build the original Qt `CurveVisualizer.exe` |

---

## CI / Windows artifact

A GitHub Actions workflow (`.github/workflows/build-sfml-windows.yml`) runs on every push / pull request and:

1. Sets up MSVC 2022 (x64).
2. Configures CMake – SFML 2.6.2 is fetched automatically.
3. Builds `MathVisualizer.exe`.
4. Packages the exe + DLLs into `MathVisualizer-Windows-x64.zip`.
5. Uploads the result as a **downloadable artifact** named `MathVisualizer-Windows-x64`.

### Downloading the pre-built exe

1. Go to the **Actions** tab in this repository.
2. Click the latest successful *SFML Math Visualizer – Windows Build* workflow run.
3. Under **Artifacts**, click `MathVisualizer-Windows-x64` to download the ZIP.
4. Extract the ZIP and run `MathVisualizer.exe` — no installation required.

---

## Project structure

```
.
├── CMakeLists.txt                         # Unified build file (Qt + SFML targets)
├── README.md
├── .github/
│   └── workflows/
│       ├── build-windows.yml              # CI: Qt build
│       └── build-sfml-windows.yml         # CI: SFML build (new)
├── src/
│   ├── ExprParser.h / .cpp               # Recursive-descent expression parser (shared)
│   ├── CurveData.h                       # Qt curve data structures
│   ├── Plot2DWidget.h / .cpp             # Qt 2D canvas
│   ├── Plot3DWidget.h / .cpp             # Qt 3D canvas
│   ├── MainWindow.h / .cpp               # Qt main window
│   └── main.cpp                          # Qt entry point
└── sfml/
    ├── main.cpp                          # SFML entry point
    ├── App.h / App.cpp                   # Window lifecycle + input handling
    ├── Renderer.h / Renderer.cpp         # Grid, axes, curve, HUD rendering
    ├── ViewTransform.h                   # World↔screen coordinate mapping
    ├── MathFunctions.h                   # Built-in function catalogue
    └── resources/
        └── FontData.h                    # Embedded DejaVuSans font (binary array)
```

---

## Original Qt Curve Visualizer

### Features

#### Curve input modes

| Mode | Example input |
|---|---|
| **2D Explicit** `y = f(x)` | `sin(x)`, `x^2 + 1`, `exp(-x^2)` |
| **2D General Curve** `F(x,y)=0` | `x^2 + y^2 = 4`, `(x^2+y^2)^2 = 2*(x^2-y^2)` |
| **2D Parametric** `x(t), y(t)` | `cos(t)`, `sin(t)` |
| **3D Parametric** `x(t), y(t), z(t)` | `cos(t)`, `sin(t)`, `t/5` |
| **3D Surface** `z = f(x,y)` | `x^2 + y^2`, `x^2-y^2`, `sin(x)*cos(y)` |

#### Interaction

| Widget | Interaction |
|---|---|
| **2D canvas** | Left-drag: pan · Scroll: zoom · Double-click: reset view |
| **3D canvas** | Left-drag: rotate · Right-drag / Shift+drag: pan · Scroll: zoom · `W`/`A`/`S`/`D`: move camera · Double-click: reset |

### Building the Qt visualizer on Windows

```powershell
# Install Qt via the online installer or aqtinstall, then set Qt6_DIR

# Open "Developer Command Prompt for VS 2019/2022"

cmake -B build -S . -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH="C:\Qt\6.7.3\msvc2019_64" `
  -DBUILD_SFML_VISUALIZER=OFF

cmake --build build --config Release

# Deploy Qt DLLs
windeployqt --release build\CurveVisualizer.exe
```

---

## Expression parser

Both visualizers share the same hand-written recursive-descent parser (`src/ExprParser.h / .cpp`).

| Category | Tokens |
|---|---|
| Arithmetic | `+` `-` `*` `/` `^` |
| Functions | `sin cos tan asin acos atan sinh cosh tanh exp log ln log10 log2 sqrt cbrt abs ceil floor round sign min max pow` |
| Constants | `pi`, `e` |
| Variables | `x` (explicit) · `t` (parametric) |

---

## License

MIT – see [LICENSE](LICENSE) if present, or feel free to use this code as you see fit.
