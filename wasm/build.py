#!/usr/bin/env python3
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


def eprint(*args, **kwargs):
    print("\x1b[31m", *args, "\x1b[0m", file=sys.stderr, **kwargs)


def require_cmd(cmd: str):
    if shutil.which(cmd) is None:
        eprint(f"error: required command not found in PATH: {cmd}")
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(
        prog="build.py",
        add_help=False,
        description="Build wasm target using CMake presets and optionally run local HTTP server.",
    )
    parser.add_argument("--build-type", choices=["debug", "release"], default="release")
    parser.add_argument("--no-server", action="store_true")
    parser.add_argument("--help", "-h", action="help", help="Show this help")
    args = parser.parse_args()

    build_type = args.build_type
    no_server = args.no_server

    # Resolve paths
    script_dir = Path(__file__).resolve().parent
    project_root = (script_dir / "..").resolve()

    # Command requirements
    require_cmd("cmake")
    require_cmd("ninja")

    # Python preference
    py_exe = shutil.which("python3") or shutil.which("python")
    if not py_exe:
        eprint("error: python3 (or python) is required but not found in PATH")
        sys.exit(1)

    # Required env vars
    vcpkg_root = os.environ.get("VCPKG_ROOT")
    if not vcpkg_root:
        eprint("VCPKG_ROOT environment variable is not set. Please install vcpkg and set VCPKG_ROOT.")
        sys.exit(1)

    emsdk = os.environ.get("EMSDK")
    if not emsdk:
        eprint("EMSDK environment variable is not set. Please install Emscripten and set EMSDK.")
        sys.exit(1)

    emsdk_script = Path(emsdk.rstrip("/")) / "emsdk_env.sh"
    if not emsdk_script.is_file():
        eprint(f"error: emsdk_env.sh not found at: {emsdk_script}")
        sys.exit(1)

    # Source emsdk_env.sh (capture exported env)
    # We run a shell that sources the script and then prints env as null-separated pairs.
    try:
        env_output = subprocess.check_output(
            ["bash", "-lc", f"source {shlex_quote(str(emsdk_script))} >/dev/null 2>&1; env -0"],
            text=False,
        )
    except subprocess.CalledProcessError:
        eprint("error: failed to source emsdk_env.sh")
        sys.exit(1)

    # Build a new environment for subsequent commands
    new_env = os.environ.copy()
    for entry in env_output.split(b"\x00"):
        if not entry:
            continue
        key, _, val = entry.partition(b"=")
        if key:
            new_env[key.decode("utf-8", errors="ignore")] = val.decode("utf-8", errors="ignore")

    # Ensure emcmake is available after sourcing
    if shutil.which("emcmake", path=new_env.get("PATH", "")) is None:
        eprint("error: emcmake not found after sourcing emsdk_env.sh")
        sys.exit(1)

    # Configure
    try:
        subprocess.check_call(
            ["cmake", "--fresh", "--preset", f"wasm-{build_type}", "."],
            cwd=str(project_root),
            env=new_env,
        )
    except subprocess.CalledProcessError:
        eprint("error: Failed to configure the project")
        sys.exit(1)

    # Compute threads
    try:
        nproc = os.cpu_count() or 4
    except Exception:
        nproc = 4
    threads = max(8, (nproc * 3 + 1) // 2)
    print(f"Threads count: {threads}")

    # Build
    try:
        subprocess.check_call(
            ["cmake", "--build", "--preset", f"wasm-{build_type}", "--parallel", str(threads)],
            cwd=str(project_root),
            env=new_env,
        )
    except subprocess.CalledProcessError:
        eprint("error: Failed to build the project")
        sys.exit(1)

    # Optional server
    if not no_server:
        try:
            subprocess.check_call(
                [py_exe, "wasm/run-server.py"],
                cwd=str(project_root),
                env=new_env,
            )
        except subprocess.CalledProcessError:
            eprint("error: Failed to run the server")
            sys.exit(1)


def shlex_quote(s: str) -> str:
    # Minimal quoting to safely embed paths in a shell command
    if not s:
        return "''"
    if all(c.isalnum() or c in "._-/:@" for c in s):
        return s
    return "'" + s.replace("'", "'\"'\"'") + "'"


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(130)
