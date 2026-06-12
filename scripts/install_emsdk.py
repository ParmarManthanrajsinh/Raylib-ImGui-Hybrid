#!/usr/bin/env python3
import os
import shutil
import subprocess
import sys
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
    print(f"{Colors.RED}{Colors.BOLD}[ERROR]{Colors.RESET} {msg}")

def print_success(msg):
    print(f"{Colors.GREEN}{Colors.BOLD}[SUCCESS]{Colors.RESET} {msg}")

def print_warning(msg):
    print(f"{Colors.YELLOW}{Colors.BOLD}[WARNING]{Colors.RESET} {msg}")

def print_info(msg):
    print(f"{Colors.CYAN}[INFO]{Colors.RESET} {msg}")

def print_step(msg):
    print(f"{Colors.MAGENTA}{msg}{Colors.RESET}")

def run_command(command, folder):
    """Runs a terminal command inside a specific folder."""
    process = subprocess.Popen(command, shell=True, cwd=folder)
    process.communicate()
    if process.returncode != 0:
        print_error(f"Failed to execute command: {command}")
        sys.exit(1)

def main():
    print_step("Starting Emscripten SDK Installation...")
    print()

    # Define where to install emsdk (your home folder)
    emsdk_folder = Path.home() / "emsdk-main"

    # 1. Check for Git
    if not shutil.which("git"):
        print_error("Git is not installed on this computer. Please install Git and try again.")
        sys.exit(1)

    # 2. Check if folder exists and ask user before deleting
    should_clone = True
    if emsdk_folder.exists():
        print_warning(f"Found an existing installation folder at {emsdk_folder}")
        user_choice = input("Do you want to remove it and redownload cleanly? (y/n): ").strip().lower()

        if user_choice in ["y", "yes"]:
            print_info("Removing old installation folder...")
            shutil.rmtree(emsdk_folder, ignore_errors=True)
        else:
            print_info("Skipping download step. Using the existing repository folder.")
            should_clone = False

    # 3. Clone the official repository using Git
    if should_clone:
        print_info("Downloading Emscripten repository from GitHub...")
        part1 = "https://github.com"
        part2 = "emscripten-core/emsdk.git"
        run_command(f'git clone --depth 1 {part1}/{part2} "{emsdk_folder}"', None)

    # 4. Choose script name based on OS (Windows use .bat, Linux/Mac use ./)
    script_name = "emsdk.bat" if sys.platform.startswith("win32") else "./emsdk"

    # 5. Run the standard Emscripten setup commands
    print_info("Installing and activating the latest compiler tools...")
    run_command(f"{script_name} install latest", emsdk_folder)
    run_command(f"{script_name} activate latest", emsdk_folder)

    print()
    print("=" * 56)
    print_success("Emscripten SDK setup is complete!")
    print("=" * 56)
    
    if sys.platform.startswith("win32"):
        print(f"To activate the environment in your current Windows command prompt, run:")
        print(f"   {Colors.CYAN}cd {emsdk_folder}{Colors.RESET}")
        print(f"   {Colors.CYAN}emsdk_env.bat{Colors.RESET}")
    else:
        print(f"To activate the environment in your current Linux terminal, run:")
        print(f"   {Colors.CYAN}source {emsdk_folder}/emsdk_env.sh{Colors.RESET}")
    
    print("=" * 56)
    print()

if __name__ == "__main__":
    main()