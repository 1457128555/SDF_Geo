@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul
echo ========================================
echo   SDF_Geo项目 - 编译脚本
echo ========================================
echo.

REM 检查vcpkg路径
if exist "vcpkg_path.txt" (
    set /p VCPKG_TOOLCHAIN=<vcpkg_path.txt
    echo [信息] 使用vcpkg工具链: %VCPKG_TOOLCHAIN%
) else (
    echo [警告] 未找到vcpkg_path.txt
    echo [提示] 请先运行 setup.bat 安装依赖
    echo.
    set /p continue="是否继续编译？(可能失败) [y/N]: "
    if /i not "!continue!"=="y" (
        exit /b 0
    )
    set VCPKG_TOOLCHAIN=
)

REM 创建build目录
if not exist "build" (
    echo [信息] 创建build目录...
    mkdir build
)

cd build

REM 配置CMake
echo.
echo [信息] 正在配置CMake...
if defined VCPKG_TOOLCHAIN (
    cmake -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" ..
) else (
    cmake ..
)

if !errorlevel! neq 0 (
    echo.
    echo [错误] CMake配置失败
    echo.
    echo 可能的原因：
    echo   1. 未安装CMake，请访问: https://cmake.org/download/
    echo   2. 未安装依赖库，请运行 setup.bat
    echo   3. 未安装Visual Studio或其他C++编译器
    echo.
    cd ..
    pause
    exit /b 1
)

REM 编译项目
echo.
echo [信息] 正在编译项目...
cmake --build . --config Release

if !errorlevel! neq 0 (
    echo.
    echo [错误] 编译失败
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo   编译成功！
echo ========================================
echo.
echo 可执行文件位置：
if exist "build\Release\SDF_Geo.exe" (
    echo   build\Release\SDF_Geo.exe
) else if exist "build\Debug\SDF_Geo.exe" (
    echo   build\Debug\SDF_Geo.exe
) else if exist "build\SDF_Geo.exe" (
    echo   build\SDF_Geo.exe
)
echo.
echo Next: Run the program
echo   - Double click run.bat
echo   - Or run the .exe file above
echo.
pause
exit /b 0
