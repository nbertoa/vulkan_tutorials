#ifndef UTILS_DESCRIPTOR_POOL
#define UTILS_DESCRIPTOR_POOL

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

//
// VkDescriptorPool wrapper.
//
// Before you create a descriptor set, its layout must be specified by creating
// a DescriptorSetLayout.
//
// You also need to create a DescriptorPool. It is a simple object used to allocate descriptor sets. 
// When creating a descriptor pool, you must specify the maximum number of descriptor sets and descriptors 
// of different types that you are going to allocate from it.
//
// A descriptor pool maintains a pool of descriptors, from which descriptor 
// sets are allocated. 
// Descriptor pools are externally synchronized, meaning that the application 
// must not allocate and/or free descriptor sets from the same pool in multiple 
// threads simultaneously.
//
// You need the DescriptorPool to:
// - Create a DescriptorSet
//
class DescriptorPool {
public:
    // * descriptorType: 
    //   - VK_DESCRIPTOR_TYPE_SAMPLER
    //   - VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    //   - VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
    //   - VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
    //   - VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
    //   - VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
    //   - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    //   - VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    //   - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    //   - VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
    //   - VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
    //   - VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT
    //
    // * descriptorCount of that type to allocate.
    //   If type is VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then descriptorCount 
    //   is the number of bytes to allocate for descriptors of this type.
    //
    // * maxDescriptorSetCount that can be allocated from the pool.
    //
    // * flags bitmask specifying certain supported operations on the pool:
    //
    //   - VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT specifies that descriptor sets 
    //     can return their individual allocations to the pool, i.e. all of vkAllocateDescriptorSets, 
    //     vkFreeDescriptorSets, and vkResetDescriptorPool are allowed. 
    //     Otherwise, descriptor sets allocated from the pool must not be individually 
    //     freed back to the pool, i.e. only vkAllocateDescriptorSets and vkResetDescriptorPool are allowed.
    //   - VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT specifies that descriptor sets allocated from 
    //     this pool can include bindings with the VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT bit set.
    //     It is valid to allocate descriptor sets that have bindings that do not set the 
    //     VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT bit from a pool that has 
    //     VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT set.
    DescriptorPool(const LogicalDevice& logicalDevice,
                   const VkDescriptorType descriptorType,
                   const uint32_t descriptorCount,
                   const uint32_t maxDescriptorSetCount,
                   const VkDescriptorPoolCreateFlags flags = 0);
    DescriptorPool(const LogicalDevice& logicalDevice,
                   const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                   const uint32_t maxDescriptorSetCount,
                   const VkDescriptorPoolCreateFlags flags = 0);
    ~DescriptorPool();
    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool(const DescriptorPool&) = delete;
    const DescriptorPool& operator=(const DescriptorPool&) = delete;

    VkDescriptorPool 
    vkDescriptorPool() const;

private:
    void 
    createPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
               const uint32_t maxDescriptorSetCount,
               const VkDescriptorPoolCreateFlags flags);

    const LogicalDevice& mLogicalDevice;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
};
}

#endif 