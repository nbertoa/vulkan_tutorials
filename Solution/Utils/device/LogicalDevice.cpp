#include "LogicalDevice.h"

#include <cassert>

#include "PhysicalDevice.h"

namespace vulkan {
vk::Device 
LogicalDevice::mLogicalDevice;

vk::Queue
LogicalDevice::mGraphicsQueue;

vk::Queue
LogicalDevice::mTransferQueue;

vk::Queue
LogicalDevice::mPresentationQueue;

void
LogicalDevice::initialize(const std::vector<const char*>& deviceExtensionNames) {
    assert(mLogicalDevice == VK_NULL_HANDLE);

    initLogicalDevice(deviceExtensionNames);

    initQueues();
}

void
LogicalDevice::finalize() {
    assert(mLogicalDevice != VK_NULL_HANDLE);
    mLogicalDevice.destroy();
}

vk::Device
LogicalDevice::device() {
    assert(mLogicalDevice != VK_NULL_HANDLE);
    return mLogicalDevice;
}

vk::Queue
LogicalDevice::graphicsQueue() {
    assert(mGraphicsQueue != VK_NULL_HANDLE);
    return mGraphicsQueue;
}

vk::Queue
LogicalDevice::transferQueue() {
    assert(mTransferQueue != VK_NULL_HANDLE);
    return mTransferQueue;
}

vk::Queue
LogicalDevice::presentationQueue() {
    assert(mPresentationQueue != VK_NULL_HANDLE);
    return mPresentationQueue;
}

void
LogicalDevice::initLogicalDevice(const std::vector<const char*>& deviceExtensionNames) {
    assert(mLogicalDevice == VK_NULL_HANDLE);
    assert(deviceExtensionNames.empty() == false);

    const float queuePriority = 1.0f;
    const std::vector<vk::DeviceQueueCreateInfo> infoVector = queuesCreateInfo(queuePriority);
    assert(infoVector.empty() == false);
    
    vk::PhysicalDeviceFeatures physicalDeviceFeatures;
    physicalDeviceFeatures.setSamplerAnisotropy(VK_TRUE);

    vk::DeviceCreateInfo info;
    info.setPEnabledFeatures(&physicalDeviceFeatures);
    info.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensionNames.size()));
    info.setPpEnabledExtensionNames(deviceExtensionNames.data());
    info.setQueueCreateInfoCount(static_cast<uint32_t>(infoVector.size()));
    info.setPQueueCreateInfos(infoVector.data());
    
    mLogicalDevice = PhysicalDevice::device().createDevice(info);
}

std::vector<vk::DeviceQueueCreateInfo>
LogicalDevice::queuesCreateInfo(const float& queuePriority) {
    std::vector<vk::DeviceQueueCreateInfo> infoVector;

    vk::DeviceQueueCreateInfo info;
    info.setQueueFamilyIndex(PhysicalDevice::graphicsQueueFamilyIndex());
    info.setQueueCount(1);
    info.setPQueuePriorities(&queuePriority);
    infoVector.emplace_back(info);

    // If graphics and presentation queue family indices are different, 
    // we add a new create info for presentation queue.
    if (PhysicalDevice::graphicsQueueFamilyIndex() !=
        PhysicalDevice::presentationQueueFamilyIndex()) {
        info.setQueueFamilyIndex(PhysicalDevice::presentationQueueFamilyIndex());
        infoVector.emplace_back(info);

        // If presentation and transfer queue family indices are different,
        // then we add a new create info for transfer queue.
        if (PhysicalDevice::presentationQueueFamilyIndex() !=
            PhysicalDevice::transferQueueFamilyIndex()) {
            info.setQueueFamilyIndex(PhysicalDevice::transferQueueFamilyIndex());
            infoVector.emplace_back(info);
        }
    } else if (PhysicalDevice::graphicsQueueFamilyIndex() !=
               PhysicalDevice::transferQueueFamilyIndex()) {
        // If graphics and transfer queue family indices are different,
        // then we add a new create info for transfer queue.
        info.setQueueFamilyIndex(PhysicalDevice::transferQueueFamilyIndex());
        infoVector.emplace_back(info);
    }

    return infoVector;
}

void
LogicalDevice::initQueues() {
    assert(mLogicalDevice != VK_NULL_HANDLE);

    mGraphicsQueue = mLogicalDevice.getQueue(PhysicalDevice::graphicsQueueFamilyIndex(),
                                             0);

    mTransferQueue = mLogicalDevice.getQueue(PhysicalDevice::transferQueueFamilyIndex(),
                                             0);

    mPresentationQueue = mLogicalDevice.getQueue(PhysicalDevice::presentationQueueFamilyIndex(),
                                                 0);
}

}