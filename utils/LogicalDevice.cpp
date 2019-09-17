#include "LogicalDevice.h"

#include "DebugUtils.h"

namespace vk {
LogicalDevice::LogicalDevice(const AppInstance& appInstance, 
                             const Surface& surface)
    : mPhysicalDevice(new PhysicalDevice(appInstance, 
                                         surface)) {
    createLogicalDevice(*mPhysicalDevice);
    setQueues();
}

LogicalDevice::~LogicalDevice() {
    delete mPhysicalDevice;
    vkDestroyDevice(mLogicalDevice, nullptr);
}

LogicalDevice::LogicalDevice(LogicalDevice&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mGraphicsQueue(other.mGraphicsQueue)
    , mPresentationQueue(other.mPresentationQueue)
    , mPhysicalDevice(other.mPhysicalDevice)
{
    other.mLogicalDevice = VK_NULL_HANDLE;
    other.mGraphicsQueue = VK_NULL_HANDLE;
    other.mPresentationQueue = VK_NULL_HANDLE;
    mPhysicalDevice = nullptr;
}

void
LogicalDevice::createLogicalDevice(const PhysicalDevice& physicalDevice) {
    assert(mLogicalDevice == VK_NULL_HANDLE);
    assert(physicalDevice.vkPhysicalDevice() != VK_NULL_HANDLE);

    const std::vector<VkDeviceQueueCreateInfo> createInfoVector =
        buildDeviceQueueCreateInfoVector(physicalDevice);
    const std::vector<const char*>& deviceExtensions = physicalDevice.deviceExtensions();

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

    VkDeviceCreateInfo physicalDeviceCreateInfo = {};
    physicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    physicalDeviceCreateInfo.pQueueCreateInfos = createInfoVector.data();
    physicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(createInfoVector.size());
    physicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    physicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    physicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    physicalDeviceCreateInfo.enabledLayerCount = 0;
    physicalDeviceCreateInfo.ppEnabledLayerNames = nullptr;

    vkChecker(vkCreateDevice(physicalDevice.vkPhysicalDevice(),
                             &physicalDeviceCreateInfo, 
                             nullptr, 
                             &mLogicalDevice));
    assert(mLogicalDevice != VK_NULL_HANDLE);
}

std::vector<VkDeviceQueueCreateInfo>
LogicalDevice::buildDeviceQueueCreateInfoVector(const PhysicalDevice& physicalDevice) {
    assert(physicalDevice.vkPhysicalDevice() != VK_NULL_HANDLE);

    std::vector<VkDeviceQueueCreateInfo> createInfoVector;

    // Add graphics support queue family
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = physicalDevice.graphicsSupportQueueFamilyIndex();
    queueCreateInfo.queueCount = 1;
    const float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    createInfoVector.emplace_back(queueCreateInfo);

    // Only if graphics and presentation support queue family indices are different, 
    // we add a new create info.
    if (physicalDevice.graphicsSupportQueueFamilyIndex() != 
        physicalDevice.presentationSupportQueueFamilyIndex()) {
        queueCreateInfo.queueFamilyIndex = physicalDevice.presentationSupportQueueFamilyIndex();
        createInfoVector.emplace_back(queueCreateInfo);
    }

    return createInfoVector;
}

void
LogicalDevice::setQueues() {
    assert(mLogicalDevice != VK_NULL_HANDLE);
    assert(mPhysicalDevice != nullptr);

    vkGetDeviceQueue(mLogicalDevice,
                     mPhysicalDevice->graphicsSupportQueueFamilyIndex(),
                     0,
                     &mGraphicsQueue);
    assert(mGraphicsQueue != VK_NULL_HANDLE);

    vkGetDeviceQueue(mLogicalDevice,
                     mPhysicalDevice->transferSupportQueueFamilyIndex(),
                     0,
                     &mTransferQueue);
    assert(mTransferQueue != VK_NULL_HANDLE);

    vkGetDeviceQueue(mLogicalDevice,
                     mPhysicalDevice->presentationSupportQueueFamilyIndex(),
                     0,
                     &mPresentationQueue);
    assert(mPresentationQueue != VK_NULL_HANDLE);
}
}