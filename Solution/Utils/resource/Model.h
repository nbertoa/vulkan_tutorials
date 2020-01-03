#ifndef UTILS_RESOURCE_MODEL
#define UTILS_RESOURCE_MODEL

#include <vector>

#include "Buffer.h"

namespace vulkan {
template<typename T>
struct Model {

// The client must free the returned Buffer.
Buffer* 
createVertexBuffer() const;
Buffer*
createIndexBuffer() const;

std::vector<T> mVertices;
std::vector<uint32_t> mIndices;
};

template<typename T>
Buffer*
Model<T>::createVertexBuffer() const {
    assert(mVertices.empty() == false);

    const size_t verticesSize = sizeof(T) * mVertices.size();

    Buffer* buffer = new Buffer(verticesSize,
                                vk::BufferUsageFlagBits::eTransferDst |
                                vk::BufferUsageFlagBits::eVertexBuffer,
                                vk::MemoryPropertyFlagBits::eDeviceLocal);

    buffer->copyFromDataToDeviceMemory(mVertices.data(),
                                       verticesSize);

    return buffer;
}

template<typename T>
Buffer*
Model<T>::createIndexBuffer() const {
    assert(mIndices.empty() == false);

    const size_t indicesSize = sizeof(uint32_t) * mIndices.size();

    Buffer* buffer = new Buffer(indicesSize,
                                vk::BufferUsageFlagBits::eTransferDst |
                                vk::BufferUsageFlagBits::eIndexBuffer,
                                vk::MemoryPropertyFlagBits::eDeviceLocal);

    buffer->copyFromDataToDeviceMemory(mIndices.data(),
                                       indicesSize);

    return buffer;
}

}

#endif
