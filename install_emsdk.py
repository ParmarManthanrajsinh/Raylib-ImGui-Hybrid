#!/usr/bin/env python3
import os
import shutil
import subprocess
import sys
from pathlib import Path


def run_command(command, folder):
    """Runs a terminal command inside a specific folder."""
    process = subprocess.Popen(command, shell=True, cwd=folder)
    process.communicate()
    if process.returncode != 0:
        print(f"[ERROR] Failed to execute command: {command}")
        sys.exit(1)


def main():
    print("[INFO] Starting Emscripten SDK Installation...")

    # Define where to install emsdk (your home folder)
    emsdk_folder = Path.home() / "emsdk-main"

    # 1. Check for Git
    if not shutil.which("git"):
        print(
            "[ERROR] Git is not installed on this computer. Please install Git and try again."
        )
        sys.exit(1)

    # 2. Check if folder exists and ask user before deleting
    should_clone = True
    if emsdk_folder.exists():
        print(f"[WARNING] Found an existing installation folder at {emsdk_folder}")
        user_choice = (
            input("Do you want to remove it and redownload cleanly? (y/n): ")
            .strip()
            .lower()
        )

        if user_choice in ["y", "yes"]:
            print("[INFO] Removing old installation folder...")
            shutil.rmtree(emsdk_folder, ignore_errors=True)
        else:
            print(
                "[INFO] Skipping download step. Using the existing repository folder."
            )
            should_clone = False

    # 3. Clone the official repository using Git (Split to prevent paste truncation errors)
    if should_clone:
        print("[INFO] Downloading Emscripten repository from GitHub...")
        part1 = "https://github.com"
        part2 = "emscripten-core/emsdk.git"
        run_command(f'git clone --depth 1 {part1}/{part2} "{emsdk_folder}"', None)

    # 4. Choose script name based on OS (Windows use .bat, Linux/Mac use ./)
    script_name = "emsdk.bat" if sys.platform.startswith("win32") else "./emsdk"

    # 5. Run the standard Emscripten setup commands
    print("[INFO] Installing and activating the latest compiler tools...")
    run_command(f"{script_name} install latest", emsdk_folder)
    run_command(f"{script_name} activate latest", emsdk_folder)

    print("\n========================================================")
    print("[SUCCESS] Emscripten SDK setup is complete!")
    print("========================================================")
    if sys.platform.startswith("win32"):
        print(
            f"To activate the environment in your current Windows command prompt, run:"
        )
        print(f"   cd {emsdk_folder}\n   emsdk_env.bat")
    else:
        print(f"To activate the environment in your current Linux terminal, run:")
        print(f"   source {emsdk_folder}/emsdk_env.sh")
    print("========================================================\n")


if __name__ == "__main__":
    main()
