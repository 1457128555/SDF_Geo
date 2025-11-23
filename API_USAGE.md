# SDFProcessor API 使用指南

## 📚 概述

`SDFProcessor` 是一个高层次的几何处理API，封装了所有SDF相关的底层算法实现。外部用户只需要调用简洁的接口，无需了解Marching Squares、线段连接、重采样等内部细节。

## 🎯 核心功能

- ✅ **SDF值查询** - 查询任意点的有向距离场值
- ✅ **等值线提取** - 从SDF场中提取指定阈值的轮廓
- ✅ **环形生成** - 生成两个等值线之间的三角化区域
- ✅ **自动缓存** - 智能缓存SDF网格数据，提高性能

## 📖 API参考

### 结构体

#### `SDFConfig` - 配置参数
```cpp
struct SDFConfig {
    float gridSize;      // 网格单元大小 (默认: 0.1)
    float padding;       // 多边形周围额外边距 (默认: 0.5)
    int maxGridSize;     // 最大网格维度 (默认: 512)
    int resamplePoints;  // 重采样目标点数 (默认: 64)
};
```

#### `ContourResult` - 轮廓提取结果
```cpp
struct ContourResult {
    std::vector<Point2D> orderedPoints;  // 有序轮廓点（闭合环）
    std::vector<Point2D> lineSegments;   // 线段用于渲染 (GL_LINES)
    bool isValid;                         // 是否提取成功
};
```

#### `RingResult` - 环形生成结果
```cpp
struct RingResult {
    std::vector<Point2D> triangleVertices;  // 三角形顶点 (GL_TRIANGLES)
    int triangleCount;                       // 三角形数量
    std::vector<Point2D> outerContour;      // 外轮廓点
    std::vector<Point2D> innerContour;      // 内轮廓点
    bool isValid;                            // 是否生成成功
};
```

### 类方法

#### 构造函数
```cpp
SDFProcessor(const SDFConfig& config = SDFConfig());
```

#### `setPolygon()` - 设置多边形
```cpp
bool setPolygon(const std::vector<Point2D>& polygon);
```
- **参数**: 多边形顶点数组（至少3个点）
- **返回**: 多边形是否有效

#### `querySDFValue()` - 查询SDF值
```cpp
float querySDFValue(const Point2D& point) const;
```
- **参数**: 查询点坐标
- **返回**: SDF值（负=内部，正=外部，0=边界）

#### `extractContour()` - 提取等值线
```cpp
ContourResult extractContour(float threshold);
```
- **参数**: SDF阈值（0.0 = 边界）
- **返回**: 轮廓提取结果

#### `generateRing()` - 生成环形
```cpp
RingResult generateRing(float outerThreshold, float innerThreshold);
```
- **参数**: 
  - `outerThreshold`: 外轮廓阈值（应大于innerThreshold）
  - `innerThreshold`: 内轮廓阈值
- **返回**: 环形生成结果

#### `getBounds()` - 获取包围盒
```cpp
void getBounds(float& outMinX, float& outMaxX, 
               float& outMinY, float& outMaxY) const;
```

## 💡 使用示例

### 示例1: 基本使用
```cpp
#include "SDFProcessor.h"

// 创建多边形
std::vector<Point2D> polygon = {
    {0.0f, 0.5f},   // 顶点
    {-0.5f, -0.5f}, // 左下
    {0.5f, -0.5f}   // 右下
};

// 创建处理器
SDFProcessor processor;
processor.setPolygon(polygon);

// 查询某点的SDF值
Point2D testPoint = {0.0f, 0.0f};
float sdf = processor.querySDFValue(testPoint);
std::cout << "SDF value at (0,0): " << sdf << std::endl;
```

### 示例2: 提取等值线
```cpp
// 提取边界轮廓（threshold = 0.0）
ContourResult boundary = processor.extractContour(0.0f);

if (boundary.isValid) {
    std::cout << "Boundary has " << boundary.orderedPoints.size() 
              << " points" << std::endl;
    
    // 渲染线段
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                 boundary.lineSegments.size() * sizeof(Point2D),
                 boundary.lineSegments.data(), 
                 GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, boundary.lineSegments.size());
}

// 提取外部偏移轮廓
ContourResult offset = processor.extractContour(0.2f);
if (offset.isValid) {
    // 处理偏移轮廓...
}
```

### 示例3: 生成环形区域
```cpp
// 生成外圈阈值0.2，内圈阈值0.0的环形
RingResult ring = processor.generateRing(0.2f, 0.0f);

if (ring.isValid) {
    std::cout << "Ring has " << ring.triangleCount 
              << " triangles" << std::endl;
    
    // 渲染填充三角形
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 ring.triangleVertices.size() * sizeof(Point2D),
                 ring.triangleVertices.data(),
                 GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, ring.triangleVertices.size());
}
```

### 示例4: 自定义配置
```cpp
// 创建自定义配置
SDFConfig config;
config.gridSize = 0.05f;     // 更精细的网格
config.padding = 1.0f;       // 更大的边距
config.maxGridSize = 1024;   // 更高的分辨率
config.resamplePoints = 128; // 更多的采样点

SDFProcessor processor(config);
processor.setPolygon(polygon);

// 后续使用processor...
```

