# VulkanDisplayer 
该项目是一个基于Vulkan的图像渲染器，是为了学习Vulkan而创建的。通过本项目可以了解通过Vulkan API来实现图像渲染的基本流程。

# 项目依赖
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#linux)
- GLFW3
- GLM
- CMake
- GCC
- glslc

# 项目结构
``` shell
cmake/          # cmake模块用于查找Vulkan库 
    FindGLFW3.cmake
    FindGLM.cmake   
include/        # 项目头文件
    Vertex.h    # 顶点结构体定义
    VulkanDisplayer.h # VulkanDisplayer类定义
shader/         # 项目着色器文件
    shader.frag   # 片段着色器源文件
    shader.frag.spv # glslc编译后的片段着色器文件
    shader.vert   # 顶点着色器源文件
    shader.vert.spv # glslc编译后的顶点着色器文件
src/            # 项目源文件
    Vertex.cpp  # 顶点结构体实现
    VulkanDisplayer.cpp # VulkanDisplayer类实现
main.cpp        # 项目入口文件
CMakeLists.txt  # 项目CMake配置文件
README.md       # 项目说明文件
```

# 项目编译
``` shell
mkdir build
cd build
cmake ..
make
```
# 项目运行
``` shell
./displayer
```

# 项目效果

加载一个三角形，然后通过Vulkan API进行渲染，同时每一帧都会旋转三角形。

# 渲染流程

1. 首先需要创建一个窗口，这里使用GLFW3库来创建窗口。
2. 初始化Vulkan相关资源：
   1. 创建Vulkan实例: 通过Vulkan SDK提供的API来创建Vulkan实例
   2. 创建窗口表面(这里通过GLFW3库来创建，使用不同窗口创建库会有所不同) 
   3. 选择可用的物理设备: 通过Vulkan SDK提供的API来选择可用的物理设备，一般选择第一个可用的物理设备。   
   4. 创建逻辑设备：重要的是创建graphics queue和present queue，这里使用相同的队列。
   5. 创建交换链: 交换链是用来交换图像的, 一般用来做双缓冲，三缓冲等。
   6. 创建图像视图： 交换链图像视图，这个视图是为了接住交换链渲染出来的图像。
   7. 创建渲染通道：渲染通道是用来描述渲染过程的，比如颜色附件，深度附件等。
   8. 创建DescriptorSetLayout：描述符集布局，用来描述描述符集的布局，一般UniformBuffer，纹理这些内存都会放在描述符集中。
   9. 创建管线布局：管线布局是用来描述管线的，比如描述符集布局，顶点输入等。
   10. 创建帧缓冲区
   11. 创建命令池
   12. 创建命令缓冲区
   13. 创建顶点，索引，纹理缓冲区
   14. 创建UniformBuffer
   15. 创建DescriptorPool
   16. 创建DescriptorSet
   17. 创建渲染信号量

3. 进入渲染循环，每一帧都会进行如下操作：
    - 获取窗口事件
    - 更新渲染资源
     - 1. 开始渲染
     - 2. 更新UniformBuffer
     - 3. (如果需要更新设备上的内存，需要继续提交到命令缓冲区，比如更新Vertex的值。recordCommandBuffer())
     - 4. 提交命令缓冲区
    - 渲染
4. 退出程序，释放资源

# Vulkan Debug
1. validation layer debug callback. 设置validation layer和debug callback可以帮助我们检查Vulkan API调用是否正确。

2. [renderdoc](https://renderdoc.org/) 
renderdoc是一个非常好用的图形调试工具，可以帮助我们查看渲染出来的图像中的信息，比如顶点，纹理等内部的信息。

# 参考资料
- [Vulkan Tutorial](https://vulkan-tutorial.com/)