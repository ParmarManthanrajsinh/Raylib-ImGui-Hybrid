@echo off
if exist "%USERPROFILE%\emsdk-main\emsdk_env.bat" (
    call "%USERPROFILE%\emsdk-main\emsdk_env.bat"
) else (
    echo [ERROR] Emscripten SDK not found at %USERPROFILE%\emsdk-main
    echo Please run scripts\install_emsdk.py first.
)
