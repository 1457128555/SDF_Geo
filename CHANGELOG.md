# 更新日志 (Changelog)

所有重要的项目变更都会记录在这个文件中。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，
项目遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

## [Unreleased]

### 计划添加
- 3D SDF支持
- GPU加速SDF生成
- 更多几何图元支持（圆、椭圆等）
- 导出SVG功能
- Python绑定

## [1.0.0] - 2025-01-XX

### 首次发布 🎉

#### 核心功能 ✨
- SDF生成和计算
  - CPU端精确计算多边形的有向距离场
  - 点到线段最短距离计算
  - Winding Number算法判断点在多边形内外
- 等值线提取
  - Marching Squares算法实现
  - 自动连接无序线段形成闭合轮廓
- 环形区域三角化
  - 弧长均匀重采样算法
  - Triangle Strip高效三角化
  - 支持填充和线框两种渲染模式

#### API设计 💻
- `SDFProcessor` 高层API类
  - `setPolygon()` - 设置多边形
  - `querySDFValue()` - 查询SDF值
  - `extractContour()` - 提取等值线
  - `generateRing()` - 生成环形
  - `getBounds()` - 获取边界
- 结果结构体
  - `ContourResult` - 轮廓提取结果
  - `RingResult` - 环形生成结果
  - `SDFConfig` - 配置参数
- 智能缓存机制
  - 自动缓存SDF网格
  - 避免重复计算

#### 可视化Demo 🎨
- OpenGL 3.3+ 实时渲染
- ImGui交互界面
  - 相机控制（缩放、平移）
  - SDF可视化开关
  - 参数实时调整
  - 多种渲染模式
- 动态网格和坐标轴
- 颜色渐变映射
- 交互式点添加

#### 文档 📖
- 详细的README
- 完整的API使用文档 (API_USAGE.md)
- API快速入门 (API_README.md)
- 可运行的示例代码 (example_usage.cpp)
- 贡献指南 (CONTRIBUTING.md)
- GitHub发布指南 (GITHUB_SETUP.md)

#### 构建系统 🔧
- CMake 3.10+ 支持
- vcpkg依赖管理
- Windows批处理脚本
  - `setup.bat` - 自动安装依赖
  - `build.bat` - 自动编译
  - `run.bat` - 一键运行
- C++11标准

#### 性能 ⚡
- SDF生成: ~1-10ms (512x512网格)
- 等值线提取: ~2-5ms
- 环形三角化: ~1-3ms
- 实时60+ FPS

---

## 版本说明

### 版本号格式
版本号遵循 `主版本号.次版本号.修订号` (MAJOR.MINOR.PATCH):

- **主版本号**: 不兼容的API修改
- **次版本号**: 向下兼容的功能性新增
- **修订号**: 向下兼容的问题修正

### 变更类型
- `Added` - 新增功能
- `Changed` - 功能变更
- `Deprecated` - 即将废弃的功能
- `Removed` - 已移除的功能
- `Fixed` - Bug修复
- `Security` - 安全问题修复

---

## 未来规划

### v1.1.0 (计划中)
- [ ] 添加更多几何图元支持
- [ ] 优化SDF网格生成性能
- [ ] 增加导出功能（PNG、SVG）
- [ ] 添加更多示例

### v1.2.0 (计划中)
- [ ] GPU加速支持
- [ ] 3D SDF初步支持
- [ ] 批处理模式

### v2.0.0 (远期)
- [ ] 完整的3D SDF支持
- [ ] Python/C# 绑定
- [ ] 插件系统

---

## 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md)。

所有贡献者会在每个版本的Release Notes中被提及。

---

**注意**: 日期格式为 YYYY-MM-DD

[Unreleased]: https://github.com/1457128555/SDF_2D/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/1457128555/SDF_2D/releases/tag/v1.0.0
