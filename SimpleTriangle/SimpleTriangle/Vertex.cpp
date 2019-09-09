#include "Vertex.h"

VkVertexInputBindingDescription 
Vertex::vertexInputBindingDescription() {
    VkVertexInputBindingDescription description = {};
    // The index of the binding in the array of bindings.
    description.binding = 0;
    // The number of bytes from one entry to the next.
    description.stride = sizeof(Vertex);
    // Move to the next data entry after each vertex
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return description;
}

std::vector<VkVertexInputAttributeDescription> 
Vertex::vertexInputAttributeDescription() {
    std::vector<VkVertexInputAttributeDescription> descriptions(2);
    // From which binding the per-vertex data comes
    descriptions[0].binding = 0;
    // Location directive of the input in the vertex shader
    descriptions[0].location = 0;
    // Type of data for the attribute
    descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    // The number of bytes since the start of the
    // per-vertex data to read from.
    descriptions[0].offset = offsetof(Vertex, mPosition);

    descriptions[1].binding = 0;
    descriptions[1].location = 1;
    descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    descriptions[1].offset = offsetof(Vertex, mColor);

    return descriptions;
}