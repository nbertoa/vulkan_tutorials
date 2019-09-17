#include "DeviceMemory.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
DeviceMemory::DeviceMemory(const LogicalDevice& logicalDevice,
                           const VkMemoryRequirements& memoryRequirements,
                           const VkMemoryPropertyFlags memoryPropertyFlags)
    : mLogicalDevice(logicalDevice)
{
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

    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    // Specify the memory size and type, both of which are derived
    // from the memory requirements of the buffer and the desired
    // property.
    info.allocationSize = memoryRequirements.size;
    info.memoryTypeIndex = mLogicalDevice.physicalDevice().memoryTypeIndex(memoryRequirements.memoryTypeBits,
                                                                           memoryPropertyFlags);
    assert(mLogicalDevice.physicalDevice().isValidMemoryTypeIndex(info.memoryTypeIndex));
    vkChecker(vkAllocateMemory(mLogicalDevice.vkDevice(),
                               &info,
                               nullptr,
                               &mDeviceMemory));
}

DeviceMemory::~DeviceMemory() {
    vkFreeMemory(mLogicalDevice.vkDevice(),
                 mDeviceMemory,
                 nullptr);
}

DeviceMemory::DeviceMemory(DeviceMemory&& other) noexcept 
    : mLogicalDevice(other.mLogicalDevice)
    , mDeviceMemory(other.mDeviceMemory)
{
    other.mDeviceMemory = VK_NULL_HANDLE;
}

}