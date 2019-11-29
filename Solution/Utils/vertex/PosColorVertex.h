#ifndef UTILS_VERTEX_POS_COLOR_VERTEX
#define UTILS_VERTEX_POS_COLOR_VERTEX

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk2 {
// Vertex with position and color in that order.
struct PosColorVertex {
    glm::vec3 mPosition = {0.0f, 0.0f, 0.0f};
    glm::vec3 mColor = {0.0f, 0.0f, 0.0f};

    // A vertex binding describes at which rate to load data from memory
    // throughout the vertices.
    // It specifies the number of bytes between data entries and whether
    // to move to the next data entry after each vertex or after each instance.
    static void 
    vertexInputBindingDescriptions(std::vector<VkVertexInputBindingDescription>& descriptions);

    // A vertex attribute binding describes how to handle vertex input.
    static void 
    vertexInputAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& descriptions);
};
}

#endif 
