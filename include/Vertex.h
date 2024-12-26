#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    Vertex();
    Vertex(const glm::vec3& position, const glm::vec3& color);

    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};

#endif // _VERTEX_H_