#ifndef UTILS_DEVICE_MEMORY 
#define UTILS_DEVICE_MEMORY

#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkDeviceMemory wrapper.
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
    // Memory property flags:
    // - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory 
    // allocated with this type can be mapped for host access using vkMapMemory.
    //
    // - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    // management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    // are not needed to flush host writes to the device or make device writes visible 
    // to the host, respectively.
    //
    // - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    // this type is the most efficient for device access. 
    // This property will be set if and only if the memory type belongs to a heap with 
    // the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    DeviceMemory(const LogicalDevice& logicalDevice,
                 const VkMemoryRequirements& memoryRequirements,
                 const VkMemoryPropertyFlags memoryPropertyFlags);
    ~DeviceMemory();
    DeviceMemory(DeviceMemory&& other) noexcept;
    DeviceMemory(const DeviceMemory&) = delete;
    const DeviceMemory& operator=(const DeviceMemory&) = delete;

    VkDeviceMemory vkDeviceMemory() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkDeviceMemory mDeviceMemory = VK_NULL_HANDLE;
};
}

#endif