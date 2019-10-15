#ifndef UTILS_PHYSICAL_DEVICE
#define UTILS_PHYSICAL_DEVICE

#include <vector>
#include <vulkan/vulkan.h>

#include "PhysicalDeviceData.h"

namespace vk {
class Instance;
class Surface;

//
// VkPhysicalDevice wrapper.
//
// Vulkan separates the concept of physical and logical devices.
// A physical device usually represents a single complete implementation 
// of Vulkan (excluding instance-level functionality) available to the host, 
// of which there are a finite number.
// A logical device represents an instance of that implementation with its 
// own state and resources independent of other logical devices.
//
// PhysicalDevice represents a specific Vulkan-compatible device, like a graphics card. 
//
// PhysicalDevice can enumerate all available types of Queue Families. 
// The graphics queue is the main one, but you may also have additional ones 
// that support only Compute or Transfer.
//
// PhysicalDevice can also enumerate Memory Heaps and Memory Types inside them. 
// A Memory Heap represents a specific pool of RAM. 
// It may abstract your system RAM on the motherboard or a certain memory space in video RAM 
// on a dedicated graphics card, or any other host- or device-specific memory the implementation 
// wants to expose. 
// You must specify the Memory Type when allocating memory. 
// It holds specific requirements for the memory blob like visible to the host, 
// coherent (between CPU and GPU) and cached. 
// There may be an arbitrary combination of these, depending on the device driver.
//
// You enumerate these from Instance and you can then query them for their vendorID, 
// deviceID, and supported features, as well as other properties and limits.
//
// You need the PhysicalDevice to:
// - Get the memory heap from where you can get the memory type (indices uint32_t)
//   which is needed for DeviceMemory creation.
// - Get queue family (indices uint32_t) which are needed for Queues creation 
//   in LogicalDevice. (read below)
//
// To understand queue families, you first have to understand queues.
// A queue is something you submit command buffers to, and command buffers submitted 
// to a queue are executed in order[*1] relative to each other.
// Command buffers submitted to different queues are unordered relative to each other unless 
// you explicitly synchronize them with VkSemaphore. You can only submit work to a queue 
// from one thread at a time, but different threads can submit work to different queues simultaneously.
//
// Each queue can only perform certain kinds of operations:
// - Graphics queues can run graphics pipelines started by vkCmdDraw* commands.
// - Compute queues can run compute pipelines started by vkCmdDispatch* .
// - Transfer queues can perform transfer(copy) operations from vkCmdCopy*.
// - Sparse binding queues can change the binding of sparse resources to memory with vkQueueBindSparse
// (note this is an operation submitted directly to a queue, not a command in a command buffer).
//
// Some queues can perform multiple kinds of operations. Every command that can be submitted to a queue have 
// a "Command Properties" table that lists what queue types can execute the command.
// A queue family just describes a set of queues with identical properties.
//
// [*1] Oversimplifying a bit. They start in order, but are allowed to proceed independently after that 
// and complete out of order. Independent progress of different queues is not guaranteed though.
//
class PhysicalDevice {
public:
    // - surface is used to check if the candidate physical device
    //   supports presentation and also if our swap chain is supported.
    PhysicalDevice(const Instance& instance,
                   const Surface& surface);
    PhysicalDevice(PhysicalDevice&& other) noexcept;
    PhysicalDevice(const PhysicalDevice&) = delete;
    const PhysicalDevice& operator=(const PhysicalDevice&) = delete;

    VkPhysicalDevice 
    vkPhysicalDevice() const;

    uint32_t 
    graphicsSupportQueueFamilyIndex() const;

    uint32_t 
    transferSupportQueueFamilyIndex() const;

    uint32_t 
    presentationSupportQueueFamilyIndex() const;

    // Extensions may define new Vulkan commands, structures, and enumerants.
    // Extensions may extend or change the behavior of the Vulkan API.
    const std::vector<const char*>& 
    deviceExtensionNames() const;

    // Returns the memory type index of this physical device that is
    // suitable. If there is not a suitable memory property, then it returns
    // std::numeric_limits<uint32_t>::max()
    //
    // PhysicalDevice can also enumerate Memory Heaps and Memory Types inside them. 
    // A Memory Heap represents a specific pool of RAM. 
    // It may abstract your system RAM on the motherboard or a certain memory space in video RAM 
    // on a dedicated graphics card, or any other host- or device-specific memory the implementation 
    // wants to expose. 
    // You must specify the Memory Type when allocating memory. 
    // It holds specific requirements for the memory blob like visible to the host, 
    // coherent (between CPU and GPU) and cached. 
    // There may be an arbitrary combination of these, depending on the device driver.
    //
    // - memoryTypeFilter is to specify the bit field of memory types
    //   that are suitable.
    // - memoryPropertyFlags is used to check if there is a memory type in the physical device
    //   that is suitable for the buffer that also has all of the properties we need. 
    //
    // VkMemoryPropertyFlags:
    // - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with 
    //   this type is the most efficient for device access.
    //   This property will be set ifand only if the memory type belongs to a heap 
    //   with the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
    // - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with 
    //   this type can be mapped for host access using vkMapMemory.
    // - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache 
    //   management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
    //   are not needed to flush host writes to the device or make device writes visible 
    //   to the host, respectively.
    // - VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this 
    //   type is cached on the host.Host memory accesses to uncached memory are slower than 
    //   to cached memory, however uncached memory is always host coherent.
    // - VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit specifies that the memory type only 
    //   allows device access to the memory.Memory types must not have both 
    //   K_MEMORY_PROPERTY_LAZILY_ALLOCATED_BITand VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set.
    //   Additionally, the object’s backing memory may be provided by the implementation lazily 
    //   as specified in Lazily Allocated Memory.
    // - VK_MEMORY_PROPERTY_PROTECTED_BIT bit specifies that the memory type only allows device 
    //   access to the memory, and allows protected queue operations to access the memory.
    //   Memory types must not have VK_MEMORY_PROPERTY_PROTECTED_BIT setand any of 
    //   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set, or VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, 
    //   or VK_MEMORY_PROPERTY_HOST_CACHED_BIT set.
    // - VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD bit specifies that device accesses to 
    //   allocations of this memory type are automatically made availableand visible.
    // - VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD bit specifies that memory allocated with 
    //   this type is not cached on the device.Uncached device memory is always device coherent.
    uint32_t 
    memoryTypeIndex(const uint32_t memoryTypeFilter,
                    const VkMemoryPropertyFlags memoryPropertyFlags) const;

    bool 
    isValidMemoryTypeIndex(const uint32_t memoryTypeIndex) const;

private:                            
    void 
    initPhysicalDevice(const std::vector<PhysicalDeviceData>& supportedPhysicalDevices);

    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;

    uint32_t mGraphicsSupportQueueFamilyIndex = 0;
    uint32_t mTransferSupportQueueFamilyIndex = 0;
    uint32_t mPresentationSupportQueueFamilyIndex = 0;

    std::vector<const char*> mDeviceExtensions;
};
}

#endif