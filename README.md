# 🎨 SDF 2D Geometry Processor

<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-11-blue.svg)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3+-green.svg)](https://www.opengl.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)](https://www.microsoft.com/windows)

**一个基于Signed Distance Field的2D几何处理库和可视化Demo**

[演示视频](#-演示) • [快速开始](#-快速开始) • [API文档](API_USAGE.md) • [示例代码](example_usage.cpp)

</div>

---

## 📖 简介

SDF 2D Geometry Processor 是一个强大而易用的2D几何处理工具，提供：

- 🎯 **Signed Distance Field (SDF)** 生成和查询
- 📏 **等值线提取** - 基于Marching Squares算法
- 🔄 **环形区域三角化** - Triangle Strip方法
- 🎨 **实时可视化** - OpenGL + ImGui交互界面
- 🚀 **简洁API** - 隐藏实现细节，3行代码完成复杂操作

## ✨ 主要特性

### 核心功能

| 功能 | 描述 |
|------|------|
| **SDF计算** | CPU端精确计算多边形的有向距离场 |
| **点查询** | 快速查询任意点到多边形的距离和内外关系 |
| **等值线提取** | 提取指定SDF阈值的轮廓线 |
| **形状偏移** | 生成向内/向外偏移的多边形 |
| **环形填充** | 将两个轮廓之间的区域三角化 |
| **实时渲染** | 支持填充和线框两种渲染模式 |

### 算法实现

- ✅ **点到线段距离** - 精确的几何计算
- ✅ **点在多边形内判定** - Winding Number算法
- ✅ **Marching Squares** - ISO轮廓提取
- ✅ **线段连接算法** - 贪心连接无序线段
- ✅ **弧长重采样** - 均匀分布采样点
- ✅ **Triangle Strip三角化** - 高效环形网格生成

## 🎬 演示

<div align="center">

### 功能展示

[查看完整演示视频](docs/demo.mp4)

<table>
  <tr>
    <td align="center"><b>SDF可视化</b></td>
    <td align="center"><b>等值线提取</b></td>
    <td align="center"><b>环形填充</b></td>
  </tr>
  <tr>
    <td><img src="docs/screenshots/sdf_visualization.png" width="250" alt="SDF可视化"/></td>
    <td><img src="docs/screenshots/iso_contour.png" width="250" alt="等值线"/></td>
    <td><img src="docs/screenshots/ring_fill.png" width="250" alt="环形填充"/></td>
  </tr>
</table>

**主要功能:**
- 🎨 SDF纹理可视化（颜色渐变映射）
- 📐 动态网格和坐标轴
- 🔍 相机缩放和平移
- 🎛️ 实时参数调整
- 🖱️ 交互式点添加

</div>

## 🚀 快速开始

### 方法1: 使用脚本（推荐）⚡

```bash
# 1. 克隆仓库
git clone https://github.com/1457128555/SDF_2D.git
cd SDF_2D

# 2. 安装依赖（首次运行）
setup.bat

# 3. 编译并运行
run.bat
```

**就这么简单！** 🎉

### 方法2: 手动构建

#### 前置要求

- **CMake** 3.10+
- **C++11** 编译器
- **vcpkg** (推荐) 或手动安装依赖
- **OpenGL** 3.3+

#### 安装依赖

```bash
# 使用vcpkg安装依赖
vcpkg install glfw3:x64-windows
vcpkg install glad:x64-windows
vcpkg install imgui[glfw-binding,opengl3-binding]:x64-windows
```

#### 编译

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg路径]/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

#### 运行

```bash
.\Release\SDF_Geo.exe
```

## 💻 使用示例

### 基础用法

```cpp
#include "SDFProcessor.h"

// 创建多边形
std::vector<Point2D> polygon = {
    {0.0f, 0.5f}, {-0.5f, -0.5f}, {0.5f, -0.5f}
};

// 创建处理器
SDFProcessor processor;
processor.setPolygon(polygon);

// 查询SDF值
float sdf = processor.querySDFValue({0.0f, 0.0f});

// 提取等值线
ContourResult contour = processor.extractContour(0.0f);

// 生成环形
RingResult ring = processor.generateRing(0.2f, 0.0f);

// 渲染
if (ring.isValid) {
    glDrawArrays(GL_TRIANGLES, 0, ring.triangleVertices.size());
}
```

### 更多示例

- 📚 [完整API文档](API_USAGE.md)
- 💡 [使用示例代码](example_usage.cpp)
- 📖 [API快速参考](API_README.md)

## 🎮 交互界面

### 操作说明

| 操作 | 功能 |
|------|------|
| **鼠标左键** | 添加多边形顶点 |
| **鼠标右键拖拽** | 平移视图 |
| **鼠标滚轮** | 缩放视图 |
| **ESC** | 退出程序 |

### UI控件

- 🎛️ **相机控制** - 缩放、平移、重置
- 🎨 **SDF可视化** - 开关、边距、渐变阈值
- 📏 **等值线** - 开关、阈值调整
- 🔄 **环形填充** - 开关、内外阈值、渲染模式（填充/线框）
- 🗑️ **清空功能** - 清空所有点重新开始

## 📁 项目结构

```
SDF_2D/
├── docs/                    # 文档和演示资源
│   ├── demo.mp4            # 演示视频
│   └── screenshots/        # 截图
├── src/                    # 核心源代码（可选组织）
│   ├── main.cpp           # Demo应用主程序
│   ├── sdf.h              # 底层SDF算法接口
│   ├── sdf.cpp            # 底层算法实现
│   ├── SDFProcessor.h     # 高层API接口 ⭐
│   └── SDFProcessor.cpp   # 高层API实现 ⭐
├── API_USAGE.md           # 详细API使用文档
├── API_README.md          # API快速入门
├── example_usage.cpp      # 可运行的示例代码
├── CMakeLists.txt         # CMake构建配置
├── setup.bat              # 自动安装依赖脚本
├── build.bat              # 自动编译脚本
├── run.bat                # 一键运行脚本
├── .gitignore            # Git忽略文件
├── LICENSE               # MIT许可证
└── README.md             # 本文件
```

## 🔧 技术栈

| 技术 | 版本 | 用途 |
|------|------|------|
| **C++** | 11+ | 核心语言 |
| **OpenGL** | 3.3+ | 图形渲染 |
| **GLFW** | 3.x | 窗口管理 |
| **GLAD** | - | OpenGL加载器 |
| **ImGui** | 1.91+ | GUI界面 |
| **CMake** | 3.10+ | 构建系统 |

## 📊 性能

- **SDF生成**: ~1-10ms (512x512网格)
- **等值线提取**: ~2-5ms (Marching Squares)
- **环形三角化**: ~1-3ms (64采样点)
- **帧率**: 60+ FPS (实时交互)

## 🎓 学习资源

### 算法理论

- [Signed Distance Fields](https://iquilezles.org/articles/distfunctions2d/) - Inigo Quilez
- [Marching Squares](https://en.wikipedia.org/wiki/Marching_squares) - Wikipedia
- [Triangle Strip](https://www.khronos.org/opengl/wiki/Primitive#Triangle_primitives) - OpenGL Wiki

### 实现参考

- [LearnOpenGL CN](https://learnopengl-cn.github.io/) - OpenGL教程
- [ImGui Documentation](https://github.com/ocornut/imgui) - GUI库文档
- [GLM Mathematics](https://github.com/g-truc/glm) - 数学库（可选）

## 📝 应用场景

- 🎮 **游戏开发** - 碰撞检测、路径规划
- 🗺️ **地图处理** - 边界偏移、缓冲区分析
- 🎨 **图形设计** - 形状偏移、轮廓提取
- 🤖 **机器人** - 安全区域规划
- 📐 **CAD/CAM** - 刀具路径补偿

## 🤝 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详情。

### 开发流程

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 📜 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情

## 👨‍💻 作者

- **Fan** - *Initial work*

## 🙏 致谢

- [Dear ImGui](https://github.com/ocornut/imgui) - 优秀的GUI库
- [GLFW](https://www.glfw.org/) - 跨平台窗口库
- [Inigo Quilez](https://iquilezles.org/) - SDF技术分享

## 📞 联系方式

-  Issues: [GitHub Issues](https://github.com/1457128555/SDF_2D/issues)

---

<div align="center">

**如果这个项目对你有帮助，请给个 ⭐ Star！**

Made with ❤️ and C++

</div>
