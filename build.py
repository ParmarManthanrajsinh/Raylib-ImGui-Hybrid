#!/usr/bin/env python3
import os
import sys
import subprocess
import webbrowser
from pathlib import Path

# ANSI color codes for colored output
class Colors:
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def print_error(msg):
    print(f"{Colors.RED}{Colors.BOLD}ERROR:{Colors.RESET} {msg}")

def print_success(msg):
    print(f"{Colors.GREEN}{Colors.BOLD}SUCCESS:{Colors.RESET} {msg}")

def print_warning(msg):
    print(f"{Colors.YELLOW}{Colors.BOLD}WARNING:{Colors.RESET} {msg}")

def print_info(msg):
    print(f"{Colors.CYAN}{msg}{Colors.RESET}")

def print_step(msg):
    print(f"{Colors.MAGENTA}{msg}{Colors.RESET}")

def run_command(command, folder):
    """Runs a terminal command inside a specific folder."""
    process = subprocess.Popen(command, shell=True, cwd=folder)
    process.communicate()
    if process.returncode != 0:
        sys.exit(1)

def main():
    print_step("Starting WebAssembly Build...")
    
    project_root = Path(__file__).parent.resolve()
    build_folder = project_root / "build-web"
    
    # 1. Locate the Emscripten SDK inside your home folder
    emsdk_folder = os.environ.get("EMSDK") or str(Path.home() / "emsdk-main")
    toolchain_file = Path(emsdk_folder) / "upstream" / "emscripten" / "cmake" / "Modules" / "Platform" / "Emscripten.cmake"
    
    if not toolchain_file.exists():
        print_error("Active Emscripten SDK could not be found.")
        sys.exit(1)

    # 2. Re-create a clean build folder
    if build_folder.exists():
        cache_file = build_folder / "CMakeCache.txt"
        if cache_file.exists():
            print_warning("Removing old CMake cache...")
            cache_file.unlink()
    else:
        build_folder.mkdir(parents=True, exist_ok=True)

    # 3. Detect system type to select standard Makefile generator (No Ninja required)
    generator = '-G "MinGW Makefiles"' if sys.platform.startswith("win32") else '-G "Unix Makefiles"'

    # 4. Generate the build files via CMake
    print_info("Configuring project targets...")
    cmake_cmd = f'cmake "{project_root}" {generator} -DCMAKE_TOOLCHAIN_FILE="{toolchain_file}" -DPLATFORM=Web'
    run_command(cmake_cmd, build_folder)

    # 5. Compile the code using the universal CMake build abstraction
    print_info("Compiling project code...")
    run_command("cmake --build .", build_folder)

    print("\n" + "=" * 56)
    print_success("BUILD COMPLETE: Launching application...")
    print("=" * 56 + "\n")

    # 6. Open the system web browser directly to your generated HTML page
    webbrowser.open("http://localhost:8000/raylib_imgui_hybrid.html")

    # 7. Start the local python distribution web server
    print_warning("Starting web server - Press Ctrl+C to stop")
    server_cmd = f'{sys.executable} -m http.server 8000 --directory "{build_folder}"'
    run_command(server_cmd, build_folder)

if __name__ == "__main__":
    main()