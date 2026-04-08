@echo off
echo ========================================
echo 反辐射无人机仿真系统
echo ========================================
echo.

set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%build

echo 正在打开项目...
if exist "%BUILD_DIR%\SituationAnalysis.sln" (
    start "" "%BUILD_DIR%\SituationAnalysis.sln"
    echo ✓ 已打开 SituationAnalysis.sln
) else (
    echo ✗ 未找到解决方案文件
    echo 正在生成Visual Studio项目...
    if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
    cd "%BUILD_DIR%"
    cmake .. -G "Visual Studio 16 2019" -A x64
    if %ERRORLEVEL% EQU 0 (
        start "" "%BUILD_DIR%\SituationAnalysis.sln"
        echo ✓ 已生成并打开 SituationAnalysis.sln
    ) else (
        echo ✗ CMake生成失败
    )
)

echo.
pause
