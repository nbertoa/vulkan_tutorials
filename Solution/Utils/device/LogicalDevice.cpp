#include "LogicalDevice.h"

#include <cassert>

#include "../DebugUtils.h"
#include "PhysicalDevice.h"

namespace vk2 {
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
    assert(!mLogicalDevice);

    initLogicalDevice(deviceExtensionNames);

    initQueues();
}

void
LogicalDevice::finalize() {
    assert(mLogicalDevice);
    mLogicalDevice.destroy();
}

vk::Device
LogicalDevice::device() {
    assert(mLogicalDevice);
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
    assert(!mLogicalDevice);

    const float queuePriority = 1.0f;
    const std::vector<vk::DeviceQueueCreateInfo> createInfoVector = 
        physicalDeviceQueuesCreateInfo(queuePriority);
    
    vk::PhysicalDeviceFeatures physicalDeviceFeatures;
    physicalDeviceFeatures.setSamplerAnisotropy(VK_TRUE);

    vk::DeviceCreateInfo physicalDeviceCreateInfo = 
    {
        vk::DeviceCreateFlags(),
        static_cast<uint32_t>(createInfoVector.size()),
        createInfoVector.data(),
        0, // deprecated layer count
        nullptr, // deprecated layer names
        static_cast<uint32_t>(deviceExtensionNames.size()),
        deviceExtensionNames.data(),
        &physicalDeviceFeatures
    };    

    vk::PhysicalDevice physicalDevice(PhysicalDevice::vkPhysicalDevice());
    mLogicalDevice = physicalDevice.createDevice({physicalDeviceCreateInfo});
}

std::vector<vk::DeviceQueueCreateInfo>
LogicalDevice::physicalDeviceQueuesCreateInfo(const float& queuePriority) {
    std::vector<vk::DeviceQueueCreateInfo> createInfoVector;

    // Add graphics support queue family
    vk::DeviceQueueCreateInfo queueCreateInfo =
    {
        vk::DeviceQueueCreateFlags(),
        PhysicalDevice::graphicsQueueFamilyIndex(),
        1,
        &queuePriority
    };
    createInfoVector.emplace_back(queueCreateInfo);

    // If graphics and presentation support queue family indices are different, 
    // we add a new create info for presentation queue.
    if (PhysicalDevice::graphicsQueueFamilyIndex() !=
        PhysicalDevice::presentationQueueFamilyIndex()) {
        queueCreateInfo.setQueueFamilyIndex(PhysicalDevice::presentationQueueFamilyIndex());
        createInfoVector.emplace_back(queueCreateInfo);

        // If presentation and transfer support queue family indices are different,
        // then we add a new create info for transfer queue.
        if (PhysicalDevice::presentationQueueFamilyIndex() !=
            PhysicalDevice::transferQueueFamilyIndex()) {
            queueCreateInfo.setQueueFamilyIndex(PhysicalDevice::transferQueueFamilyIndex());
            createInfoVector.emplace_back(queueCreateInfo);
        }
    } else if (PhysicalDevice::graphicsQueueFamilyIndex() !=
               PhysicalDevice::transferQueueFamilyIndex()) {
        // If graphics and transfer support queue family indices are different,
        // then we add a new create info for transfer queue.
        queueCreateInfo.setQueueFamilyIndex(PhysicalDevice::transferQueueFamilyIndex());
        createInfoVector.emplace_back(queueCreateInfo);
    }

    return createInfoVector;
}

void
LogicalDevice::initQueues() {
    vk::Device device(device());

    mGraphicsQueue = device.getQueue(PhysicalDevice::graphicsQueueFamilyIndex(),
                                     0);

    mTransferQueue = device.getQueue(PhysicalDevice::transferQueueFamilyIndex(),
                                     0);

    mPresentationQueue = device.getQueue(PhysicalDevice::presentationQueueFamilyIndex(),
                                         0);
}

}