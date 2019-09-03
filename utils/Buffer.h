#ifndef UTILS_BUFFER
#define UTILS_BUFFER

#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkBuffer's wrapper to be able to create/destroy
// and handle it easily.
// Buffers represent linear arrays of data which are 
// used for various purposes by binding them to a
// graphics or compute pipeline via descriptor sets
// or via certain commands, or by directly specifying 
// them as parameters to certain commands.
class Buffer {
public:
    Buffer(const LogicalDevice& logicalDevice,
           const VkDeviceSize sizeInBytes, 
           const VkBufferUsageFlags usageFlags,
           const VkSharingMode sharingMode);
    ~Buffer();

    Buffer(Buffer&& other) noexcept;

    Buffer(const Buffer&) = delete;
    const Buffer& operator=(const Buffer&) = delete;

    VkMemoryRequirements memoryRequirements() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkBuffer mBuffer = VK_NULL_HANDLE;
};
}

#endif 