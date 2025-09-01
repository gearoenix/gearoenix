#!/usr/bin/env python3

import os
import re
import subprocess
import sys
import multiprocessing
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

script_dir = Path(os.path.dirname(os.path.abspath(__file__)))

submodules_dir = script_dir / 'submodules'
submodules_dir_str = str(submodules_dir)

build_dir = script_dir / 'build'
build_dir_str = str(build_dir)


def print_fatal(msg):
    print(f"\033[91m Error: {msg} \033[0m ", file=sys.stderr)
    sys.exit(1)


def find_clang_format():
    try:
        subprocess.run(['clang-format-20', '--version'], capture_output=True, text=True)
        print("Found clang-format-20")
        return 'clang-format-20'
    except:
        try:
            result = subprocess.run(['clang-format', '--version'], capture_output=True, text=True)
            version_match = re.search(r'version (\d+)', result.stdout)
            if version_match and int(version_match.group(1)) >= 20:
                print(f"Found clang-format with version: {result.stdout}")
                return 'clang-format'
            else:
                print_fatal("clang-format version must be 20 or higher")
        except:
            print_fatal("No suitable clang-format found")
    return None


def is_cpp_file(path):
    return path.suffix in {'.cpp', '.hpp', '.cppm', '.hppm', '.mm'}


def is_not_ignored(path):
    p = str(path)
    return submodules_dir_str not in p and build_dir_str not in p


def format_file(args):
    path, formatter = args
    print(f"Formatting: {path}")
    subprocess.run([formatter, '-i', str(path)], check=True)


def main():
    clang_format = find_clang_format()
    cpp_files = []
    all_files = script_dir.rglob('*')

    for path in all_files:
        if not path.is_file():
            continue
        if not is_cpp_file(path):
            continue
        if not is_not_ignored(path):
            continue
        cpp_files.append(path)

    if not cpp_files:
        print("No C++ files found")
        return

    cpp_files.sort()

    with ThreadPoolExecutor(max_workers=int((multiprocessing.cpu_count() * 3) / 2)) as executor:
        executor.map(format_file, [(f, clang_format) for f in cpp_files])


if __name__ == '__main__':
    main()
