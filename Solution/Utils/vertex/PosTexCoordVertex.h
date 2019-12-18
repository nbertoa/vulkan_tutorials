#ifndef UTILS_VERTEX_POS_TEX_COORD_VERTEX
#define UTILS_VERTEX_POS_TEX_COORD_VERTEX

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vulkan {
// Vertex with position and texture coordinates in that order.
struct PosTexCoordVertex {
    glm::vec3 mPosition = {0.0f, 0.0f, 0.0f};
    glm::vec2 mTexCoord = {0.0f, 0.0f};

    // A vertex binding describes at which rate to load data from memory
    // throughout the vertices.
    // It specifies the number of bytes between data entries and whether
    // to move to the next data entry after each vertex or after each instance.
    static void 
    vertexInputBindingDescriptions(std::vector<vk::VertexInputBindingDescription>& descriptions);

    // A vertex attribute binding describes how to handle vertex input.
    static void 
    vertexInputAttributeDescriptions(std::vector<vk::VertexInputAttributeDescription>& descriptions);
};
}

#endif 
