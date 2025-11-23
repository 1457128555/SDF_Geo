@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul
echo ========================================
echo   SDF_Geo项目 - 依赖安装脚本
echo   Dependencies: GLFW, GLAD, ImGui
echo ========================================
echo.

REM 检查vcpkg是否已安装
where vcpkg >nul 2>&1
if !errorlevel! neq 0 (
    echo [错误] 未找到vcpkg，正在检查本地vcpkg...
    
    if exist "vcpkg\vcpkg.exe" (
        echo [信息] 找到本地vcpkg
        set VCPKG_ROOT=%cd%\vcpkg
    ) else (
        echo.
        echo [提示] vcpkg未安装。请选择：
        echo   1. 自动下载并安装vcpkg到当前目录
        echo   2. 手动安装依赖库
        echo   3. 退出
        echo.
        set /p choice="请输入选择 (1/2/3): "
        
        if "!choice!"=="1" (
            echo.
            echo [信息] 正在克隆vcpkg...
            git clone https://github.com/Microsoft/vcpkg.git
            if !errorlevel! neq 0 (
                echo [错误] 克隆失败，请检查git是否已安装
                pause
                exit /b 1
            )
            
            echo [信息] 正在初始化vcpkg...
            cd vcpkg
            call bootstrap-vcpkg.bat
            if !errorlevel! neq 0 (
                echo [错误] vcpkg初始化失败
                cd ..
                pause
                exit /b 1
            )
            cd ..
            set VCPKG_ROOT=%cd%\vcpkg
        ) else if "!choice!"=="2" (
            echo.
            echo [信息] 请手动下载以下库：
            echo   - GLFW: https://www.glfw.org/download.html
            echo   - GLAD: https://glad.dav1d.de/
            echo.
            echo 下载后，修改CMakeLists.txt中的路径
            pause
            exit /b 0
        ) else (
            exit /b 0
        )
    )
) else (
    echo [信息] 检测到系统已安装vcpkg
    for /f "delims=" %%i in ('where vcpkg') do set VCPKG_ROOT=%%~dpi
    set VCPKG_ROOT=%VCPKG_ROOT:~0,-1%
)

echo.
echo [信息] vcpkg路径: %VCPKG_ROOT%
echo.

REM 安装依赖
echo [信息] 正在安装GLFW...
"%VCPKG_ROOT%\vcpkg.exe" install glfw3:x64-windows
set INSTALL_ERROR=%errorlevel%
if %INSTALL_ERROR% neq 0 (
    echo [错误] GLFW安装失败
    pause
    exit /b 1
)

echo.
echo [信息] 正在安装GLAD...
"%VCPKG_ROOT%\vcpkg.exe" install glad:x64-windows
set INSTALL_ERROR=%errorlevel%
if %INSTALL_ERROR% neq 0 (
    echo [错误] GLAD安装失败
    pause
    exit /b 1
)

echo.
echo [信息] 正在安装ImGui (包含GLFW和OpenGL3绑定)...
"%VCPKG_ROOT%\vcpkg.exe" install imgui[glfw-binding,opengl3-binding]:x64-windows
set INSTALL_ERROR=%errorlevel%
if %INSTALL_ERROR% neq 0 (
    echo [错误] ImGui安装失败
    pause
    exit /b 1
)

echo.
echo ========================================
echo   所有依赖安装完成！
echo ========================================
echo.
echo vcpkg工具链文件路径：
echo %VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
echo.
echo 此路径已保存到 vcpkg_path.txt
echo %VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake > vcpkg_path.txt
echo.
echo 现在可以运行 build.bat 编译项目
echo 或直接运行 run.bat 一键编译并运行
echo.
pause
