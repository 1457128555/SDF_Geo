#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include "sdf.h"  // SDF calculation module

// Global variables

std::vector<Point2D> points;
bool isAddingPoints = false;
const float gridSpacing = 1.0f;  // Fixed grid spacing

// SDF visualization
bool showSDFVisualization = false;

// SDF texture
const float sdfGridSize = 0.1f;  // Grid cell size for SDF texture
float sdfTexturePadding = 0.5f;  // Extra padding around polygon for SDF texture
float sdfGradientScale = 2.0f;   // Gradient scale for SDF visualization
unsigned int sdfTexture = 0;
int sdfTextureWidth = 0;
int sdfTextureHeight = 0;
bool sdfTextureNeedsUpdate = true;

// Iso-contour extraction
bool showIsoContour = false;
float isoContourValue = 0.0f;    // SDF threshold for contour extraction
std::vector<Point2D> isoContourPoints;
bool isoContourNeedsUpdate = true;

// Ring triangulation
enum RingRenderMode {
    RING_FILLED = 0,
    RING_WIREFRAME = 1
};

bool showRingFill = false;
RingRenderMode ringRenderMode = RING_FILLED;
float ringOuterThreshold = 0.2f;  // Outer contour threshold
float ringInnerThreshold = 0.0f;  // Inner contour threshold
std::vector<Point2D> ringVertices;
bool ringNeedsUpdate = true;

// Camera system
float cameraZoom = 1.0f;
float cameraPanX = 0.0f;
float cameraPanY = 0.0f;
bool isDragging = false;
double lastMouseX = 0.0;
double lastMouseY = 0.0;

GLFWwindow* g_window = nullptr;

// Shader sources
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform float zoom;
uniform vec2 pan;
void main()
{
    vec2 pos = (aPos - pan) * zoom;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.2f, 0.8f, 0.3f, 1.0f);
}
)";

const char* gridFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
}
)";

const char* axisFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.8f, 0.8f, 0.8f, 1.0f);
}
)";

// Texture rendering shaders
const char* textureVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
uniform float zoom;
uniform vec2 pan;
out vec2 TexCoord;
void main()
{
    vec2 pos = (aPos - pan) * zoom;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* textureFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D sdfTexture;
uniform float gradientScale;
void main()
{
    float sdf = texture(sdfTexture, TexCoord).r;
    
    // Color mapping for SDF
    vec3 color;
    if (sdf < 0.0)
    {
        // Inside: blue gradient
        float intensity = min(1.0, -sdf * gradientScale);
        color = vec3(0.2, 0.4, 0.6 + intensity * 0.4);
    }
    else if (sdf > 0.0)
    {
        // Outside: red gradient
        float intensity = min(1.0, sdf * gradientScale);
        color = vec3(0.6 + intensity * 0.4, 0.3, 0.2);
    }
    else
    {
        // On boundary: yellow
        color = vec3(1.0, 1.0, 0.3);
    }
    
    FragColor = vec4(color, 0.7);  // Semi-transparent
}
)";

