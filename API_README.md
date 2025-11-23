# SDFProcessor - 简洁的几何处理API

## 🎯 设计目标

将复杂的SDF几何处理算法封装成简单易用的API，让外部调用者：
- ✅ **无需了解内部实现** - Marching Squares、线段连接、重采样等细节全部隐藏
- ✅ **简洁的接口** - 3行代码完成复杂操作
- ✅ **开箱即用** - 合理的默认配置
- ✅ **高性能** - 自动缓存优化

## 📁 文件结构

```
SDF_2D/
├── SDFProcessor.h        # 高层API接口（⭐ 主要使用）
├── SDFProcessor.cpp      # 高层API实现
├── API_USAGE.md          # 详细使用文档
├── example_usage.cpp     # 使用示例代码
├── sdf.h                 # 底层算法接口
└── sdf.cpp               # 底层算法实现
```

## 🚀 快速开始

### 最简单的例子
```cpp
#include "SDFProcessor.h"

// 1. 创建多边形
std::vector<Point2D> polygon = {
    {0.0f, 0.5f}, {-0.5f, -0.5f}, {0.5f, -0.5f}
};

// 2. 创建处理器
SDFProcessor processor;
processor.setPolygon(polygon);

// 3. 生成环形
RingResult ring = processor.generateRing(0.2f, 0.0f);

// 4. 渲染
if (ring.isValid) {
    glDrawArrays(GL_TRIANGLES, 0, ring.triangleVertices.size());
}
```

就这么简单！🎉

## 📚 完整功能

| 功能 | 方法 | 用途 |
|------|------|------|
| 设置多边形 | `setPolygon()` | 设置要处理的多边形 |
| 查询SDF值 | `querySDFValue()` | 查询任意点的距离值 |
| 提取等值线 | `extractContour()` | 提取指定阈值的轮廓 |
| 生成环形 | `generateRing()` | 生成两轮廓间的三角网格 |
| 获取边界 | `getBounds()` | 获取多边形包围盒 |

## 🎨 API对比

### 使用SDFProcessor（推荐）✅
```cpp
SDFProcessor processor;
processor.setPolygon(polygon);
RingResult ring = processor.generateRing(0.2f, 0.0f);
// 3行代码，清晰简洁
```

### 直接使用底层函数 ❌
```cpp
calculatePolygonBounds(polygon, minX, maxX, minY, maxY);
minX -= padding; maxX += padding; minY -= padding; maxY += padding;
int width = (int)((maxX - minX) / gridSize) + 1;
int height = (int)((maxY - minY) / gridSize) + 1;
width = std::min(width, 512); height = std::min(height, 512);
float* sdfData = new float[width * height];
generateSDFGrid(width, height, minX, maxX, minY, maxY, polygon, sdfData);
std::vector<Point2D> outerSeg = extractIsoContour(sdfData, width, height, minX, maxX, minY, maxY, 0.2f);
std::vector<Point2D> innerSeg = extractIsoContour(sdfData, width, height, minX, maxX, minY, maxY, 0.0f);
std::vector<Point2D> outer = connectLineSegments(outerSeg);
std::vector<Point2D> inner = connectLineSegments(innerSeg);
std::vector<Triangle> tris = triangulateRing(outer, inner);
std::vector<Point2D> verts = trianglesToVertices(tris);
delete[] sdfData;
// 14行代码，需要了解所有细节，容易出错
```

## 💡 典型应用场景

### 1. 形状偏移
```cpp
processor.setPolygon(shape);
ContourResult offset = processor.extractContour(0.5f);
// 获得向外偏移0.5单位的形状
```

### 2. 碰撞检测
```cpp
float sdf = processor.querySDFValue(point);
if (sdf < 0) {
    // 碰撞！点在多边形内
}
```

### 3. 缓冲区可视化
```cpp
RingResult buffer = processor.generateRing(1.0f, 0.0f);
renderTriangles(buffer.triangleVertices);
// 显示1单位的缓冲区
```

### 4. 路径规划
```cpp
// 提取多层等值线作为导航路径
for (float d = 0.0f; d < 2.0f; d += 0.2f) {
    ContourResult path = processor.extractContour(d);
    // 使用path规划路径
}
```

## 🔧 配置选项

```cpp
SDFConfig config;
config.gridSize = 0.1f;      // 网格精度（越小越精细）
config.padding = 0.5f;       // 边界扩展范围
config.maxGridSize = 512;    // 最大分辨率
config.resamplePoints = 64;  // 重采样点数

SDFProcessor processor(config);
```

### 预设配置

```cpp
// 高质量（慢）
SDFConfig highQuality;
highQuality.gridSize = 0.05f;
highQuality.maxGridSize = 1024;
highQuality.resamplePoints = 128;

// 实时性能（快）
SDFConfig realtime;
realtime.gridSize = 0.2f;
realtime.maxGridSize = 256;
realtime.resamplePoints = 32;
```

## 📖 文档

- **`API_USAGE.md`** - 详细的API文档和使用指南
- **`example_usage.cpp`** - 6个完整的使用示例
- **`SDFProcessor.h`** - 头文件包含详细注释

## ⚡ 性能特性

### 智能缓存
```cpp
processor.setPolygon(polygon);

// 第一次调用：生成SDF网格（慢）
auto ring1 = processor.generateRing(0.2f, 0.0f);

// 后续调用：使用缓存（快）
auto ring2 = processor.generateRing(0.3f, 0.1f);
auto contour = processor.extractContour(0.15f);
```

### 内存管理
- 自动管理SDF网格内存
- 更换多边形时自动清理缓存
- 无内存泄漏

## 🎓 学习路径

1. **快速开始** - 复制上面的最简例子运行
2. **运行示例** - 编译并运行 `example_usage.cpp`
3. **阅读文档** - 查看 `API_USAGE.md`
4. **集成项目** - 在你的项目中使用 `SDFProcessor`

## 🔄 从底层API迁移

如果你已经在使用底层函数：

| 底层函数 | SDFProcessor方法 |
|---------|-----------------|
| `calculatePolygonSDF()` | `querySDFValue()` |
| `extractIsoContour()` + `connectLineSegments()` | `extractContour()` |
| 多个函数组合 | `generateRing()` |
| `calculatePolygonBounds()` | `getBounds()` |

## 📦 编译

已自动添加到 `CMakeLists.txt`：
```cmake
add_executable(${PROJECT_NAME} 
    main.cpp
    sdf.cpp
    SDFProcessor.cpp  # ← 已添加
)
```

## ✅ 优势总结

| 特性 | SDFProcessor | 直接使用底层 |
|------|-------------|------------|
| 代码量 | ⭐⭐⭐⭐⭐ (3行) | ⭐ (10+行) |
| 易用性 | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| 安全性 | ⭐⭐⭐⭐⭐ (自动) | ⭐⭐⭐ (手动) |
| 性能 | ⭐⭐⭐⭐⭐ (缓存) | ⭐⭐⭐⭐ |
| 维护性 | ⭐⭐⭐⭐⭐ | ⭐⭐ |

## 🎯 总结

**`SDFProcessor` 是完美的几何处理API封装：**

- 🎨 **简洁** - 最少3行代码完成任务
- 🚀 **高效** - 自动优化和缓存
- 🛡️ **安全** - 完善的错误处理
- 📚 **文档** - 详细的使用指南
- 💪 **强大** - 支持所有常见操作

**推荐在所有新项目中使用！** ✨

---

如有问题，请查看：
- 📖 `API_USAGE.md` - 详细文档
- 💡 `example_usage.cpp` - 代码示例
- 💻 `SDFProcessor.h` - API参考
