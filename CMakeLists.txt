cmake_minimum_required(VERSION 3.10)
project(SDF_Geo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置UTF-8编码支持 (MSVC)
if(MSVC)
    add_compile_options(/utf-8)
endif()

#============================================
# SDF Library (Core)
#============================================
add_library(sdf STATIC
    src/sdf.cpp
    src/SDFProcessor.cpp
)

target_include_directories(sdf PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#============================================
# Demo Application
#============================================
# 查找OpenGL
find_package(OpenGL REQUIRED)

# GLFW、GLAD和ImGui的路径配置
# 方式1: 使用vcpkg (推荐)
# 如果使用vcpkg，请在命令行中使用: cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..
find_package(glfw3 CONFIG)
find_package(glad CONFIG)
find_package(imgui CONFIG)

# 创建Demo可执行文件
add_executable(SDF_Demo
    examples/demo/main.cpp
)

target_link_libraries(SDF_Demo PRIVATE
    sdf  # 链接SDF库
)

if(glfw3_FOUND AND glad_FOUND AND imgui_FOUND)
    # 使用vcpkg安装的库
    target_link_libraries(SDF_Demo PRIVATE
        OpenGL::GL
        glfw
        glad::glad
        imgui::imgui
    )
else()
    message(WARNING "GLFW, GLAD, or ImGui not found via vcpkg")
    message("Please run setup.bat to install dependencies")
    message("Or install manually: vcpkg install glfw3 glad imgui[glfw-binding,opengl3-binding]")
endif()

# Windows特定设置
if(WIN32)
    target_link_libraries(SDF_Demo PRIVATE opengl32)
endif()

#============================================
# Basic Usage Example (Optional)
#============================================
option(BUILD_EXAMPLES "Build basic usage examples" OFF)

if(BUILD_EXAMPLES)
    add_executable(basic_usage
        examples/basic/example_usage.cpp
    )
    
    target_link_libraries(basic_usage PRIVATE sdf)
endif()
