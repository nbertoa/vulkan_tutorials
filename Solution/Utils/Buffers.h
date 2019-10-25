#ifndef UTILS_BUFFERS
#define UTILS_BUFFERS

#include <vector>
#include <vulkan/vulkan.h>

#include "Buffer.h"

namespace vk {
class LogicalDevice;
class PhysicalDevice;

class Buffers {
public:
    // Read Buffer constructor to understand this.
    Buffers(const LogicalDevice& logicalDevice,
            const PhysicalDevice& physicalDevice,
            const size_t bufferCount,
            const VkDeviceSize size,
            const VkBufferUsageFlags usageFlags,            
            const VkMemoryPropertyFlags memoryPropertyFlags,
            const VkSharingMode sharingMode,
            const std::vector<uint32_t>& queueFamilyIndices = {});

    // Read Buffer constructor to understand this.
    Buffers(const LogicalDevice& logicalDevice,
            const size_t bufferCount,
            const VkDeviceSize size,
            const VkBufferUsageFlags usageFlags,
            const DeviceMemory& deviceMemory,
            const VkSharingMode sharingMode,
            const std::vector<uint32_t>& queueFamilyIndices = {});

    Buffers(Buffers&& other) noexcept;
    Buffers(const Buffers&) = delete;
    const Buffers& operator=(const Buffers&) = delete;

    size_t 
    bufferCount() const;

    Buffer& 
    buffer(const size_t bufferIndex);

private:
    std::vector<Buffer> mBuffers;
};
}

#endif