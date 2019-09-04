#ifndef UTILS_DEVICE_MEMORY 
#define UTILS_DEVICE_MEMORY

#include <cassert>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkDeviceMemory's wrapper to be able to create/destroy
// and handle it easily.
//
// Resources (like buffers) are initially created as virtual
// allocations with no backing memory.
// Device memory is allocated separately and then 
// associated with the resource.
// Device memory is memory that is visible to the device
// for example, the contents of opaque images that can be
// natively used by the device, or uniform buffer objects
// that reside in on-devide memory.
class DeviceMemory {
public:
    DeviceMemory(const LogicalDevice& logicalDevice,
                 const VkMemoryRequirements& memoryRequirements,
                 const VkMemoryPropertyFlags memoryPropertyFlags);
    ~DeviceMemory();

    DeviceMemory(DeviceMemory&& other) noexcept;

    DeviceMemory(const DeviceMemory&) = delete;
    const DeviceMemory& operator=(const DeviceMemory&) = delete;

    VkDeviceMemory vkDeviceMemory() const {
        assert(mDeviceMemory != VK_NULL_HANDLE);
        return mDeviceMemory;
    }

private:
    const LogicalDevice& mLogicalDevice;
    VkDeviceMemory mDeviceMemory = VK_NULL_HANDLE;
};
}

#endif