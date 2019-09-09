#ifndef VERTEX
#define VERTEX

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

struct Vertex {
    glm::vec2 mPosition = { 0.0f, 0.0f };
    glm::vec3 mColor = {0.0f, 0.0f, 0.0f};

    // A vertex binding describes at which rate to load data from memory
    // throughout the vertices.
    // It specifies the number of bytes between data entries and whether
    // to move to the next data entry after each vertex or after each instance.
    static VkVertexInputBindingDescription vertexInputBindingDescription();

    // A vertex attribute binding describes how to handle vertex input.
    static std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription();
};

#endif 