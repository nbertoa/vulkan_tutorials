#include "Semaphore.h"

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
Semaphore::Semaphore(const LogicalDevice& logicalDevice)
    : mLogicalDevice(logicalDevice)
{
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkChecker(vkCreateSemaphore(mLogicalDevice.vkDevice(),
                                &createInfo,
                                nullptr,
                                &mSemaphore));
}

Semaphore::~Semaphore() {
    assert(mSemaphore != VK_NULL_HANDLE);
    vkDestroySemaphore(mLogicalDevice.vkDevice(),
                       mSemaphore,
                       nullptr);
}
}