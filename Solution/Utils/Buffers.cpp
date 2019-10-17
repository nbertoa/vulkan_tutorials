#include "Buffers.h"

#include <cassert>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace vk {
Buffers::Buffers(const LogicalDevice& logicalDevice,
                 const PhysicalDevice& physicalDevice,
                 const size_t bufferCount,
                 const VkDeviceSize size,
                 const VkBufferUsageFlags usageFlags,
                 const VkSharingMode sharingMode,
                 const VkMemoryPropertyFlags memoryPropertyFlags) {
    assert(bufferCount > 0);
    mBuffers.reserve(bufferCount);
    std::vector<VkCommandBuffer> commandBuffers(bufferCount);
    for (size_t i = 0; i < bufferCount; ++i) {
        mBuffers.emplace_back(Buffer(logicalDevice,
                                     physicalDevice,
                                     size,
                                     usageFlags,
                                     sharingMode,
                                     memoryPropertyFlags));
    }
}

Buffers::Buffers(const LogicalDevice& logicalDevice,
                 const size_t bufferCount,
                 const VkDeviceSize size,
                 const VkBufferUsageFlags usageFlags,
                 const VkSharingMode sharingMode,
                 const DeviceMemory& deviceMemory) {
    assert(bufferCount > 0);
    mBuffers.reserve(bufferCount);
    std::vector<VkCommandBuffer> commandBuffers(bufferCount);
    for (size_t i = 0; i < bufferCount; ++i) {
        mBuffers.emplace_back(Buffer(logicalDevice,
                                     size,
                                     usageFlags,
                                     sharingMode,
                                     deviceMemory));
    }
}

Buffers::Buffers(Buffers&& other) noexcept
    : mBuffers(std::move(other.mBuffers)) {

}

size_t
Buffers::bufferCount() const {
    assert(mBuffers.empty() == false);
    return mBuffers.size();
}

Buffer&
Buffers::buffer(const size_t bufferIndex) {
    assert(bufferIndex < mBuffers.size());
    return mBuffers[bufferIndex];
}
}