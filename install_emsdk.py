#!/usr/bin/env python3
import os
import sys
import shutil
import subprocess
from pathlib import Path

def run_command(command, folder):
    """Runs a terminal command inside a specific folder."""
    process = subprocess.Popen(command, shell=True, cwd=folder)
    process.communicate()
    if process.returncode != 0:
        print(f"❌ Failed: {command}")
        sys.exit(1)

def main():
    print("🚀 Starting Emscripten SDK Installation...")
    
    # Define where to install emsdk (your home folder)
    emsdk_folder = Path.home() / "emsdk-main"
    
    # 1. Check for Git
    if not shutil.which("git"):
        print("❌ Error: Git is not installed on this computer.")
        sys.exit(1)

    # 2. Clear old installations if they exist
    if emsdk_folder.exists():
        print("🧹 Removing old installation folder...")
        shutil.rmtree(emsdk_folder, ignore_errors=True)

    # 3. Clone the official repository using Git (Split to prevent paste truncation)
    print("📥 Downloading Emscripten repository...")
    part1 = "https://github.com"
    part2 = "emscripten-core/emsdk.git"
    run_command(f'git clone --depth 1 {part1}/{part2} "{emsdk_folder}"', None)

    # 4. Choose script name based on OS (Windows use .bat, Linux/Mac use ./)
    script_name = "emsdk.bat" if sys.platform.startswith("win32") else "./emsdk"

    # 5. Run the standard Emscripten setup commands
    print("🛠️  Installing latest compiler tools...")
    run_command(f"{script_name} install latest", emsdk_folder)
    run_command(f"{script_name} activate latest", emsdk_folder)

    print("\n========================================================")
    print("🎉 SUCCESS: Emscripten SDK is now installed!")
    print("========================================================")
    if sys.platform.startswith("win32"):
        print(f"💡 Run this in your Windows command prompt to activate it:")
        print(f"   cd {emsdk_folder}\n   emsdk_env.bat")
    else:
        print(f"💡 Run this in your Linux terminal to activate it:")
        print(f"   source {emsdk_folder}/emsdk_env.sh")
    print("========================================================\n")

if __name__ == "__main__":
    main()
