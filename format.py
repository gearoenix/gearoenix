#!/usr/bin/env python3
import re
import shutil
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

CLANG_FORMAT_VERSION = 22
ROOT = Path(__file__).resolve().parent
IGNORED_DIRS = {'submodules', 'build', '.cxx', 'vcpkg_installed', '.git', 'node_modules'}
EXTS = {'.cpp', '.hpp', '.cppm', '.hppm', '.mm'}


def find_clang_format() -> str:
    for name in (f'clang-format-{CLANG_FORMAT_VERSION}', 'clang-format'):
        if path := shutil.which(name):
            out = subprocess.run(
                [path, '--version'], capture_output=True, text=True, check=True
            ).stdout
            if (m := re.search(r'version (\d+)', out)) and int(m.group(1)) >= CLANG_FORMAT_VERSION:
                print(f"Using {path}: {out.strip()}")
                return path
    sys.exit(f"\033[91mError: clang-format >= {CLANG_FORMAT_VERSION} not found\033[0m")


def main() -> None:
    fmt = find_clang_format()
    files: list[Path] = []
    for dirpath, dirnames, filenames in ROOT.walk():
        dirnames[:] = [d for d in dirnames if d not in IGNORED_DIRS]
        files.extend(dirpath / fn for fn in filenames if Path(fn).suffix in EXTS)
    files.sort()
    if not files:
        print("No C++ files found")
        return

    def run(p: Path) -> None:
        print(f"Formatting: {p}")
        subprocess.run([fmt, '-i', str(p)], check=True)

    with ThreadPoolExecutor() as ex:
        list(ex.map(run, files))


if __name__ == '__main__':
    main()
