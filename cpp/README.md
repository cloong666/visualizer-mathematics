# C++ Core Skeleton — `cpp/`

> **Branch:** `visualizer-mathematics/cpp`  
> **Target:** Pure C++ migration – minimal viable core + CLI demo + unit tests

---

## Goal

This directory establishes the foundation for porting the project to pure C++:

| Layer | Target |
|-------|--------|
| Core library (`vm_core`) | Expression-based function sampling, 2-D vector math |
| CLI demo (`vm_cli`) | Headless "smoke test" – prints sampled points to stdout |
| Unit tests (`test_vm_core`) | Self-contained assertion program, registered with CTest |

GUI / rendering work (SFML, Qt, OpenGL …) is **out of scope** for this branch and lives in separate feature branches (`cpp-1.0`, `cpp-1.1`, `cpp-2.0`, …).

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
│   │       └── Vec2.h           # 2-D vector type
│   └── src/
│       └── Sampler.cpp          # Sampler implementation
├── cli/
│   └── main.cpp                 # Demo: sample functions, print results
└── tests/
    └── test_sampler.cpp         # Unit tests (Sampler + Vec2)
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

### Option A — standalone (build only the C++ skeleton)

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
cmake --build build --target vm_core vm_cli test_vm_core
```

Both options produce the same binaries; only the output directory differs.

---

## Run the CLI demo

```bash
# After a standalone build (Option A):
./build_cpp/vm_cli

# After a unified build (Option B):
./build/vm_cli
```

Expected output (abbreviated):

```
╔══════════════════════════════════════════════════╗
║  vm_cli · Mathematics Visualizer Core Demo       ║
╚══════════════════════════════════════════════════╝

── Function sampling: sin(x) over [-π, π] (13 points) ──
  x = -3.14159   y = -0.00000
  x = -2.61799   y = -0.50000
  ...
  x =  3.14159   y =  0.00000

── Function sampling: sqrt(x) over [-2, 4] (invalid points skipped) ──
  4 valid point(s) out of 7 requested:
  ...

── Vec2 operations ──
  a           = (3.0000, 4.0000)
  ...
  |a|         = 5.0000   (expected 5.0)
  a · b       = 11.0000  (expected 11.0)
```

---

## Run the tests

```bash
# Standalone build:
cd build_cpp && ctest --output-on-failure

# Unified build:
cd build && ctest --output-on-failure
```

All tests should report **pass** and the process should exit with code 0.

You can also run the test executable directly for verbose output:

```bash
./build_cpp/test_vm_core
```

---

## Implemented Scope

| Feature | Status |
|---------|--------|
| Expression parser (`ExprParser`) | ✅ Reused from `src/` |
| Function sampling (`vm::sampleFunction`) | ✅ |
| 2-D vector arithmetic (`vm::Vec2`) | ✅ |
| Unit tests – Sampler | ✅ |
| Unit tests – Vec2 | ✅ |
| CTest integration | ✅ |

## Out-of-Scope (planned for later branches)

| Feature | Target branch |
|---------|---------------|
| Matrix / linear algebra utilities | `cpp-1.0` |
| Coordinate-system transforms | `cpp-1.0` |
| 2-D renderer (headless / SVG export) | `cpp-1.1` |
| Interactive GUI (SFML / ImGui) | `cpp-2.0` |
| 3-D surface sampling | `cpp-2.0` |
| Parametric curve sampling | `cpp-1.1` |

---

## Incremental Evolution

The skeleton follows a deliberate layered design:

```
cpp-0.x (this branch)  →  vm_core (Sampler, Vec2)  +  vm_cli (headless demo)
cpp-1.0                →  +Mat2/Mat3, CoordTransform, more samplers
cpp-1.1                →  +parametric / implicit curve samplers, SVG export
cpp-2.0                →  +interactive GUI layer (SFML or ImGui)
```

All later additions should be placed in new subdirectories under `cpp/` (e.g., `cpp/render/`, `cpp/app/`) and registered as new CMake targets, so that `vm_core` remains a lean, dependency-free library.
