# Import the ESP-IDF environment into the CURRENT PowerShell session, so
# `idf.py build / flash / monitor` works in CLion's (or any) terminal.
# Usage:  . .\espidf_source.ps1
cmd /c "call C:\Espressif\frameworks\esp-idf-v5.5.3\export.bat >nul 2>&1 && set" |
  ForEach-Object {
    if ($_ -match '^([^=]+)=(.*)$') { Set-Item -Path "env:$($Matches[1])" -Value $Matches[2] }
  }
Write-Host "ESP-IDF environment loaded (IDF_PATH=$env:IDF_PATH)"
