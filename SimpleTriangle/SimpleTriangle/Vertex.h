#ifndef VERTEX
#define VERTEX

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

struct Vertex {
    glm::vec2 mPosition = { 0.0f, 0.0f };
    glm::vec3 mColor = {0.0f, 0.0f, 0.0f};

    static VkVertexInputBindingDescription vertexInputBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription();
};

#endif 