### 示例5: 批量处理
```cpp
SDFProcessor processor;

// 处理多个多边形
std::vector<std::vector<Point2D>> polygons = { /* ... */ };

for (const auto& poly : polygons) {
    processor.setPolygon(poly);
    
    // 提取边界
    ContourResult boundary = processor.extractContour(0.0f);
    
    // 生成偏移
    RingResult ring = processor.generateRing(0.3f, 0.0f);
    
    // 处理结果...
}
```

## 🎨 渲染集成示例

### OpenGL渲染函数
```cpp
void renderContour(const ContourResult& contour) {
    if (!contour.isValid) return;
    
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glBufferData(GL_ARRAY_BUFFER,
                 contour.lineSegments.size() * sizeof(Point2D),
                 contour.lineSegments.data(),
                 GL_DYNAMIC_DRAW);
    
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, contour.lineSegments.size());
    glLineWidth(1.0f);
}

void renderRing(const RingResult& ring, bool wireframe = false) {
    if (!ring.isValid) return;
    
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glBufferData(GL_ARRAY_BUFFER,
                 ring.triangleVertices.size() * sizeof(Point2D),
                 ring.triangleVertices.data(),
                 GL_DYNAMIC_DRAW);
    
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    
    glDrawArrays(GL_TRIANGLES, 0, ring.triangleVertices.size());
    
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
```

## ⚡ 性能优化建议

### 1. 重用处理器实例
```cpp
// ✅ 好的做法
SDFProcessor processor;
for (const auto& poly : polygons) {
    processor.setPolygon(poly);
    // 处理...
}

// ❌ 避免
for (const auto& poly : polygons) {
    SDFProcessor processor;  // 重复创建
    processor.setPolygon(poly);
}
```

### 2. 多次提取时只设置一次多边形
```cpp
processor.setPolygon(polygon);

// SDF网格会被缓存，多次提取效率高
auto contour1 = processor.extractContour(0.0f);
auto contour2 = processor.extractContour(0.1f);
auto contour3 = processor.extractContour(0.2f);
```

### 3. 调整配置以平衡质量和性能
```cpp
// 高质量（慢）
SDFConfig highQuality;
highQuality.gridSize = 0.01f;
highQuality.maxGridSize = 2048;
highQuality.resamplePoints = 256;

// 实时性能（快）
SDFConfig realtime;
realtime.gridSize = 0.2f;
realtime.maxGridSize = 256;
realtime.resamplePoints = 32;
```

## 🔍 错误处理

```cpp
// 检查多边形有效性
if (!processor.isValid()) {
    std::cerr << "Invalid polygon!" << std::endl;
    return;
}

// 检查结果有效性
ContourResult contour = processor.extractContour(0.0f);
if (!contour.isValid) {
    std::cerr << "Failed to extract contour" << std::endl;
    return;
}

RingResult ring = processor.generateRing(0.2f, 0.0f);
if (!ring.isValid) {
    std::cerr << "Failed to generate ring" << std::endl;
    return;
}
```

## 📊 应用场景

### 1. 碰撞检测
```cpp
// 检查点是否在多边形内
float sdf = processor.querySDFValue(point);
if (sdf < 0) {
    // 点在内部
}
```

### 2. 形状偏移
```cpp
// 生成向外偏移的形状
ContourResult offset = processor.extractContour(0.5f);
```

### 3. 缓冲区可视化
```cpp
// 显示安全区域
RingResult buffer = processor.generateRing(0.5f, 0.0f);
renderRing(buffer, false);
```

### 4. 路径规划
```cpp
// 提取多层等值线作为导航路径
for (float threshold = 0.0f; threshold < 2.0f; threshold += 0.2f) {
    ContourResult path = processor.extractContour(threshold);
    // 使用path作为路径...
}
```

## 🎓 与底层API对比

### 使用SDFProcessor（推荐）
```cpp
// 简洁、清晰
SDFProcessor processor;
processor.setPolygon(polygon);
RingResult ring = processor.generateRing(0.2f, 0.0f);
```

### 直接使用底层函数（复杂）
```cpp
// 需要了解所有细节
float minX, maxX, minY, maxY;
calculatePolygonBounds(polygon, minX, maxX, minY, maxY);
minX -= padding; maxX += padding; // ...

int width = (int)((maxX - minX) / gridSize) + 1;
// ... 计算height

float* sdfData = new float[width * height];
generateSDFGrid(width, height, minX, maxX, minY, maxY, polygon, sdfData);

std::vector<Point2D> outerSegments = extractIsoContour(sdfData, /*...*/);
std::vector<Point2D> innerSegments = extractIsoContour(sdfData, /*...*/);

std::vector<Point2D> outerContour = connectLineSegments(outerSegments);
std::vector<Point2D> innerContour = connectLineSegments(innerSegments);

std::vector<Triangle> triangles = triangulateRing(outerContour, innerContour);
std::vector<Point2D> vertices = trianglesToVertices(triangles);

delete[] sdfData;
```

## 📝 总结

`SDFProcessor` 提供了一个简洁、易用的接口来处理基于SDF的几何操作：

- ✅ **简单** - 3行代码完成复杂操作
- ✅ **高效** - 自动缓存和优化
- ✅ **安全** - 完善的错误检查
- ✅ **灵活** - 可配置的参数
- ✅ **封装** - 隐藏实现细节

对于需要SDF功能的应用，直接使用`SDFProcessor`即可，无需关注底层算法实现！
