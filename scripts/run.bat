@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul
echo ========================================
echo   SDF 2D Geometry Processor - Run Script
echo ========================================
echo.

REM 检查是否已编译
set EXE_PATH=
if exist "build\Release\SDF_Demo.exe" (
    set EXE_PATH=build\Release\SDF_Demo.exe
) else if exist "build\Debug\SDF_Demo.exe" (
    set EXE_PATH=build\Debug\SDF_Demo.exe
) else if exist "build\SDF_Demo.exe" (
    set EXE_PATH=build\SDF_Demo.exe
)

if defined EXE_PATH (
    echo [信息] 找到已编译的程序: %EXE_PATH%
    echo.
    set /p rebuild="是否重新编译？[y/N]: "
    if /i "!rebuild!"=="y" (
        goto BUILD
    ) else (
        goto RUN
    )
) else (
    echo [信息] 未找到已编译的程序，开始编译...
    goto BUILD
)

:BUILD
echo.
echo [步骤 1/2] 编译项目...
call scripts\build.bat
if !errorlevel! neq 0 (
    echo.
    echo [错误] 编译失败，无法运行程序
    pause
    exit /b 1
)

REM 重新检查可执行文件
set EXE_PATH=
if exist "build\Release\SDF_Demo.exe" (
    set EXE_PATH=build\Release\SDF_Demo.exe
) else if exist "build\Debug\SDF_Demo.exe" (
    set EXE_PATH=build\Debug\SDF_Demo.exe
) else if exist "build\SDF_Demo.exe" (
    set EXE_PATH=build\SDF_Demo.exe
)

if not defined EXE_PATH (
    echo.
    echo [错误] 编译完成但未找到可执行文件
    pause
    exit /b 1
)

:RUN
echo.
echo [步骤 2/2] 运行程序...
echo ========================================
echo.
echo 启动 SDF Demo 渲染程序...
echo 按 ESC 键退出程序
echo.
echo ========================================
echo.

REM 运行程序
"%EXE_PATH%"

echo.
echo ========================================
echo   程序已退出
echo ========================================
echo.
pause
