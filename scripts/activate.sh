#!/bin/bash
if [ -f "$HOME/emsdk-main/emsdk_env.sh" ]; then
    source "$HOME/emsdk-main/emsdk_env.sh"
else
    echo -e "\033[91m[ERROR]\033[0m Emscripten SDK not found at $HOME/emsdk-main"
    echo "Please run python scripts/install_emsdk.py first."
fi
