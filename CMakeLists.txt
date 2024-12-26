cmake_minimum_required(VERSION 3.18)

project(VulkanDisplayer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 确保必须使用 C++17
# Vulkan
find_package(Vulkan REQUIRED)

# OpenCV
find_package(OpenCV REQUIRED)


file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories("include")
include("cmake/FindGLFW3.cmake")
include("cmake/FindGLM.cmake")

add_executable(displayer main.cpp ${SOURCES})

target_link_libraries(displayer Vulkan::Vulkan ${OpenCV_LIBS} glfw)

