# 🔧 Integration Guide - 集成指南

## 📦 将SDF库集成到你的项目

### 快速集成（推荐）

**只需复制2个目录：**

```
include/
src/
```

**复制到你的项目：**

```
your_project/
├── lib/sdf/
│   ├── include/
│   │   ├── sdf.h
│   │   └── SDFProcessor.h
│   └── src/
│       ├── sdf.cpp
│       └── SDFProcessor.cpp
└── your_code.cpp
```

### CMake集成

```cmake
# 添加SDF库
add_library(sdf STATIC
    lib/sdf/src/sdf.cpp
    lib/sdf/src/SDFProcessor.cpp
)

target_include_directories(sdf PUBLIC
    lib/sdf/include
)

# 链接到你的程序
add_executable(your_app your_code.cpp)
target_link_libraries(your_app PRIVATE sdf)
```

### 使用示例

```cpp
#include <SDFProcessor.h>

int main() {
    std::vector<Point2D> polygon = {
        {0.0f, 0.5f}, {-0.5f, -0.5f}, {0.5f, -0.5f}
    };
    
    SDFProcessor processor;
    processor.setPolygon(polygon);
    
    // 生成环形
    RingResult ring = processor.generateRing(0.2f, 0.0f);
    
    // 使用 ring.triangleVertices 进行渲染
    return 0;
}
```

## 📁 目录说明

| 目录/文件 | 用途 | 是否需要 |
|----------|------|---------|
| `include/` | 公共头文件 | ✅ 必须 |
| `src/` | 库实现 | ✅ 必须 |
| `examples/` | 示例代码 | ❌ 参考 |
| `docs/` | 文档 | ❌ 参考 |
| `scripts/` | 构建脚本 | ❌ 可选 |

## 🎯 零依赖

SDF库**不依赖**任何外部库：
- ❌ 不需要OpenGL
- ❌ 不需要GLFW
- ❌ 不需要ImGui
- ✅ 只需要C++11标准库

## 📊 文件大小

- `include/sdf.h`: ~200行
- `include/SDFProcessor.h`: ~160行
- `src/sdf.cpp`: ~450行
- `src/SDFProcessor.cpp`: ~200行

**总计：约1000行纯C++代码**

## 💡 使用场景

### 场景1: 只需要基础SDF计算

复制文件：
```
include/sdf.h
src/sdf.cpp
```

使用方法：
```cpp
#include "sdf.h"

float dist = calculatePolygonSDF(point, polygon);
```

### 场景2: 需要完整功能（推荐）

复制文件：
```
include/sdf.h
include/SDFProcessor.h
src/sdf.cpp
src/SDFProcessor.cpp
```

使用方法：
```cpp
#include "SDFProcessor.h"

SDFProcessor processor;
auto result = processor.generateRing(0.2f, 0.0f);
```

## 🔍 详细API文档

查看 `docs/API_USAGE.md` 获取完整API文档。

## 📞 需要帮助？

- 📖 查看 `examples/basic/example_usage.cpp`
- 🎨 查看 `examples/demo/main.cpp` (OpenGL可视化示例)
- 💬 提交Issue到GitHub

---

**移植简单，使用方便！** ✨
