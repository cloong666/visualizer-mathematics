# C++ 2D Curve Visualizer — `cpp/`

> **Branch:** `visualizer-mathematics/cpp-1.0`  
> **PR target:** `visualizer-mathematics/cpp`  
> **Milestone:** 1.0 – 2D curve visualization (function sampling → ASCII/CSV output)

---

## Series Roadmap

| Series | Branch | Scope |
|--------|--------|-------|
| **1.~** | `cpp-1.0`, `cpp-1.1`, … | 2D curve visualization – sampling, coordinate mapping, ASCII/CSV output |
| **2.~** | `cpp-2.0`, `cpp-2.1`, … | 3D content – parametric surfaces, perspective projection |
| **3.~** | `cpp-3.0`, … | Camera movement, per-point coordinate labels, interactive display |

> **Design philosophy:** Pure C++ is preferred over heavy GUI frameworks.
> Where effect quality must trade off against C++ purity (e.g., no OpenGL),
> we accept the reduced fidelity and note it in the relevant section below.

---

## What's New in 1.0

| Feature | Module | Status |
|---------|--------|--------|
| Function sampling `y = f(x)` | `Sampler.h / .cpp` | ✅ (from `cpp` base) |
| 2-D vector arithmetic | `Vec2.h` | ✅ (from `cpp` base) |
| **Coordinate mapping** (math ↔ screen) | `CoordMapper.h` | ✅ **new** |
| **ASCII terminal plot** | `AsciiPlot.h / .cpp` | ✅ **new** |
| **CSV export** | `CsvExport.h` | ✅ **new** |
| Unit tests – Sampler | `test_sampler.cpp` | ✅ (from `cpp` base) |
| Unit tests – CoordMapper / AsciiPlot / CSV | `test_coord_mapper.cpp` | ✅ **new** |
| CTest integration | `CMakeLists.txt` | ✅ |

### Known Limitations (1.0)

- **ASCII rendering only** – colour, anti-aliasing, and smooth curves are not available without a GUI library.  This is intentional (C++ purity over visual quality).
- Single-variable explicit functions `y = f(x)` only; parametric / implicit curves are planned for `cpp-1.1`.
- No interactive input – all parameters are hard-coded in `cli/main.cpp` or passed via API.

---

## Directory Layout

```
cpp/
├── CMakeLists.txt               # Standalone or included build
├── README.md                    # This file
├── core/
│   ├── include/
│   │   └── vm_core/
│   │       ├── Sampler.h        # Function sampling API
│   │       ├── Vec2.h           # 2-D vector type
│   │       ├── CoordMapper.h    # Math ↔ screen linear coordinate mapping
│   │       ├── AsciiPlot.h      # Terminal ASCII curve renderer
│   │       └── CsvExport.h      # CSV export helper (header-only)
│   └── src/
│       ├── Sampler.cpp          # Sampler implementation
│       └── AsciiPlot.cpp        # AsciiPlot implementation
├── cli/
│   └── main.cpp                 # Demo: sampling → ASCII plot → CSV export
└── tests/
    ├── test_sampler.cpp         # Unit tests: Sampler + Vec2
    └── test_coord_mapper.cpp    # Unit tests: CoordMapper, AsciiPlot, CsvExport
```

The implementation reuses `src/ExprParser.h / .cpp` (the hand-written recursive-descent parser already present in the repository) – no additional dependencies are required.

---

## Build Requirements

| Tool | Version |
|------|---------|
| C++ compiler | GCC ≥ 9, Clang ≥ 10, or MSVC 2019/2022 |
| CMake | ≥ 3.16 |

No external libraries are needed.

---

## Configure & Build

### Option A — standalone (build only the C++ core)

```bash
# from the repo root
cmake -S cpp -B build_cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build_cpp
```

### Option B — unified build (all targets)

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_QT_VISUALIZER=OFF \
  -DBUILD_SFML_VISUALIZER=OFF \
  -DBUILD_CPP_CORE=ON
cmake --build build --target vm_core vm_cli test_vm_core test_vm_coord
```

Both options produce the same binaries; only the output directory differs.

---

## Run the CLI demo

```bash
# After a standalone build (Option A):
./build_cpp/vm_cli
```

The demo exercises all 1.0 features in sequence:

1. Samples `sin(x)` over `[-π, π]` and prints raw points.
2. Samples `sqrt(x)` over `[-2, 4]`, skipping invalid (complex) values.
3. Demonstrates `Vec2` arithmetic.
4. Shows `CoordMapper` math→screen conversion.
5. Renders `sin(x)` as an **ASCII plot** in the terminal.
6. Exports `x²` sample data to `x_squared.csv`.

---

## Run the tests

```bash
cd build_cpp && ctest --output-on-failure
```

Expected:
```
1/2 Test #1: vm_core_tests  ...  Passed
2/2 Test #2: vm_coord_tests ...  Passed
100% tests passed, 0 tests failed out of 2
```

Run individual test executables for verbose output:

```bash
./build_cpp/test_vm_core    # Sampler + Vec2 tests
./build_cpp/test_vm_coord   # CoordMapper + AsciiPlot + CsvExport tests
```

---

## Implemented Scope (1.0)

| Feature | Status |
|---------|--------|
| Expression parser (`ExprParser`) | ✅ Reused from `src/` |
| Function sampling (`vm::sampleFunction`) | ✅ |
| 2-D vector arithmetic (`vm::Vec2`) | ✅ |
| Coordinate mapping (`vm::CoordMapper`) | ✅ |
| ASCII terminal plot (`vm::asciiPlot`, `vm::asciiPlotAuto`) | ✅ |
| CSV export (`vm::exportCsv`) | ✅ |
| Unit tests – Sampler / Vec2 | ✅ |
| Unit tests – CoordMapper / AsciiPlot / CsvExport | ✅ |
| CTest integration | ✅ |

---

## Next Steps

### 1.1 (planned)
- Parametric curve sampling `(x(t), y(t))`
- Implicit curve approximation `F(x, y) = 0`
- SVG / PNG file export (using a lightweight header-only library)

### 2.0 (planned)
- 3-D point / vector types (`Vec3`)
- Parametric surface sampling `(x(u,v), y(u,v), z(u,v))`
- Perspective projection utilities

### 3.0 (planned)
- Camera model (position, look-at, field of view)
- Per-point coordinate label overlay
- Interactive parameter adjustment (keyboard / file-driven)

---

## Incremental Architecture

```
cpp/core (vm_core library)           ← dependency-free, always buildable
  └── Sampler, Vec2                  ← cpp base branch
  └── CoordMapper, AsciiPlot, Csv   ← cpp-1.0  (this PR)
  └── Parametric, Implicit samplers ← cpp-1.1
  └── Vec3, Surface samplers        ← cpp-2.0
  └── Camera, Labels                ← cpp-3.0

cpp/cli (vm_cli executable)          ← demo / smoke-test for the current core
cpp/tests                            ← self-contained assertion tests (CTest)
```

All later additions should be placed in new subdirectories under `cpp/` (e.g., `cpp/render/`, `cpp/app/`) and registered as new CMake targets, so that `vm_core` remains a lean, dependency-free library.
