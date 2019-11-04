#include "Buffers.h"

#include <cassert>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace vk {
Buffers::Buffers(const LogicalDevice& logicalDevice,
                 const PhysicalDevice& physicalDevice,
                 const uint32_t bufferCount,
                 const VkDeviceSize size,
                 const VkBufferUsageFlags usageFlags,
                 const VkMemoryPropertyFlags memoryPropertyFlags,
                 const VkSharingMode sharingMode,
                 const std::vector<uint32_t>& queueFamilyIndices) {
    assert(bufferCount > 0);
    mBuffers.reserve(bufferCount);
    std::vector<VkCommandBuffer> commandBuffers(bufferCount);
    for (uint32_t i = 0; i < bufferCount; ++i) {
        Buffer buffer(logicalDevice,
                      physicalDevice,
                      size,
                      usageFlags,
                      memoryPropertyFlags,
                      sharingMode,
                      queueFamilyIndices);

        mBuffers.emplace_back(std::move(buffer));
    }
}

Buffers::Buffers(const LogicalDevice& logicalDevice,
                 const uint32_t bufferCount,
                 const VkDeviceSize size,
                 const VkBufferUsageFlags usageFlags,
                 const DeviceMemory& deviceMemory,
                 const VkSharingMode sharingMode,
                 const std::vector<uint32_t>& queueFamilyIndices) {
    assert(bufferCount > 0);
    mBuffers.reserve(bufferCount);
    std::vector<VkCommandBuffer> commandBuffers(bufferCount);
    for (uint32_t i = 0; i < bufferCount; ++i) {
        mBuffers.emplace_back(std::move(Buffer(logicalDevice,
                                               size,
                                               usageFlags,
                                               deviceMemory,
                                               sharingMode,
                                               queueFamilyIndices)));
    }
}

Buffers::Buffers(Buffers&& other) noexcept
    : mBuffers(std::move(other.mBuffers)) {

}

uint32_t
Buffers::bufferCount() const {
    assert(mBuffers.empty() == false);
    return static_cast<uint32_t>(mBuffers.size());
}

Buffer&
Buffers::buffer(const uint32_t bufferIndex) {
    assert(bufferIndex < mBuffers.size());
    return mBuffers[bufferIndex];
}
}