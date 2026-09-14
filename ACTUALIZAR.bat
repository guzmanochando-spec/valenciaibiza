@echo off
chcp 65001 >nul
cd /d "%~dp0"
echo Actualizando ValenciaIbiza desde GitHub...
git pull origin main
if errorlevel 1 (
  echo.
  echo Si pide usuario, entra con GitHub. Si falla git, abre Git Bash en esta carpeta y escribe: git pull origin main
  pause
  exit /b 1
)
echo.
echo Listo. Abriendo el proyecto Unreal...
start "" "ValenciaIbiza.uproject"
