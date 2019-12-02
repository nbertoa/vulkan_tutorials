#include "PhysicalDevice.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../Instance.h"

namespace vk2 {
vk::PhysicalDevice 
PhysicalDevice::mPhysicalDevice;

uint32_t 
PhysicalDevice::mGraphicsQueueFamilyIndex = 0;

uint32_t 
PhysicalDevice::mTransferQueueFamilyIndex = 0;

uint32_t 
PhysicalDevice::mPresentationQueueFamilyIndex = 0;

std::vector<const char*> mDeviceExtensions;

void
PhysicalDevice::initialize(const std::vector<const char*>& deviceExtensionNames) {
    assert(deviceExtensionNames.empty() == false);

    const std::vector<PhysicalDeviceData> candidatePhysicalDevices = 
        Instance::getCandidatePhysicalDevices(deviceExtensionNames);

    const PhysicalDeviceData* chosenDeviceData = &candidatePhysicalDevices.front();

    // From all the suitable physical devices, we get the first that is a discrete GPU.
    // Otherwise, we get the first device in the list.
    for (const PhysicalDeviceData& deviceData : candidatePhysicalDevices) {
        const vk::PhysicalDeviceProperties properties =
            deviceData.device().getProperties();

        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            chosenDeviceData = &deviceData;
            break;
        }
    }

    mPhysicalDevice = chosenDeviceData->device();
    mGraphicsQueueFamilyIndex = chosenDeviceData->graphicsQueueFamilyIndex();
    mTransferQueueFamilyIndex = chosenDeviceData->transferQueueFamilyIndex();
    mPresentationQueueFamilyIndex = chosenDeviceData->presentationQueueFamilyIndex();
}

void
PhysicalDevice::finalize() {

}

vk::PhysicalDevice 
PhysicalDevice::device() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPhysicalDevice;
}

uint32_t 
PhysicalDevice::graphicsQueueFamilyIndex() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mGraphicsQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::transferQueueFamilyIndex() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mTransferQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::presentationQueueFamilyIndex() {
    assert(mPhysicalDevice != VK_NULL_HANDLE);
    return mPresentationQueueFamilyIndex;
}

uint32_t 
PhysicalDevice::memoryTypeIndex(const uint32_t memoryTypeFilter,
                                const vk::MemoryPropertyFlags memoryPropertyFlags) {

    uint32_t typeIndex = std::numeric_limits<uint32_t>::max();

    // Each physical device can then be queried for its capabilities, 
    // including its queueand queue family properties.
    const vk::PhysicalDeviceMemoryProperties memoryProperties =
        mPhysicalDevice.getMemoryProperties();

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
            const vk::MemoryPropertyFlags currentProperty = 
                memoryProperties.memoryTypes[i].propertyFlags;

            // We may have more than one desiderable property, so we should
            // check if the result of the bitwise AND is not just non-zero,
            // but equal to the desired properties bit field.
            // If there is a memory type suitable for the buffer that also
            // has all of the properties we need, then we return its index,
            // otherwise, std::numeric_limits<uint32_t>::max().
            if ((currentProperty & memoryPropertyFlags) == memoryPropertyFlags) {
                typeIndex = i;
                break;
            }
        }
    }

    return typeIndex;
}

bool 
PhysicalDevice::isValidMemoryTypeIndex(const uint32_t memoryTypeIndex) {
    return memoryTypeIndex != std::numeric_limits<uint32_t>::max();
}
}