# 贡献指南 (Contributing Guide)

感谢你考虑为 SDF 2D Geometry Processor 做出贡献！

## 🎯 贡献方式

### 报告Bug 🐛

如果你发现了bug，请：

1. 检查 [Issues](https://github.com/yourusername/SDF_2D/issues) 是否已有相同问题
2. 如果没有，创建新的Issue，包含：
   - 清晰的标题
   - 详细的问题描述
   - 重现步骤
   - 期望行为
   - 实际行为
   - 截图或日志（如果有）
   - 系统信息（OS, 编译器版本等）

### 提出新功能 ✨

有好的想法？请：

1. 先创建一个Issue讨论
2. 描述功能的用途和价值
3. 如果可能，提供使用示例
4. 等待维护者反馈

### 提交代码 💻

#### 准备工作

```bash
# Fork仓库到你的账号
# 克隆你的fork
git clone https://github.com/your-username/SDF_2D.git
cd SDF_2D

# 添加上游仓库
git remote add upstream https://github.com/original-username/SDF_2D.git
```

#### 开发流程

1. **创建分支**
```bash
git checkout -b feature/your-feature-name
# 或
git checkout -b bugfix/issue-number-description
```

2. **编写代码**
   - 遵循现有代码风格
   - 添加必要的注释
   - 更新相关文档

3. **测试**
```bash
# 编译并测试
.\build.bat
.\run.bat

# 确保没有新的警告或错误
```

4. **提交**
```bash
git add .
git commit -m "feat: add amazing feature"
# 或
git commit -m "fix: resolve issue #123"
```

5. **推送**
```bash
git push origin feature/your-feature-name
```

6. **创建Pull Request**
   - 访问你fork的仓库页面
   - 点击 "New Pull Request"
   - 填写PR模板
   - 等待代码审查

## 📝 代码规范

### C++代码风格

```cpp
// 类名：大驼峰
class SDFProcessor {
    
// 公共方法：小驼峰
public:
    void extractContour();
    
// 私有成员：m_前缀 + 小驼峰
private:
    int m_sdfWidth;
    float m_sdfData;
    
// 常量：大写 + 下划线
private:
    static const int MAX_GRID_SIZE = 1024;
};

// 函数名：小驼峰
float calculateDistance(Point2D p1, Point2D p2);

// 局部变量：小驼峰
float localVariable = 0.0f;

// 结构体：大驼峰
struct ContourResult {
    bool isValid;
};
```

### 注释规范

```cpp
/**
 * @brief 简短描述函数功能
 * 
 * 详细说明函数的行为、算法等
 * 
 * @param param1 参数1的描述
 * @param param2 参数2的描述
 * @return 返回值描述
 */
ReturnType functionName(Type1 param1, Type2 param2);
```

### 提交信息规范

使用 [Conventional Commits](https://www.conventionalcommits.org/) 格式：

```
<type>: <subject>

<body>

<footer>
```

**类型 (type):**
- `feat`: 新功能
- `fix`: Bug修复
- `docs`: 文档更新
- `style`: 代码格式（不影响功能）
- `refactor`: 重构
- `perf`: 性能优化
- `test`: 测试相关
- `chore`: 构建/工具链相关

**示例:**
```
feat: add custom configuration for SDF grid size

Added SDFConfig struct to allow users to customize grid size,
padding, and other parameters.

Closes #42
```

## 🔍 代码审查

提交PR后，维护者会：

1. ✅ 检查代码质量
2. ✅ 验证功能正确性
3. ✅ 测试性能影响
4. 💬 提供反馈和建议

请耐心等待审查，并及时响应反馈。

## 🧪 测试指南

### 手动测试

1. 编译项目
2. 运行程序
3. 测试以下场景：
   - 添加3-10个点形成多边形
   - 启用SDF可视化
   - 调整SDF参数
   - 提取等值线
   - 生成环形填充
   - 切换渲染模式
   - 清空并重新开始

### 性能测试

- 监控帧率（应保持60+ FPS）
- 测试大网格（512x512）性能
- 测试复杂多边形（20+顶点）

## 📚 文档贡献

文档同样重要！你可以：

- 修正错别字
- 改进说明清晰度
- 添加使用示例
- 翻译文档（英文/中文）
- 补充API文档

## 🌟 认可贡献者

所有贡献者都会被列入：
- README.md的致谢部分
- Contributors页面
- Release Notes（如果贡献重大）

## 💡 开发建议

### 推荐工具

- **IDE**: Visual Studio 2019+, VS Code + C/C++ Extension
- **调试**: Visual Studio Debugger, GDB
- **格式化**: ClangFormat
- **分析**: Valgrind (Linux), Visual Studio Profiler

### 有用的命令

```bash
# 清理构建
rm -rf build/

# 重新构建
.\build.bat

# 查看帮助
.\run.bat --help

# 同步上游
git fetch upstream
git merge upstream/main
```

## ❓ 需要帮助？

- 📖 查看 [API文档](API_USAGE.md)
- 💬 在Issue中提问
- 📧 联系维护者

## 🎉 感谢

感谢你的贡献，让这个项目变得更好！

---

**记住**: 任何贡献都值得认可，从修正错别字到添加重大功能。每个贡献都很重要！ 🙏
