#!/usr/bin/env python3
import os
import sys
import subprocess
import webbrowser
from pathlib import Path

# ANSI color codes
class Colors:
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    CYAN = '\033[96m'
    MAGENTA = '\033[95m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def print_error(msg):
    print(f"{Colors.RED}{Colors.BOLD}[ERROR]{Colors.RESET} {msg}")

def print_success(msg):
    print(f"{Colors.GREEN}{Colors.BOLD}[SUCCESS]{Colors.RESET} {msg}")

def print_info(msg):
    print(f"{Colors.CYAN}[INFO]{Colors.RESET} {msg}")

def run_command(command, folder):
    process = subprocess.Popen(command, shell=True, cwd=folder)
    process.communicate()
    if process.returncode != 0:
        sys.exit(1)

def main():
    print_info("Starting WebAssembly Build...")
    
    # Get project root (parent of scripts folder)
    script_dir = Path(__file__).parent.resolve()
    project_root = script_dir.parent.resolve()
    
    build_folder = project_root / "build-web"
    
    # 1. Locate Emscripten SDK
    emsdk_folder = os.environ.get("EMSDK") or str(Path.home() / "emsdk-main")
    toolchain_file = Path(emsdk_folder) / "upstream" / "emscripten" / "cmake" / "Modules" / "Platform" / "Emscripten.cmake"
    
    if not toolchain_file.exists():
        print_error("Active Emscripten SDK could not be found.")
        sys.exit(1)

    # 2. Clean build folder
    if build_folder.exists():
        cache_file = build_folder / "CMakeCache.txt"
        if cache_file.exists():
            cache_file.unlink()
    else:
        build_folder.mkdir(parents=True, exist_ok=True)

    # 3. Generate build files
    print_info("Configuring project targets...")
    cmake_cmd = f'emcmake cmake "{project_root}" -DCMAKE_TOOLCHAIN_FILE="{toolchain_file}" -DPLATFORM=Web -DCMAKE_C_FLAGS="-Wno-tautological-compare" -DCMAKE_CXX_FLAGS="-Wno-tautological-compare"'
    run_command(cmake_cmd, build_folder)

    # 5. Compile
    print_info("Compiling project code...")
    run_command("cmake --build .", build_folder)

    print("\n" + "=" * 56)
    print_success("BUILD COMPLETE")
    print("=" * 56 + "\n")

    # 6. Open browser
    webbrowser.open("http://localhost:8000/raylib_imgui_hybrid.html")

    # 7. Start server
    print_info("Starting web server - Press Ctrl+C to stop")
    server_cmd = f'{sys.executable} -m http.server 8000 --directory "{build_folder}"'
    run_command(server_cmd, build_folder)

if __name__ == "__main__":
    main()