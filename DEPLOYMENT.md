# SDF_Geo 项目部署指南

本文档说明如何在新环境中快速部署 SDF_Geo 项目。

## 📦 项目依赖

本项目依赖以下库（全部通过vcpkg自动安装）：

- **GLFW** 3.x - 窗口和输入管理
- **GLAD** - OpenGL函数加载器
- **ImGui** 1.91.9 - GUI界面库（包含GLFW和OpenGL3绑定）

## 🚀 快速部署（推荐）

### 前置要求

在开始之前，确保系统已安装：

1. **Git** - [下载地址](https://git-scm.com/downloads)
2. **CMake** 3.10+ - [下载地址](https://cmake.org/download/)
3. **Visual Studio 2017+** 或其他C++编译器
   - 推荐 Visual Studio 2019/2022
   - 安装时选择"使用C++的桌面开发"工作负载

### 部署步骤

1. **克隆或复制项目到目标机器**
   ```bash
   # 如果使用git
   git clone <your-repo-url>
   cd SDF_2D
   
   # 或直接复制整个SDF_2D文件夹
   ```

2. **运行一键安装脚本**
   
   双击 `setup.bat` 或在命令行运行：
   ```bash
   .\setup.bat
   ```
   
   脚本会自动：
   - 检测并安装vcpkg（如果未安装）
   - 下载并安装GLFW
   - 下载并安装GLAD
   - 下载并安装ImGui（包含GLFW和OpenGL3绑定）
   - 配置vcpkg工具链路径

   ⏱️ 首次安装大约需要 2-5 分钟（取决于网络速度）

3. **编译并运行**
   
   双击 `run.bat` 或在命令行运行：
   ```bash
   .\run.bat
   ```
   
   这会自动编译项目并启动程序。

## 🔧 手动部署（高级）

如果自动脚本无法使用，可以手动部署：

### 1. 安装vcpkg

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

### 2. 安装依赖库

```bash
.\vcpkg install glfw3:x64-windows
.\vcpkg install glad:x64-windows
.\vcpkg install imgui[glfw-binding,opengl3-binding]:x64-windows
```

### 3. 配置并编译项目

```bash
cd <项目目录>
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=<vcpkg路径>/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

### 4. 运行程序

```bash
.\Release\SDF_Geo.exe
```

## 📁 项目文件说明

部署后的项目结构：

```
SDF_2D/
├── main.cpp              # 主程序源代码
├── CMakeLists.txt        # CMake配置文件
├── setup.bat            # 一键安装依赖脚本
├── build.bat            # 编译脚本
├── run.bat              # 一键运行脚本
├── README.md            # 项目说明文档
├── DEPLOYMENT.md        # 本部署指南
├── vcpkg/               # vcpkg包管理器（setup.bat会创建）
├── vcpkg_path.txt       # vcpkg路径配置（setup.bat会创建）
└── build/               # 编译输出目录（build.bat会创建）
    └── Release/
        └── SDF_Geo.exe  # 可执行文件
```

## 🌐 离线部署

如果目标机器无法访问互联网：

### 方案1：打包vcpkg

1. 在有网络的机器上先运行 `setup.bat`
2. 将整个项目文件夹（包括 `vcpkg/` 目录）打包
3. 在目标机器上解压
4. 直接运行 `build.bat` 或 `run.bat`

### 方案2：使用预编译库

1. 下载GLFW、GLAD和ImGui的预编译版本
2. 修改 `CMakeLists.txt` 指向本地库路径
3. 手动编译

## ⚠️ 常见问题

### Q: setup.bat报错找不到git
**A**: 安装Git并确保已添加到系统PATH环境变量

### Q: 编译时报错找不到库
**A**: 
1. 确保运行了 `setup.bat`
2. 检查 `vcpkg_path.txt` 文件是否存在
3. 尝试删除 `build/` 目录后重新编译

### Q: 运行时黑屏或闪退
**A**: 
1. 更新显卡驱动
2. 确保显卡支持OpenGL 3.3+
3. 在命令行运行程序查看错误信息

### Q: vcpkg下载速度慢
**A**: 
1. 使用代理或VPN
2. 配置vcpkg使用镜像源
3. 使用离线部署方案

## 🔄 更新项目

如果项目代码有更新：

1. 拉取最新代码：
   ```bash
   git pull
   ```

2. 检查是否有新的依赖：
   - 查看 `CMakeLists.txt` 是否有变化
   - 如有新依赖，重新运行 `setup.bat`

3. 重新编译：
   ```bash
   .\run.bat
   ```

## 📮 技术支持

如遇到部署问题，请提供以下信息：

- 操作系统版本
- Visual Studio版本
- CMake版本
- 错误信息截图或日志
- `setup.bat` 执行输出

## 📝 版本历史

- **v1.0** - 初始版本
  - 基础OpenGL渲染框架
  - 集成ImGui界面
  - "添加点"功能占位

---

**祝部署顺利！** 🎉
