@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo    GitHub 发布准备脚本
echo ========================================
echo.

REM 创建docs目录结构
echo [1/6] 创建文档目录结构...
if not exist "docs" mkdir docs
if not exist "docs\screenshots" mkdir docs\screenshots
echo     ✓ 目录创建完成

REM 移动demo.mp4
echo.
echo [2/6] 检查demo.mp4文件...
if exist "demo.mp4" (
    move demo.mp4 docs\demo.mp4 >nul
    echo     ✓ demo.mp4 已移动到 docs 目录
) else if exist "docs\demo.mp4" (
    echo     ✓ demo.mp4 已在 docs 目录中
) else (
    echo     ⚠ 未找到 demo.mp4 文件
    echo     请手动将演示视频放入 docs 目录
)

REM 备份并替换README
echo.
echo [3/6] 更新README文件...
if exist "README_GITHUB.md" (
    if exist "README.md" (
        copy README.md README_OLD.md >nul
        echo     ✓ 原README已备份为 README_OLD.md
    )
    copy README_GITHUB.md README.md >nul
    echo     ✓ 已使用GitHub版本的README
    del README_GITHUB.md >nul
    echo     ✓ 清理临时文件
) else (
    echo     ⓘ README_GITHUB.md 不存在，跳过
)

REM 检查必要文件
echo.
echo [4/6] 检查必要文件...
set "missing_files="

if not exist "LICENSE" (
    set "missing_files=!missing_files! LICENSE"
)
if not exist ".gitignore" (
    set "missing_files=!missing_files! .gitignore"
)
if not exist "CONTRIBUTING.md" (
    set "missing_files=!missing_files! CONTRIBUTING.md"
)
if not exist "CHANGELOG.md" (
    set "missing_files=!missing_files! CHANGELOG.md"
)

if defined missing_files (
    echo     ⚠ 缺少以下文件: !missing_files!
) else (
    echo     ✓ 所有必要文件都存在
)

REM 检查Git仓库
echo.
echo [5/6] 检查Git仓库...
if exist ".git" (
    echo     ✓ Git仓库已初始化
) else (
    echo     ⓘ Git仓库未初始化
    echo.
    set /p init_git="是否初始化Git仓库? (Y/N): "
    if /i "!init_git!"=="Y" (
        git init
        echo     ✓ Git仓库初始化完成
    )
)

REM 显示文件结构
echo.
echo [6/6] 当前项目结构:
echo.
tree /F /A | findstr /v "build vcpkg_installed .vs bin obj" | findstr /v ".exe .dll .lib .obj"
echo.

REM 完成
echo ========================================
echo    准备完成！
echo ========================================
echo.
echo 📋 下一步操作:
echo.
echo 1. 检查 docs\demo.mp4 是否存在
echo 2. (可选) 在 docs\screenshots\ 添加截图
echo 3. 更新 README.md 中的个人信息:
echo    - GitHub用户名
echo    - 作者名字
echo    - 联系方式
echo 4. 更新 LICENSE 中的版权信息
echo.
echo 5. Git 提交:
echo    git add .
echo    git status
echo    git commit -m "Initial commit: SDF 2D Geometry Processor"
echo.
echo 6. 推送到GitHub:
echo    git remote add origin https://github.com/YOUR_USERNAME/SDF_2D.git
echo    git branch -M main
echo    git push -u origin main
echo.
echo 📖 详细说明请查看: GITHUB_SETUP.md
echo.

pause