// Update SDF texture based on current polygon
void updateSDFTexture(const std::vector<Point2D>& polygon, float minX, float maxX, float minY, float maxY, float padding)
{
    if (polygon.size() < 3)
        return;
    
    // Add padding to the bounds
    minX -= padding;
    maxX += padding;
    minY -= padding;
    maxY += padding;
    
    // Calculate texture dimensions based on grid size
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;
    
    sdfTextureWidth = (int)(rangeX / sdfGridSize) + 1;
    sdfTextureHeight = (int)(rangeY / sdfGridSize) + 1;
    
    // Limit texture size for performance
    sdfTextureWidth = std::min(sdfTextureWidth, 512);
    sdfTextureHeight = std::min(sdfTextureHeight, 512);
    
    // Generate SDF data
    float* sdfData = new float[sdfTextureWidth * sdfTextureHeight];
    generateSDFGrid(sdfTextureWidth, sdfTextureHeight,
                   minX, maxX, minY, maxY,
                   polygon, sdfData);
    
    // Create or update OpenGL texture
    if (sdfTexture == 0)
    {
        glGenTextures(1, &sdfTexture);
    }
    
    glBindTexture(GL_TEXTURE_2D, sdfTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, sdfTextureWidth, sdfTextureHeight, 0, GL_RED, GL_FLOAT, sdfData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    delete[] sdfData;
    
    std::cout << "SDF texture updated: " << sdfTextureWidth << "x" << sdfTextureHeight << std::endl;
}

// Generate grid lines based on camera view
std::vector<Point2D> generateGridLines(float spacing, float zoom, float panX, float panY)
{
    std::vector<Point2D> gridLines;
    
    // Calculate visible range in world space
    float visibleRangeX = 1.0f / zoom;
    float visibleRangeY = 1.0f / zoom;
    
    float minX = panX - visibleRangeX;
    float maxX = panX + visibleRangeX;
    float minY = panY - visibleRangeY;
    float maxY = panY + visibleRangeY;
    
    // Snap to grid
    float startX = floor(minX / spacing) * spacing;
    float startY = floor(minY / spacing) * spacing;
    
    // Vertical lines
    for (float x = startX; x <= maxX + spacing; x += spacing)
    {
        gridLines.push_back({x, minY - spacing});
        gridLines.push_back({x, maxY + spacing});
    }
    
    // Horizontal lines
    for (float y = startY; y <= maxY + spacing; y += spacing)
    {
        gridLines.push_back({minX - spacing, y});
        gridLines.push_back({maxX + spacing, y});
    }
    
    return gridLines;
}

// Generate axis lines
std::vector<Point2D> generateAxisLines(float zoom, float panX, float panY)
{
    std::vector<Point2D> axisLines;
    
    // Calculate visible range
    float visibleRangeX = 1.0f / zoom;
    float visibleRangeY = 1.0f / zoom;
    
    float minX = panX - visibleRangeX;
    float maxX = panX + visibleRangeX;
    float minY = panY - visibleRangeY;
    float maxY = panY + visibleRangeY;
    
    // X axis (y = 0)
    axisLines.push_back({minX - 1.0f, 0.0f});
    axisLines.push_back({maxX + 1.0f, 0.0f});
    
    // Y axis (x = 0)
    axisLines.push_back({0.0f, minY - 1.0f});
    axisLines.push_back({0.0f, maxY + 1.0f});
    
    return axisLines;
}

// Mouse scroll callback for zoom
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;
    
    // Zoom in/out
    float zoomSpeed = 0.1f;
    cameraZoom += yoffset * zoomSpeed;
    cameraZoom = std::max(0.1f, std::min(cameraZoom, 10.0f));  // Clamp between 0.1 and 10
    
    std::cout << "Camera zoom: " << cameraZoom << std::endl;
}

// Mouse button callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Check if ImGui wants to capture mouse
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;
    
    // Right button for dragging
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            isDragging = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        else if (action == GLFW_RELEASE)
        {
            isDragging = false;
        }
    }
    
    // Left button for adding points
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && isAddingPoints)
    {
        // Get mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Get window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        // Convert to NDC (Normalized Device Coordinates)
        float x = (xpos / width) * 2.0f - 1.0f;
        float y = -((ypos / height) * 2.0f - 1.0f);  // Flip Y
        
        // Apply inverse camera transform to get world coordinates
        x = x / cameraZoom + cameraPanX;
        y = y / cameraZoom + cameraPanY;
        
        // Add point
        points.push_back({x, y});
        sdfTextureNeedsUpdate = true;
        isoContourNeedsUpdate = true;
        ringNeedsUpdate = true;
        std::cout << "Point added: (" << x << ", " << y << ") - Total points: " << points.size() << std::endl;
    }
}

// Mouse move callback for camera panning
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (isDragging)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        // Calculate delta in screen space
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;
        
        // Convert to NDC space and apply to camera
        cameraPanX -= (deltaX / width) * 2.0f / cameraZoom;
        cameraPanY += (deltaY / height) * 2.0f / cameraZoom;  // Flip Y
        
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Compile shader
unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

