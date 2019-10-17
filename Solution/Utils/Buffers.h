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
            const VkSharingMode sharingMode,
            const VkMemoryPropertyFlags memoryPropertyFlags);

    // Read Buffer constructor to understand this.
    Buffers(const LogicalDevice& logicalDevice,
            const size_t bufferCount,
            const VkDeviceSize size,
            const VkBufferUsageFlags usageFlags,
            const VkSharingMode sharingMode,
            const DeviceMemory& deviceMemory);

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