#include "PosColorVertex.h"

namespace vk2 {
void
PosColorVertex::vertexInputBindingDescriptions(std::vector<VkVertexInputBindingDescription>& descriptions) {
    descriptions.resize(1);

    // The index of the binding in the array of bindings.
    descriptions[0].binding = 0;
    // The number of bytes from one entry to the next.
    descriptions[0].stride = sizeof(PosColorVertex);
    // Move to the next data entry after each vertex
    descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void
PosColorVertex::vertexInputAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& descriptions) {
    descriptions.resize(2);
    
    // From which binding the per-vertex data comes
    descriptions[0].binding = 0;
    // Location directive of the input in the vertex shader
    descriptions[0].location = 0;
    // Type of data for the attribute
    descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    // The number of bytes since the start of the
    // per-vertex data to read from.
    descriptions[0].offset = offsetof(PosColorVertex, 
                                      mPosition);

    descriptions[1].binding = 0;
    descriptions[1].location = 1;
    descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    descriptions[1].offset = offsetof(PosColorVertex, 
                                      mColor);
}
}

