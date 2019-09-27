#include "DeviceMemory.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
DeviceMemory::DeviceMemory(const LogicalDevice& logicalDevice,
                           const VkMemoryRequirements& memoryRequirements,
                           const VkMemoryPropertyFlags memoryPropertyFlags)
    : mLogicalDevice(logicalDevice)
{
    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    // Specify the memory size and type, both of which are derived
    // from the memory requirements of the buffer and the desired
    // property.
    info.allocationSize = memoryRequirements.size;
    info.memoryTypeIndex = 
        mLogicalDevice.physicalDevice().memoryTypeIndex(memoryRequirements.memoryTypeBits,
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

VkDeviceMemory 
DeviceMemory::vkDeviceMemory() const {
    assert(mDeviceMemory != VK_NULL_HANDLE);
    return mDeviceMemory;
}

}