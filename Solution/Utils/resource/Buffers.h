#ifndef UTILS_RESOURCE_BUFFERS
#define UTILS_RESOURCE_BUFFERS

#include <vector>
#include <vulkan/vulkan.hpp>

#include "Buffer.h"

namespace vk2 {
class Buffers {
public:
    // Read Buffer constructor to understand this.
    Buffers(const uint32_t bufferCount,
            const VkDeviceSize size,
            const VkBufferUsageFlags usageFlags,            
            const vk::MemoryPropertyFlags deviceMemoryProperties,
            const VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            const std::vector<uint32_t>& queueFamilyIndices = {});

    // Read Buffer constructor to understand this.
    Buffers(const uint32_t bufferCount,
            const VkDeviceSize size,
            const VkBufferUsageFlags usageFlags,
            const vk::DeviceMemory deviceMemory,
            const VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            const std::vector<uint32_t>& queueFamilyIndices = {});

    Buffers(Buffers&& other) noexcept;
    Buffers(const Buffers&) = delete;
    const Buffers& operator=(const Buffers&) = delete;

    uint32_t 
    bufferCount() const;

    Buffer& 
    buffer(const uint32_t bufferIndex);

private:
    std::vector<Buffer> mBuffers;
};
}

#endif