#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path

SFML_EXE_NAME = "MathVisualizer.exe"
QT_EXE_NAME = "CurveVisualizer.exe"


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Check whether expected Windows executable exists in build output."
    )
    parser.add_argument(
        "--type",
        choices=("sfml", "qt"),
        default="sfml",
        help="Which executable to check (default: sfml).",
    )
    parser.add_argument(
        "--build-dir",
        default="build",
        help="Build directory that contains the exe file (default: build).",
    )
    args = parser.parse_args()

    exe_name = SFML_EXE_NAME if args.type == "sfml" else QT_EXE_NAME
    exe_path = Path(args.build_dir) / exe_name

    if exe_path.exists():
        print(f"FOUND: {exe_path}")
        return 0

    print(f"NOT FOUND: {exe_path}")
    print("Build the project first or download the Windows artifact from GitHub Actions.")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
