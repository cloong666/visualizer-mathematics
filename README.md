# Mathematics Visualizer — cpp-1.1

> **Branch:** `visualizer-mathematics/cpp-1.1` → PR target: `cpp/version`

A C++ mathematics curve visualizer with an interactive **SFML** GUI.  
This release (1.1) migrates the project to a pure C++ + SFML stack and removes all legacy non-C++ code.

---

## Migration Summary

### What was removed

| Item | Reason |
|------|--------|
| `curve_visualizer.html` | HTML/JavaScript prototype; superseded by SFML GUI |
| `src/CurveData.h` | Qt-specific data structure |
| `src/Plot2DWidget.*` | Qt 2-D canvas widget |
| `src/Plot3DWidget.*` | Qt 3-D canvas widget |
| `src/MainWindow.*` | Qt main window |
| `src/main.cpp` (Qt entry point) | Qt application entry point |
| `.github/workflows/build-windows.yml` | Qt CI workflow |

### What remains / was added

| Item | Description |
|------|-------------|
| `src/ExprParser.h/.cpp` | Shared recursive-descent expression parser (C++, no dependencies) |
| `sfml/` | SFML interactive 2-D function plotter (window, axes, curves, pan/zoom) |
| `cpp/` | Pure C++ core library (`vm_core`), CLI demo, and unit tests |
| `.github/workflows/build-sfml-windows.yml` | SFML CI workflow (Windows / MSVC x64) |

---

## SFML Math Visualizer

An interactive 2-D function plotter built with **SFML 2.6**.  
No Qt installation required. The font is embedded in the binary.

### Features

| Interaction | How |
|---|---|
| **Pan view** | Hold left mouse button + drag |
| **Zoom in / out** | Mouse scroll wheel |
| **Switch function** | `←` / `→` arrow keys (or `A` / `D`) |
| **Reset view** | Press `R` |
| **Quit** | Press `Esc` or close window |

### Built-in functions (12)

`sin(x)` · `cos(x)` · `tan(x)` · `x²` · `x³−3x` · `sin(x)/x` ·
`exp(−x²)` · `√|x|` · `1/(1+x²)` · `x·sin(x)` · `sin(3x)·cos(2x)` · `floor(x)`

---

## Dependencies

| Tool | Version | Notes |
|------|---------|-------|
| C++ compiler | MSVC 2019/2022, GCC ≥ 9, or Clang ≥ 10 | C++17 required |
| CMake | ≥ 3.16 | |
| SFML | 2.6 | Downloaded automatically via FetchContent if not installed |
| Git | any | Required for FetchContent |

---

## Build & Run

### Windows (MSVC)

```powershell
# Open "Developer Command Prompt for VS 2019/2022" (x64 Native)

git clone https://github.com/cloong666/visualizer-mathematics.git
cd visualizer-mathematics

# Configure – SFML is downloaded automatically if not found
cmake -B build -S . `
  -G "NMake Makefiles" `
  -DCMAKE_BUILD_TYPE=Release `
  -DBUILD_SFML_VISUALIZER=ON `
  -DBUILD_CPP_CORE=OFF

cmake --build build --target MathVisualizer

# Run
build\MathVisualizer.exe
```

### Linux / macOS

```bash
# Install SFML system package (optional – CMake will fetch it otherwise)
# Ubuntu/Debian: sudo apt install libsfml-dev
# macOS:         brew install sfml

cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SFML_VISUALIZER=ON \
  -DBUILD_CPP_CORE=OFF

cmake --build build --target MathVisualizer

./build/MathVisualizer
```

### Build only the pure C++ core + tests (no GUI, no SFML)

```bash
cmake -S cpp -B build_cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build_cpp
cd build_cpp && ctest --output-on-failure
```

---

## Project Structure

```
.
├── CMakeLists.txt                     # Unified build (SFML + cpp core)
├── README.md                          # This file
├── .github/
│   └── workflows/
│       └── build-sfml-windows.yml    # CI: SFML build (Windows / MSVC x64)
├── src/
│   ├── ExprParser.h / .cpp           # Recursive-descent expression parser (shared)
├── sfml/
│   ├── main.cpp                      # SFML entry point
│   ├── App.h / App.cpp               # Window lifecycle + input handling
│   ├── Renderer.h / Renderer.cpp     # Grid, axes, curve, HUD rendering
│   ├── ViewTransform.h               # World ↔ screen coordinate mapping
│   ├── MathFunctions.h               # Built-in function catalogue
│   └── resources/
│       └── FontData.h                # Embedded DejaVuSans font (binary array)
└── cpp/
    ├── CMakeLists.txt                 # Standalone or included build
    ├── README.md                      # cpp module documentation
    ├── core/include/vm_core/          # Public headers (Sampler, Vec2, CoordMapper, …)
    ├── core/src/                      # Library implementation
    ├── cli/main.cpp                   # CLI demo (ASCII plot, CSV export)
    └── tests/                         # CTest unit tests
```

---

## CI / Downloadable artifact

A GitHub Actions workflow runs on every push / PR and:

1. Sets up MSVC 2022 (x64).
2. Configures CMake – SFML 2.6.2 is fetched automatically.
3. Builds `MathVisualizer.exe`.
4. Packages the exe + DLLs into `MathVisualizer-Windows-x64.zip`.
5. Uploads the result as a downloadable artifact.

To get the pre-built exe:
1. Go to the **Actions** tab.
2. Click the latest *SFML Math Visualizer – Windows Build* run.
3. Download `MathVisualizer-Windows-x64` artifact, extract, and run.

---

## Known Limitations (cpp-1.1)

- **Linux/macOS** – the code compiles with GCC/Clang but CI only covers Windows (MSVC). No platform-specific issues are known; if you encounter build problems, please open an issue.
- **SFML must be available at build time** (FetchContent handles this automatically).
- Only explicit single-variable functions `y = f(x)` are supported; parametric / implicit curves are planned for a later release.
- The embedded font (DejaVuSans) covers Latin/Greek characters; extended Unicode is not rendered.

---

## Future Plans

| Milestone | Scope |
|-----------|-------|
| **1.2** | Parametric curve sampling `(x(t), y(t))`; implicit curve approximation `F(x,y)=0` |
| **2.0** | 3-D types (`Vec3`), parametric surface sampling, perspective projection |
| **3.0** | Camera model, per-point coordinate labels, fully interactive parameter adjustment |

---

## Expression Parser

Both the SFML visualizer and the cpp core reuse the same hand-written recursive-descent parser (`src/ExprParser.h / .cpp`).

| Category | Tokens |
|----------|--------|
| Arithmetic | `+` `-` `*` `/` `^` |
| Functions | `sin cos tan asin acos atan sinh cosh tanh exp log ln log10 log2 sqrt cbrt abs ceil floor round sign min max pow` |
| Constants | `pi`, `e` |
| Variables | `x` (explicit) · `t` (parametric) |

---

## License

MIT – see [LICENSE](LICENSE) if present, or feel free to use this code as you see fit.
