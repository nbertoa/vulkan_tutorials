#include "PhysicalDevice.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../Instance.h"
#include "../Surface.h"

namespace vk {
PhysicalDevice::PhysicalDevice(const Instance& instance,
                               const Surface& surface)
    : mDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME} 
{
    initPhysicalDevice(instance.getCandidatePhysicalDevices(surface,
                                                            mDeviceExtensions));
}
PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
    : mPhysicalDevice(other.mPhysicalDevice)
    , mGraphicsSupportQueueFamilyIndex(other.mGraphicsSupportQueueFamilyIndex)
    , mPresentationSupportQueueFamilyIndex(other.mPresentationSupportQueueFamilyIndex)
    , mDeviceExtensions(std::move(other.mDeviceExtensions))
{
    other.mPhysicalDevice = VK_NULL_HANDLE;
}

VkPhysicalDevice 
PhysicalDevice::vkPhysicalDevice() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPhysicalDevice;
}

uint32_t 
PhysicalDevice::graphicsSupportQueueFamilyIndex() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mGraphicsSupportQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::transferSupportQueueFamilyIndex() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mTransferSupportQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::presentationSupportQueueFamilyIndex() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPresentationSupportQueueFamilyIndex;
}

const std::vector<const char*>& 
PhysicalDevice::deviceExtensionNames() const {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mDeviceExtensions;
}

uint32_t 
PhysicalDevice::memoryTypeIndex(const uint32_t memoryTypeFilter,
                                const VkMemoryPropertyFlags memoryPropertyFlags) const {

    uint32_t typeIndex = std::numeric_limits<uint32_t>::max();

    // VkPhysicalDeviceMemoryProperties:
    // - memoryTypeCount in the memoryTypes array.
    // - memoryTypes array describes the memory types that can be used to access memory allocated 
    //   from the heaps specified by memoryHeaps:
    //   . heapIndex  this memory type corresponds to, 
    //     and must be less than memoryHeapCount from the VkPhysicalDeviceMemoryProperties structure.
    //   . propertyFlags bitmask for this memory type.
    // - memoryHeapCount in the memoryHeaps array.
    // - memoryHeaps array describing the memory heaps from which memory can be allocated.
    //   . VK_MEMORY_HEAP_DEVICE_LOCAL_BIT specifies that the heap corresponds to device local memory.
    //     Device local memory may have different performance characteristics than host local memory, 
    //     and may support different memory property flags.
    //   . VK_MEMORY_HEAP_MULTI_INSTANCE_BIT specifies that in a logical device representing more 
    //     than one physical device, there is a per-physical device instance of the heap memory.
    //     By default, an allocation from such a heap will be replicated to each physical device’s instance of the heap.
    //
    // Notes:
    // The VkPhysicalDeviceMemoryProperties structure describes a number of memory heaps as well as a number of memory types 
    // that can be used to access memory allocated in those heaps.Each heap describes a memory resource of a particular size, 
    // and each memory type describes a set of memory properties(e.g.host cached vs uncached) that can be used with a given memory heap.
    // Allocations using a particular memory type will consume resources from the heap indicated by that memory type’s heap index.
    // More than one memory type may share each heap, and the heaps and memory types provide a mechanism to advertise an accurate size 
    // of the physical memory resources while allowing the memory to be used with a variety of different properties.
    //
    // The number of memory heaps is given by memoryHeapCount and is less than or equal to VK_MAX_MEMORY_HEAPS.
    // Each heap is described by an element of the memoryHeaps array as a VkMemoryHeap structure.
    // The number of memory types available across all memory heaps is given by memoryTypeCount and is less than or equal to VK_MAX_MEMORY_TYPES.
    // Each memory type is described by an element of the memoryTypes array as a VkMemoryType structure.
    //
    // At least one heap must include VK_MEMORY_HEAP_DEVICE_LOCAL_BIT in VkMemoryHeap::flags.
    // If there are multiple heaps that all have similar performance characteristics, they may all include VK_MEMORY_HEAP_DEVICE_LOCAL_BIT.
    // In a unified memory architecture(UMA) system there is often only a single memory heap which is considered to be equally “local” 
    // to the hostand to the device, and such an implementation must advertise the heap as device-local.
    //
    // For each pair of elements X and Y returned in memoryTypes, X must be placed at a lower index position than Y if:
    // - either the set of bit flags returned in the propertyFlags member of X is a strict subset of the set of bit flags 
    //   returned in the propertyFlags member of Y; or
    // - the propertyFlags members of X and Y are equal, and X belongs to a memory heap with greater performance
    //   (as determined in an implementation-specific manner); or
    // - the propertyFlags members of X includes VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD or 
    //   VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD and Y does not
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice,
                                        &memoryProperties);

    // memoryTypeFilter is used to specify the bit field of memory types
    // that are suitable.
    // That means that we can find the index of a suitable memory type
    // by simply iterating over them and checking if the corresponding
    // bit is set to 1.
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        // We can find the index of a suitable memory type
        // by simply iterating over them and checking if the corresponding
        // bit is set to 1.
        if (memoryTypeFilter & (1 << i)) {
            const VkMemoryPropertyFlags currentMemoryPropertyFlags = memoryProperties.memoryTypes[i].propertyFlags;

            // We may have more than one desiderable property, so we should
            // check if the result of the bitwise AND is not just non-zero,
            // but equal to the desired properties bit field.
            // If there is a memory type suitable for the buffer that also
            // has all of the properties we need, then we return its index,
            // otherwise, std::numeric_limits<uint32_t>::max().
            if ((currentMemoryPropertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
                typeIndex = i;
                break;
            }
        }
    }

    return typeIndex;
}

bool 
PhysicalDevice::isValidMemoryTypeIndex(const uint32_t memoryTypeIndex) const {
    return memoryTypeIndex != std::numeric_limits<uint32_t>::max();
}

void
PhysicalDevice::initPhysicalDevice(const std::vector<PhysicalDeviceData>& supportedPhysicalDevices) {
    assert(mPhysicalDevice == VK_NULL_HANDLE);
    
    const PhysicalDeviceData* chosenDeviceData = &supportedPhysicalDevices.front();

    // From all the suitable physical devices, we get the first that is a discrete GPU.
    // Otherwise, we get the first device in the list.
    for (const PhysicalDeviceData& deviceData : supportedPhysicalDevices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(deviceData.vkPhysicalDevice(), 
                                      &deviceProperties);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            chosenDeviceData = &deviceData;
            break;
        }
    }
        
    mPhysicalDevice = chosenDeviceData->vkPhysicalDevice();
    mGraphicsSupportQueueFamilyIndex = chosenDeviceData->graphicsSupportQueueFamilyIndex();
    mTransferSupportQueueFamilyIndex = chosenDeviceData->transferSupportQueueFamilyIndex();
    mPresentationSupportQueueFamilyIndex = chosenDeviceData->presentationSupportQueueFamilyIndex();
}
}