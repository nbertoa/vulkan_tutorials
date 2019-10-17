#include "DescriptorPool.h"

#include <cassert>

#include "DebugUtils.h"
#include "LogicalDevice.h"

namespace vk {
DescriptorPool::DescriptorPool(const LogicalDevice& logicalDevice,
                               const VkDescriptorPoolSize& descriptorPoolSize,
                               const uint32_t maxSets,
                               const VkDescriptorPoolCreateFlags flags)
    : mLogicalDevice(logicalDevice)
{
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    descriptorPoolSizes.push_back(descriptorPoolSize);
    createPool(descriptorPoolSizes,
               maxSets,
               flags);
}

DescriptorPool::DescriptorPool(const LogicalDevice& logicalDevice,
                               const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                               const uint32_t maxSets,
                               const VkDescriptorPoolCreateFlags flags)
    : mLogicalDevice(logicalDevice) 
{
    createPool(descriptorPoolSizes,
               maxSets,
               flags);
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(mLogicalDevice.vkDevice(),
                            mDescriptorPool,
                            nullptr);
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mDescriptorPool(other.mDescriptorPool)
{
    other.mDescriptorPool = VK_NULL_HANDLE;
}

VkDescriptorPool
DescriptorPool::vkDescriptorPool() const {
    assert(mDescriptorPool != VK_NULL_HANDLE);
    return mDescriptorPool;
}

void 
DescriptorPool::createPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                           const uint32_t maxSets,
                           const VkDescriptorPoolCreateFlags flags) {
    assert(mDescriptorPool == VK_NULL_HANDLE);
    assert(descriptorPoolSizes.empty() == false);

    // VkDescriptorPoolCreateInfo:
    // - flags is a bitmask of VkDescriptorPoolCreateFlagBits specifying 
    //   certain supported operations on the pool.
    //   - VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT specifies that descriptor sets 
    //     can return their individual allocations to the pool, i.e. all of vkAllocateDescriptorSets, 
    //     vkFreeDescriptorSets, and vkResetDescriptorPool are allowed. Otherwise, descriptor sets 
    //     allocated from the pool must not be individually freed back to the pool, i.e. only 
    //     vkAllocateDescriptorSets and vkResetDescriptorPool are allowed.
    //   - VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT specifies that descriptor sets 
    //     allocated from this pool can include bindings with the 
    //     VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT bit set.
    //     It is valid to allocate descriptor sets that have bindings that do not set the 
    //     VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT bit from a pool that 
    //     has VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT set.
    // - maxSets is the maximum number of descriptor sets that can be allocated from the pool.
    // - poolSizeCount is the number of elements in pPoolSizes.
    // - pPoolSizes is a pointer to an array of VkDescriptorPoolSize structures, 
    //   each containing a descriptor type and number of descriptors of 
    //   that type to be allocated in the pool.
    //   - type is the type of descriptor.
    //   - descriptorCount is the number of descriptors of that type to allocate.
    //     If type is VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then descriptorCount 
    //     is the number of bytes to allocate for descriptors of this type.
    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    createInfo.pPoolSizes = descriptorPoolSizes.data();
    createInfo.maxSets = maxSets;
    createInfo.flags = flags;

    vkChecker(vkCreateDescriptorPool(mLogicalDevice.vkDevice(),
                                     &createInfo,
                                     nullptr,
                                     &mDescriptorPool));
}

}