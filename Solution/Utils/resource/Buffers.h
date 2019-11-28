#ifndef UTILS_RESOURCE_BUFFERS
#define UTILS_RESOURCE_BUFFERS

#include <vector>
#include <vulkan/vulkan.h>

#include "Buffer.h"

namespace vk {
class Buffers {
public:
    // Read Buffer constructor to understand this.
    Buffers(const uint32_t bufferCount,
            const VkDeviceSize size,
            const VkBufferUsageFlags usageFlags,            
            const VkMemoryPropertyFlags memoryPropertyFlags,
            const VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            const std::vector<uint32_t>& queueFamilyIndices = {});

    // Read Buffer constructor to understand this.
    Buffers(const uint32_t bufferCount,
            const VkDeviceSize size,
            const VkBufferUsageFlags usageFlags,
            const DeviceMemory& deviceMemory,
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