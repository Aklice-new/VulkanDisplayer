#include "Vertex.h"
#include "VulkanDisplayer.h"
#include <vulkan/vulkan.h>
#include <iostream>
#include <opencv2/opencv.hpp>
int main()

{
    std::cout << "Vulkan version " << VK_VERSION_MAJOR(VK_HEADER_VERSION) << "." << VK_VERSION_MINOR(VK_HEADER_VERSION)
              << "." << VK_VERSION_PATCH(VK_HEADER_VERSION) << std::endl;

    // read rgb image and give a dummy depth as the z-depth to generate the postion of every point.
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.push_back(Vertex(glm::vec3(0.0, 0.f, 0.1f), glm::vec3(1.0f, 0.0f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(0.75, -0.489, 0.1f), glm::vec3(0.0f, 1.0f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(0.2, -0.489, 0.1f), glm::vec3(0.0f, 0.0f, 1.0f)));

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    VulkanDisplayer displayer(vertices, indices);

    try
    {
        displayer.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}