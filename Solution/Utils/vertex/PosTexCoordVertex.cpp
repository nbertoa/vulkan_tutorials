#include "PosTexCoordVertex.h"

namespace vk2 {
void
PosTexCoordVertex::vertexInputBindingDescriptions(std::vector<vk::VertexInputBindingDescription>& descriptions) {
    descriptions.resize(1);

    // The index of the binding in the array of bindings.
    descriptions[0].binding = 0;
    // The number of bytes from one entry to the next.
    descriptions[0].stride = sizeof(PosTexCoordVertex);
    // Move to the next data entry after each vertex
    descriptions[0].inputRate = vk::VertexInputRate::eVertex;
}

void
PosTexCoordVertex::vertexInputAttributeDescriptions(std::vector<vk::VertexInputAttributeDescription>& descriptions) {
    descriptions.resize(2);
    
    // From which binding the per-vertex data comes
    descriptions[0].binding = 0;
    // Location directive of the input in the vertex shader
    descriptions[0].location = 0;
    // Type of data for the attribute
    descriptions[0].format = vk::Format::eR32G32B32A32Sfloat;
    // The number of bytes since the start of the
    // per-vertex data to read from.
    descriptions[0].offset = offsetof(PosTexCoordVertex, 
                                      mPosition);

    descriptions[1].binding = 0;
    descriptions[1].location = 1;
    descriptions[1].format = vk::Format::eR32G32Sfloat;
    descriptions[1].offset = offsetof(PosTexCoordVertex, 
                                      mTexCoord);
}
}

