#include "DeviceMemory.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace vk {
DeviceMemory::DeviceMemory(const LogicalDevice& logicalDevice,
                           const PhysicalDevice& physicalDevice,
                           const VkMemoryRequirements& memoryRequirements,
                           const VkMemoryPropertyFlags memoryPropertyFlags)
    : mLogicalDevice(logicalDevice)
{
    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = memoryRequirements.size;
    info.memoryTypeIndex = physicalDevice.memoryTypeIndex(memoryRequirements.memoryTypeBits,
                                                          memoryPropertyFlags);
    assert(physicalDevice.isValidMemoryTypeIndex(info.memoryTypeIndex));
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