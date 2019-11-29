#include "Fence.h"

#include <cassert>

#include "../DebugUtils.h"
#include "../device/LogicalDevice.h"

namespace vk2 {
Fence::Fence(const VkFenceCreateFlags flags) {

    // - flags is a bitmask of VkFenceCreateFlagBits specifying the 
    //   initial state and behavior of the fence.
    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = flags;

    vkChecker(vkCreateFence(LogicalDevice::vkDevice(),
                            &createInfo,
                            nullptr,
                            &mFence));
}

Fence::Fence(Fence&& other) noexcept
    : mFence(std::move(other.mFence)) {
    other.mFence = VK_NULL_HANDLE;
}

Fence::~Fence() {
    vkDestroyFence(LogicalDevice::vkDevice(),
                   mFence,
                   nullptr);
}

const VkFence& 
Fence::vkFence() const {
    assert(mFence != VK_NULL_HANDLE);
    return mFence;
}

void
Fence::wait() const {
    assert(mFence != VK_NULL_HANDLE);

    vkChecker(vkWaitForFences(LogicalDevice::vkDevice(),
                              1,
                              &mFence,
                              VK_TRUE,
                              std::numeric_limits<uint64_t>::max()));
}

void
Fence::reset() const {
    assert(mFence != VK_NULL_HANDLE);

    vkChecker(vkResetFences(LogicalDevice::vkDevice(),
                            1,
                            &mFence));
}

void 
Fence::waitAndReset() const {
    assert(mFence != VK_NULL_HANDLE);

    vkChecker(vkWaitForFences(LogicalDevice::vkDevice(),
                              1,
                              &mFence,
                              VK_TRUE,
                              std::numeric_limits<uint64_t>::max()));
    vkChecker(vkResetFences(LogicalDevice::vkDevice(),
                            1,
                            &mFence));
}
}