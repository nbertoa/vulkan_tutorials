#include "DeviceMemory.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"
#include "../device/PhysicalDevice.h"

namespace vk2 {
DeviceMemory::DeviceMemory(const VkMemoryRequirements& memoryRequirements,
                           const VkMemoryPropertyFlags memoryPropertyFlags) {
    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = memoryRequirements.size;
    info.memoryTypeIndex = PhysicalDevice::memoryTypeIndex(memoryRequirements.memoryTypeBits,
                                                                           memoryPropertyFlags);
    assert(PhysicalDevice::isValidMemoryTypeIndex(info.memoryTypeIndex));
    vkChecker(vkAllocateMemory(LogicalDevice::vkDevice(),
                               &info,
                               nullptr,
                               &mDeviceMemory));
}

DeviceMemory::~DeviceMemory() {
    vkFreeMemory(LogicalDevice::vkDevice(),
                 mDeviceMemory,
                 nullptr);
}

DeviceMemory::DeviceMemory(DeviceMemory&& other) noexcept 
    : mDeviceMemory(other.mDeviceMemory)
{
    other.mDeviceMemory = VK_NULL_HANDLE;
}

VkDeviceMemory 
DeviceMemory::vkDeviceMemory() const {
    assert(mDeviceMemory != VK_NULL_HANDLE);
    return mDeviceMemory;
}

}