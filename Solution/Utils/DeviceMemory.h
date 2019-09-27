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
//
// A Vulkan device operates on data in device memory (GPU memory) via memory objects 
// that are represented in the API by a VkDeviceMemory handle
class DeviceMemory {
public:
    // Memory property flags:
    // - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    //   this type is the most efficient for device access.
    //   This property will be set ifand only if the memory type belongs to a heap 
    //   with the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    //
    // - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with 
    //   this type can be mapped for host access using vkMapMemory.
    //
    // - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    //   management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    //   are not needed to flush host writes to the device or make device writes visible 
    //   to the host, respectively.
    //
    // - VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this 
    //   type is cached on the host.Host memory accesses to uncached memory are slower than 
    //   to cached memory, however uncached memory is always host coherent.
    //
    // - VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit specifies that the memory type only 
    //   allows device access to the memory.Memory types must not have both 
    //   K_MEMORY_PROPERTY_LAZILY_ALLOCATED_BITand VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set.
    //   Additionally, the object’s backing memory may be provided by the implementation lazily 
    //   as specified in Lazily Allocated Memory.
    //
    // - VK_MEMORY_PROPERTY_PROTECTED_BIT bit specifies that the memory type only allows device 
    //   access to the memory, and allows protected queue operations to access the memory.
    //   Memory types must not have VK_MEMORY_PROPERTY_PROTECTED_BIT setand any of 
    //   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set, or VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, 
    //   or VK_MEMORY_PROPERTY_HOST_CACHED_BIT set.
    //
    // - VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD bit specifies that device accesses to 
    //   allocations of this memory type are automatically made availableand visible.
    //
    // - VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD bit specifies that memory allocated with 
    //   this type is not cached on the device.Uncached device memory is always device coherent.
    //
    // Memory requirements:
    // - size is the size, in bytes, of the memory allocation required for the resource.
    // - alignment is the alignment, in bytes, of the offset within the 
    //   allocation required for the resource.
    // - memoryTypeBits is a bitmaskand contains one bit set for every supported 
    //   memory type for the resource.Bit i is set ifand only if the memory type i in 
    //   the VkPhysicalDeviceMemoryProperties structure for the physical device 
    //   is supported for the resource.
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