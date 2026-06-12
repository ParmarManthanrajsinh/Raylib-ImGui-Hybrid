#!/usr/bin/env python3
import os
import sys
import subprocess
import webbrowser
from pathlib import Path

def run_command(command, folder):
    """Runs a terminal command inside a specific folder."""
    process = subprocess.Popen(command, shell=True, cwd=folder)
    process.communicate()
    if process.returncode != 0:
        sys.exit(1)

def main():
    print("🚀 Starting WebAssembly Build...")
    
    project_root = Path(__file__).parent.resolve()
    build_folder = project_root / "build-web"
    
    # 1. Locate the Emscripten SDK inside your home folder
    emsdk_folder = os.environ.get("EMSDK") or str(Path.home() / "emsdk-main")
    toolchain_file = Path(emsdk_folder) / "upstream" / "emscripten" / "cmake" / "Modules" / "Platform" / "Emscripten.cmake"
    
    if not toolchain_file.exists():
        print("❌ Error: Active Emscripten SDK could not be found.")
        sys.exit(1)

    # 2. Re-create a clean build folder
    if build_folder.exists():
        cache_file = build_folder / "CMakeCache.txt"
        if cache_file.exists():
            cache_file.unlink()
    else:
        build_folder.mkdir(parents=True, exist_ok=True)

    # 3. Detect system type to select standard Makefile generator (No Ninja required)
    generator = '-G "MinGW Makefiles"' if sys.platform.startswith("win32") else '-G "Unix Makefiles"'

    # 4. Generate the build files via CMake
    print("⚙️  Configuring project targets...")
    cmake_cmd = f'cmake "{project_root}" {generator} -DCMAKE_TOOLCHAIN_FILE="{toolchain_file}" -DPLATFORM=Web'
    run_command(cmake_cmd, build_folder)

    # 5. Compile the code using the universal CMake build abstraction
    print("🛠️  Compiling project code...")
    run_command("cmake --build .", build_folder)

    print("\n========================================================")
    print("✅ BUILD COMPLETE: Launching application...")
    print("========================================================\n")

    # 6. Open the system web browser directly to your generated HTML page
    webbrowser.open("http://localhost:8000/raylib_imgui_hybrid.html")

    # 7. Start the local python distribution web server
    server_cmd = f'{sys.executable} -m http.server 8000 --directory "{build_folder}"'
    run_command(server_cmd, build_folder)

if __name__ == "__main__":
    main()
