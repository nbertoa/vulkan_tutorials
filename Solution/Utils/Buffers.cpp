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
                 const VkMemoryPropertyFlags memoryPropertyFlags,
                 const VkSharingMode sharingMode,
                 const std::vector<uint32_t>& queueFamilyIndices) {
    assert(bufferCount > 0);
    mBuffers.reserve(bufferCount);
    std::vector<VkCommandBuffer> commandBuffers(bufferCount);
    for (size_t i = 0; i < bufferCount; ++i) {
        mBuffers.emplace_back(Buffer(logicalDevice,
                                     physicalDevice,
                                     size,
                                     usageFlags,
                                     memoryPropertyFlags,
                                     sharingMode,
                                     queueFamilyIndices));
    }
}

Buffers::Buffers(const LogicalDevice& logicalDevice,
                 const size_t bufferCount,
                 const VkDeviceSize size,
                 const VkBufferUsageFlags usageFlags,
                 const DeviceMemory& deviceMemory,
                 const VkSharingMode sharingMode,
                 const std::vector<uint32_t>& queueFamilyIndices) {
    assert(bufferCount > 0);
    mBuffers.reserve(bufferCount);
    std::vector<VkCommandBuffer> commandBuffers(bufferCount);
    for (size_t i = 0; i < bufferCount; ++i) {
        mBuffers.emplace_back(Buffer(logicalDevice,
                                     size,
                                     usageFlags,
                                     deviceMemory,
                                     sharingMode,
                                     queueFamilyIndices));
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