// Create shader program
unsigned int createShaderProgram()
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Shader linking failed: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

int main()
{
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口 (1200x800)
    GLFWwindow* window = glfwCreateWindow(1200, 800, "SDF_Geo", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    g_window = window;

    // 加载OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置ImGui上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // 加载中文字体
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    
    // 设置ImGui样式
    ImGui::StyleColorsDark();
    
    // 初始化ImGui GLFW和OpenGL3后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Create shader programs
    unsigned int shaderProgram = createShaderProgram();
    
    // Create grid shader
    unsigned int gridVertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int gridFragmentShader = compileShader(GL_FRAGMENT_SHADER, gridFragmentShaderSource);
    unsigned int gridShaderProgram = glCreateProgram();
    glAttachShader(gridShaderProgram, gridVertexShader);
    glAttachShader(gridShaderProgram, gridFragmentShader);
    glLinkProgram(gridShaderProgram);
    glDeleteShader(gridVertexShader);
    glDeleteShader(gridFragmentShader);
    
    // Create axis shader
    unsigned int axisVertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int axisFragmentShader = compileShader(GL_FRAGMENT_SHADER, axisFragmentShaderSource);
    unsigned int axisShaderProgram = glCreateProgram();
    glAttachShader(axisShaderProgram, axisVertexShader);
    glAttachShader(axisShaderProgram, axisFragmentShader);
    glLinkProgram(axisShaderProgram);
    glDeleteShader(axisVertexShader);
    glDeleteShader(axisFragmentShader);
    
    // Create texture shader
    unsigned int textureVertexShader = compileShader(GL_VERTEX_SHADER, textureVertexShaderSource);
    unsigned int textureFragmentShader = compileShader(GL_FRAGMENT_SHADER, textureFragmentShaderSource);
    unsigned int textureShaderProgram = glCreateProgram();
    glAttachShader(textureShaderProgram, textureVertexShader);
    glAttachShader(textureShaderProgram, textureFragmentShader);
    glLinkProgram(textureShaderProgram);
    glDeleteShader(textureVertexShader);
    glDeleteShader(textureFragmentShader);
    
    // Create VAO and VBO for points
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Create VAO and VBO for grid
    unsigned int gridVAO, gridVBO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Create VAO and VBO for axis
    unsigned int axisVAO, axisVBO;
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);
    
    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Create VAO and VBO for texture quad
    unsigned int textureVAO, textureVBO;
    glGenVertexArrays(1, &textureVAO);
    glGenBuffers(1, &textureVBO);
    
    glBindVertexArray(textureVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    // Position (2) + TexCoord (2) = 4 floats per vertex
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Enable blending for SDF texture transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Enable point size
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(8.0f);

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 处理输入
        processInput(window);
        glfwPollEvents();

        // 启动新的ImGui帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create ImGui window
        ImGui::Begin(u8"控制面板");
        
        if (ImGui::Button(u8"添加点"))
        {
            isAddingPoints = !isAddingPoints;
            std::cout << "Add point mode: " << (isAddingPoints ? "ON" : "OFF") << std::endl;
        }
        ImGui::SameLine();
        ImGui::Text(isAddingPoints ? u8"(模式: 开启)" : u8"(模式: 关闭)");
        
        ImGui::Separator();
        ImGui::Text(u8"点的数量: %d", (int)points.size());
        
        // SDF Visualization Section
        if (points.size() >= 3)
        {
            ImGui::Separator();
            ImGui::Text(u8"SDF 功能:");
            
            ImGui::Checkbox(u8"显示SDF可视化", &showSDFVisualization);
            
            if (showSDFVisualization)
            {
                ImGui::SliderFloat(u8"SDF边距", &sdfTexturePadding, 0.1f, 2.0f, "%.2f");
                ImGui::Text(u8"提示: 边距值越大，SDF范围越广");
                
                ImGui::Separator();
                ImGui::SliderFloat(u8"渐变阈值", &sdfGradientScale, 0.5f, 10.0f, "%.1f");
                ImGui::Text(u8"提示: 值越大，颜色变化越快");
                
                ImGui::Separator();
                if (ImGui::Button(u8"重新生成SDF"))
                {
                    sdfTextureNeedsUpdate = true;
                }
            }
            
            ImGui::Separator();
            ImGui::Checkbox(u8"显示等值线", &showIsoContour);
            
            if (showIsoContour)
            {
                if (ImGui::SliderFloat(u8"等值线阈值", &isoContourValue, -1.0f, 1.0f, "%.3f"))
                {
                    isoContourNeedsUpdate = true;
                }
                ImGui::Text(u8"提示: 0.0=边界, 正值=外部, 负值=内部");
                
                if (ImGui::Button(u8"重新提取等值线"))
                {
                    isoContourNeedsUpdate = true;
                }
            }
            
            ImGui::Separator();
            ImGui::Checkbox(u8"显示环形填充", &showRingFill);
            
            if (showRingFill)
            {
                // Render mode dropdown
                const char* modes[] = { u8"填充", u8"线框" };
                int currentMode = static_cast<int>(ringRenderMode);
                if (ImGui::Combo(u8"渲染模式", &currentMode, modes, IM_ARRAYSIZE(modes)))
                {
                    ringRenderMode = static_cast<RingRenderMode>(currentMode);
                }
                
                ImGui::Separator();
                if (ImGui::SliderFloat(u8"外圈阈值", &ringOuterThreshold, -0.5f, 1.0f, "%.3f"))
                {
                    ringNeedsUpdate = true;
                }
                if (ImGui::SliderFloat(u8"内圈阈值", &ringInnerThreshold, -1.0f, 0.5f, "%.3f"))
                {
                    ringNeedsUpdate = true;
                }
                ImGui::Text(u8"提示: 外圈阈值应大于内圈阈值");
                
                if (ImGui::Button(u8"重新生成环形"))
                {
                    ringNeedsUpdate = true;
                }
            }
            
            // Polygon info
            ImGui::Separator();
            float minX, maxX, minY, maxY;
            calculatePolygonBounds(points, minX, maxX, minY, maxY);
            ImGui::Text(u8"多边形范围:");
            ImGui::Text("  X: [%.2f, %.2f]", minX, maxX);
            ImGui::Text("  Y: [%.2f, %.2f]", minY, maxY);
        }
        
        ImGui::Separator();
        if (ImGui::Button(u8"清空所有点"))
        {
            points.clear();
            sdfTextureNeedsUpdate = true;
            isoContourNeedsUpdate = true;
            ringNeedsUpdate = true;
            std::cout << "All points cleared" << std::endl;
        }
        
        if (points.size() > 0)
        {
            ImGui::Separator();
            ImGui::Text(u8"点的坐标:");
            for (size_t i = 0; i < points.size(); i++)
            {
                ImGui::Text("  Point %d: (%.3f, %.3f)", (int)i, points[i].x, points[i].y);
            }
        }
        
        ImGui::End();

        // 渲染
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render grid (always visible)
        {
            std::vector<Point2D> gridLines = generateGridLines(gridSpacing, cameraZoom, cameraPanX, cameraPanY);
            
            glUseProgram(gridShaderProgram);
            glUniform1f(glGetUniformLocation(gridShaderProgram, "zoom"), cameraZoom);
            glUniform2f(glGetUniformLocation(gridShaderProgram, "pan"), cameraPanX, cameraPanY);
            glBindVertexArray(gridVAO);
            glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
            glBufferData(GL_ARRAY_BUFFER, gridLines.size() * sizeof(Point2D), gridLines.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(GL_LINES, 0, gridLines.size());
        }
        
        // Render axis (always)
        {
            std::vector<Point2D> axisLines = generateAxisLines(cameraZoom, cameraPanX, cameraPanY);
            
            glUseProgram(axisShaderProgram);
            glUniform1f(glGetUniformLocation(axisShaderProgram, "zoom"), cameraZoom);
            glUniform2f(glGetUniformLocation(axisShaderProgram, "pan"), cameraPanX, cameraPanY);
            glBindVertexArray(axisVAO);
            glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
            glBufferData(GL_ARRAY_BUFFER, axisLines.size() * sizeof(Point2D), axisLines.data(), GL_DYNAMIC_DRAW);
            glLineWidth(2.0f);
            glDrawArrays(GL_LINES, 0, axisLines.size());
            glLineWidth(1.0f);
        }
        
        // Update and render SDF texture
        if (showSDFVisualization && points.size() >= 3)
        {
            // Update texture if needed
            if (sdfTextureNeedsUpdate)
            {
                float minX, maxX, minY, maxY;
                calculatePolygonBounds(points, minX, maxX, minY, maxY);
                updateSDFTexture(points, minX, maxX, minY, maxY, sdfTexturePadding);
                sdfTextureNeedsUpdate = false;
            }
            
            // Extract iso-contour if needed
            if (showIsoContour && isoContourNeedsUpdate)
            {
                float minX, maxX, minY, maxY;
                calculatePolygonBounds(points, minX, maxX, minY, maxY);
                minX -= sdfTexturePadding;
                maxX += sdfTexturePadding;
                minY -= sdfTexturePadding;
                maxY += sdfTexturePadding;
                
                int width = (int)((maxX - minX) / sdfGridSize) + 1;
                int height = (int)((maxY - minY) / sdfGridSize) + 1;
                width = std::min(width, 512);
                height = std::min(height, 512);
                
                float* sdfData = new float[width * height];
                generateSDFGrid(width, height, minX, maxX, minY, maxY, points, sdfData);
                
                isoContourPoints = extractIsoContour(sdfData, width, height,
                                                     minX, maxX, minY, maxY,
                                                     isoContourValue);
                
                delete[] sdfData;
                isoContourNeedsUpdate = false;
                
                std::cout << "Iso-contour extracted: " << isoContourPoints.size() / 2 << " line segments" << std::endl;
            }
            
            // Generate ring triangulation if needed
            if (showRingFill && ringNeedsUpdate)
            {
                float minX, maxX, minY, maxY;
                calculatePolygonBounds(points, minX, maxX, minY, maxY);
                minX -= sdfTexturePadding;
                maxX += sdfTexturePadding;
                minY -= sdfTexturePadding;
                maxY += sdfTexturePadding;
                
                int width = (int)((maxX - minX) / sdfGridSize) + 1;
                int height = (int)((maxY - minY) / sdfGridSize) + 1;
                width = std::min(width, 512);
                height = std::min(height, 512);
                
                float* sdfData = new float[width * height];
                generateSDFGrid(width, height, minX, maxX, minY, maxY, points, sdfData);
                
                // Extract outer and inner contours (line segments)
                std::vector<Point2D> outerSegments = extractIsoContour(sdfData, width, height,
                                                                        minX, maxX, minY, maxY,
                                                                        ringOuterThreshold);
                std::vector<Point2D> innerSegments = extractIsoContour(sdfData, width, height,
                                                                        minX, maxX, minY, maxY,
                                                                        ringInnerThreshold);
                
                delete[] sdfData;
                
                // Connect line segments into ordered contours
                std::vector<Point2D> outerContour = connectLineSegments(outerSegments);
                std::vector<Point2D> innerContour = connectLineSegments(innerSegments);
                
                std::cout << "Outer contour: " << outerSegments.size() / 2 << " segments -> " 
                          << outerContour.size() << " points" << std::endl;
                std::cout << "Inner contour: " << innerSegments.size() / 2 << " segments -> " 
                          << innerContour.size() << " points" << std::endl;
                
                // Triangulate the ring
                if (outerContour.size() >= 3 && innerContour.size() >= 3)
                {
                    std::vector<Triangle> triangles = triangulateRing(outerContour, innerContour);
                    ringVertices = trianglesToVertices(triangles);
                    std::cout << "Ring triangulated: " << triangles.size() << " triangles" << std::endl;
                }
                else
                {
                    ringVertices.clear();
                    std::cout << "Failed to extract valid contours for ring (outer: " 
                              << outerContour.size() << ", inner: " << innerContour.size() << ")" << std::endl;
                }
                
                ringNeedsUpdate = false;
            }
            
            // Render SDF texture
            if (sdfTexture != 0)
            {
                // Calculate polygon bounds for texture quad (with padding)
                float minX, maxX, minY, maxY;
                calculatePolygonBounds(points, minX, maxX, minY, maxY);
                minX -= sdfTexturePadding;
                maxX += sdfTexturePadding;
                minY -= sdfTexturePadding;
                maxY += sdfTexturePadding;
                
                // Create quad vertices (position + texcoord)
                float quadVertices[] = {
                    // pos            // texcoord
                    minX, minY,       0.0f, 0.0f,  // bottom-left
                    maxX, minY,       1.0f, 0.0f,  // bottom-right
                    maxX, maxY,       1.0f, 1.0f,  // top-right
                    
                    minX, minY,       0.0f, 0.0f,  // bottom-left
                    maxX, maxY,       1.0f, 1.0f,  // top-right
                    minX, maxY,       0.0f, 1.0f   // top-left
                };
                
                glUseProgram(textureShaderProgram);
                glUniform1f(glGetUniformLocation(textureShaderProgram, "zoom"), cameraZoom);
                glUniform2f(glGetUniformLocation(textureShaderProgram, "pan"), cameraPanX, cameraPanY);
                glUniform1f(glGetUniformLocation(textureShaderProgram, "gradientScale"), sdfGradientScale);
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, sdfTexture);
                glUniform1i(glGetUniformLocation(textureShaderProgram, "sdfTexture"), 0);
                
                glBindVertexArray(textureVAO);
                glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        
        // Render geometry if we have points
        if (points.size() > 0)
        {
            glUseProgram(shaderProgram);
            glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), cameraZoom);
            glUniform2f(glGetUniformLocation(shaderProgram, "pan"), cameraPanX, cameraPanY);
            glBindVertexArray(VAO);
            
            // Upload point data
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point2D), points.data(), GL_DYNAMIC_DRAW);
            
            // Draw lines connecting points
            if (points.size() > 1)
            {
                glDrawArrays(GL_LINE_LOOP, 0, points.size());
            }
            
            // Draw points
            glPointSize(8.0f);
            glDrawArrays(GL_POINTS, 0, points.size());
        }
        
        // Render iso-contour if we have extracted contour
        if (showIsoContour && isoContourPoints.size() > 0)
        {
            glUseProgram(shaderProgram);
            glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), cameraZoom);
            glUniform2f(glGetUniformLocation(shaderProgram, "pan"), cameraPanX, cameraPanY);
            glBindVertexArray(VAO);
            
            // Upload iso-contour line segments
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, isoContourPoints.size() * sizeof(Point2D), isoContourPoints.data(), GL_DYNAMIC_DRAW);
            
            // Draw iso-contour lines with different color/thickness
            glLineWidth(3.0f);
            glDrawArrays(GL_LINES, 0, isoContourPoints.size());
            glLineWidth(1.0f);
        }
        
        // Render ring fill if we have triangulated ring
        if (showRingFill && ringVertices.size() > 0)
        {
            glUseProgram(shaderProgram);
            glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), cameraZoom);
            glUniform2f(glGetUniformLocation(shaderProgram, "pan"), cameraPanX, cameraPanY);
            glBindVertexArray(VAO);
            
            // Upload ring triangle vertices
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, ringVertices.size() * sizeof(Point2D), ringVertices.data(), GL_DYNAMIC_DRAW);
            
            // Set polygon mode based on render mode
            if (ringRenderMode == RING_FILLED)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDrawArrays(GL_TRIANGLES, 0, ringVertices.size());
            }
            else // RING_WIREFRAME
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(2.0f);
                glDrawArrays(GL_TRIANGLES, 0, ringVertices.size());
                glLineWidth(1.0f);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore default
            }
        }
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &axisVBO);
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteBuffers(1, &textureVBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(gridShaderProgram);
    glDeleteProgram(axisShaderProgram);
    glDeleteProgram(textureShaderProgram);
    if (sdfTexture != 0)
        glDeleteTextures(1, &sdfTexture);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
