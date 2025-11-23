cmake_minimum_required(VERSION 3.10)
project(SDF_Geo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置UTF-8编码支持 (MSVC)
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 添加可执行文件
add_executable(${PROJECT_NAME} 
    main.cpp
    sdf.cpp
    SDFProcessor.cpp
)

# 查找OpenGL
find_package(OpenGL REQUIRED)

# GLFW、GLAD和ImGui的路径配置
# 方式1: 使用vcpkg (推荐)
# 如果使用vcpkg，请在命令行中使用: cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..
find_package(glfw3 CONFIG)
find_package(glad CONFIG)
find_package(imgui CONFIG)

if(glfw3_FOUND AND glad_FOUND AND imgui_FOUND)
    # 使用vcpkg安装的库
    target_link_libraries(${PROJECT_NAME} 
        OpenGL::GL
        glfw
        glad::glad
        imgui::imgui
    )
else()
    # 方式2: 手动指定库路径
    # 设置GLFW路径
    set(GLFW_DIR "" CACHE PATH "GLFW directory")
    if(GLFW_DIR)
        include_directories(${GLFW_DIR}/include)
        link_directories(${GLFW_DIR}/lib)
    endif()

    # 设置GLAD路径
    set(GLAD_DIR "" CACHE PATH "GLAD directory")
    if(GLAD_DIR)
        include_directories(${GLAD_DIR}/include)
        add_library(glad ${GLAD_DIR}/src/glad.c)
        target_link_libraries(${PROJECT_NAME} glad)
    endif()

    # 链接OpenGL和GLFW
    target_link_libraries(${PROJECT_NAME} 
        OpenGL::GL
        glfw3
    )
endif()

# Windows特定设置
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
endif()
