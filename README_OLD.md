# SDF_Geo项目

这是一个使用C++和OpenGL的SDF（Signed Distance Field）几何渲染项目。

## 项目结构

```
SDF_2D/
├── main.cpp          # 主程序文件
├── CMakeLists.txt    # CMake构建配置
├── setup.bat         # 自动安装依赖脚本
├── build.bat         # 自动编译脚本
├── run.bat           # 一键编译运行脚本
└── README.md         # 说明文档
```

## 前置要求

在开始之前，请确保已安装以下工具：

1. **Git** - [下载安装](https://git-scm.com/downloads)
2. **CMake** - [下载安装](https://cmake.org/download/)
3. **Visual Studio** 或其他C++编译器
   - Visual Studio 2017或更高版本（推荐）
   - 或 MinGW-w64

> **提示**: 如果已安装这些工具，可直接跳到快速开始部分。

## 🚀 快速开始（推荐）

**最简单的方式 - 只需3步：**

1. **安装依赖**
   - 双击运行 `setup.bat`
   - 脚本会自动下载并安装vcpkg和所有依赖库

2. **编译项目**
   - 双击运行 `build.bat`
   - 或者跳过此步，直接运行下一步

3. **运行程序**
   - 双击运行 `run.bat`
   - 如果未编译，会自动编译后运行

**就这么简单！无需手动输入任何命令！**

### 脚本说明

- **`setup.bat`** - 首次使用必须运行，用于安装依赖库
  - 自动检测或安装vcpkg
  - 自动安装GLFW、GLAD和ImGui
  - 保存配置信息供后续使用

- **`build.bat`** - 编译项目
  - 自动配置CMake
  - 自动编译Release版本
  - 显示详细的编译信息和错误提示

- **`run.bat`** - 一键运行
  - 自动检测是否已编译
  - 可选择是否重新编译
  - 自动启动程序

## 依赖项

- **OpenGL** 3.3+
- **GLFW** 3.x - 窗口和输入管理
- **GLAD** - OpenGL函数加载器
- **ImGui** 1.91.9 - 即时模式GUI库（包含GLFW和OpenGL3绑定）

## 安装依赖

### 方法1: 使用vcpkg (推荐)

1. 安装vcpkg:
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

2. 安装依赖库:
```bash
.\vcpkg install glfw3:x64-windows
.\vcpkg install glad:x64-windows
.\vcpkg install imgui[glfw-binding,opengl3-binding]:x64-windows
```

### 方法2: 手动安装

1. **GLFW**: 从 [https://www.glfw.org/download.html](https://www.glfw.org/download.html) 下载预编译版本
2. **GLAD**: 从 [https://glad.dav1d.de/](https://glad.dav1d.de/) 生成并下载
   - Language: C/C++
   - Specification: OpenGL
   - API gl: Version 3.3+
   - Profile: Core
3. **ImGui**: 从 [https://github.com/ocornut/imgui](https://github.com/ocornut/imgui) 下载源码
   - 需要手动配置backend文件（不推荐，建议使用vcpkg）

## 编译项目

### 使用vcpkg

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg路径]/scripts/buildsystems/vcpkg.cmake ..
cmake --build .
```

### 手动配置

```bash
mkdir build
cd build
cmake -DGLFW_DIR=[GLFW路径] -DGLAD_DIR=[GLAD路径] ..
cmake --build .
```

## 运行程序

编译成功后，在`build`目录下运行可执行文件：

```bash
.\SDF_Geo.exe
```

## 功能说明

- 创建一个1200x800的窗口
- 集成ImGui界面，提供交互式控制面板
- "添加点"按钮用于添加SDF几何点
- 使用OpenGL渲染SDF几何图形
- 按ESC键退出程序

## 技术栈

- **渲染**: OpenGL 3.3 Core Profile
- **窗口管理**: GLFW 3.x
- **OpenGL加载**: GLAD
- **GUI**: ImGui (Dear ImGui)
- **语言**: C++11

## 代码说明

### ImGui集成
项目使用ImGui提供用户界面：
```cpp
// 创建ImGui窗口
ImGui::Begin("Control Panel");

if (ImGui::Button("添加点"))
{
    // 添加点的逻辑
}

ImGui::End();
```

### 渲染流程
1. 初始化GLFW窗口和OpenGL上下文
2. 初始化ImGui和相关后端（GLFW + OpenGL3）
3. 在渲染循环中：
   - 处理用户输入
   - 创建ImGui界面元素
   - 渲染ImGui
   - 交换缓冲区

## 常见问题

### 编译错误: 找不到GLFW或GLAD

确保已正确安装依赖库，并在CMake配置时指定正确的路径。

### 运行时黑屏

检查OpenGL驱动是否支持3.3版本以上。

### 窗口无法创建

确保显卡驱动已更新到最新版本。

## 学习资源

- [LearnOpenGL CN](https://learnopengl-cn.github.io/)
- [OpenGL官方文档](https://www.opengl.org/documentation/)
- [GLFW文档](https://www.glfw.org/documentation.html)

## 许可证

本项目仅供学习使用。
