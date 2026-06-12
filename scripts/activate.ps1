$emsdkPath = Join-Path $env:USERPROFILE "emsdk-main\emsdk_env.ps1"
if (Test-Path $emsdkPath) {
    . $emsdkPath
} else {
    Write-Host "[ERROR] Emscripten SDK not found at $emsdkPath" -ForegroundColor Red
    Write-Host "Please run scripts\install_emsdk.py first." -ForegroundColor Red
}
