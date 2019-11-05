#ifndef UTILS_RESOURCE_DEVICE_MEMORY 
#define UTILS_RESOURCE_DEVICE_MEMORY

#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;
class PhysicalDevice;

//
// VkDeviceMemory wrapper.
//
// Creating a Buffer of certain length or an Image with specific dimensions 
// does not automatically allocate memory for it. Instead, resources 
// are initially created as virtual allocations with no backing memory.
//
// It is a 3-step process that must be manually performed by you. 
// You can also choose to use our Vulkan Memory Allocator library which 
// takes care of the allocation for you.
// - Allocate DeviceMemory,
// - Create Buffer or Image,
// - Bind them together using function vkBindBufferMemory or vkBindImageMemory.
//
// That is why you must also create a DeviceMemory object. 
// It represents a block of memory allocated from a specific memory type 
// (as supported by PhysicalDevice) with a specific length in bytes. 
// Device memory is allocated separately and then 
// associated with the resource.
//
// Device memory is memory that is visible to the device
// for example, the contents of opaque images that can be
// natively used by the device, or uniform buffer objects
// that reside in on-devide memory.
//
// You should not allocate separate DeviceMemory for each Buffer or Image. 
// Instead, you should allocate bigger chunks of memory and assign parts of them to your Buffers and Images. 
// Allocation is a costly operation and there is a limit on maximum number of allocations as well, 
// all of which can be queried from your PhysicalDevice.
//
// One exception to the obligation to allocate and bind DeviceMemory 
// for every Image is the creation of a Swapchain.
//
// A Vulkan device operates on data in device memory (GPU memory) via memory objects 
// that are represented in the API by a VkDeviceMemory handle
//
// You need the DeviceMemory to:
// - Bind DeviceMemory to a Buffer object (vkBindBufferMemory).
// - Bind DeviceMemory to a Image object (vkBindImageMemory).
//
// To create/use the DeviceMemory you need:
// - MemoryType from MemoryHeap from PhysicalDevice
//
class DeviceMemory {
public:
    // * physicalDevice is used to get the memory index that is an index
    //   that identifies a memory type from the memoryTypes array of the 
    //   VkPhysicalDeviceMemoryProperties structure. 
    //
    // * memoryRequirements is used to get the memory index that is an index
    //   that identifies a memory type from the memoryTypes array of the 
    //   VkPhysicalDeviceMemoryProperties structure:
    //
    //   - size in bytes of the memory allocation required for the resource.
    //   - alignment in bytes of the offset within the allocation required for the resource.
    //   - memoryTypeBits bitmask that contains one bit set for every supported 
    //     memory type for the resource. Bit i is set if and only if the memory type i in 
    //     the VkPhysicalDeviceMemoryProperties structure for the physical device 
    //     is supported for the resource.
    //
    // * memoryPropertyFlags is used to get the memory index that is an index
    //   that identifies a memory type from the memoryTypes array of the 
    //   VkPhysicalDeviceMemoryProperties structure:
    //
    //   - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    //     this type is the most efficient for device access.
    //     This property will be set if and only if the memory type belongs to a heap 
    //     with the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    //   - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with 
    //     this type can be mapped for host access using vkMapMemory.
    //   - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    //     management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    //     are not needed to flush host writes to the device or make device writes visible 
    //     to the host, respectively.
    //   - VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this 
    //     type is cached on the host. Host memory accesses to uncached memory are slower than 
    //     to cached memory, however uncached memory is always host coherent.
    //   - VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit specifies that the memory type only 
    //     allows device access to the memory. Memory types must not have both 
    //     VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BITand VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set.
    //     Additionally, the object is backing memory may be provided by the implementation lazily 
    //     as specified in Lazily Allocated Memory.
    //   - VK_MEMORY_PROPERTY_PROTECTED_BIT bit specifies that the memory type only allows device 
    //     access to the memory, and allows protected queue operations to access the memory.
    //     Memory types must not have VK_MEMORY_PROPERTY_PROTECTED_BIT set and any of 
    //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set, or VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, 
    //     or VK_MEMORY_PROPERTY_HOST_CACHED_BIT set.
    //   - VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD bit specifies that device accesses to 
    //     allocations of this memory type are automatically made available and visible.
    //   - VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD bit specifies that memory allocated with 
    //     this type is not cached on the device. Uncached device memory is always device coherent.
    DeviceMemory(const LogicalDevice& logicalDevice,
                 const PhysicalDevice& physicalDevice,
                 const VkMemoryRequirements& memoryRequirements,
                 const VkMemoryPropertyFlags memoryPropertyFlags);
    ~DeviceMemory();
    DeviceMemory(DeviceMemory&& other) noexcept;
    DeviceMemory(const DeviceMemory&) = delete;
    const DeviceMemory& operator=(const DeviceMemory&) = delete;

    VkDeviceMemory 
    vkDeviceMemory() const;

private:
    const LogicalDevice& mLogicalDevice;
    VkDeviceMemory mDeviceMemory = VK_NULL_HANDLE;
};
}

#endif