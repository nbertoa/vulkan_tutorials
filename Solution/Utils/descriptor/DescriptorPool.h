#ifndef UTILS_DESCRIPTOR_DESCRIPTOR_POOL
#define UTILS_DESCRIPTOR_DESCRIPTOR_POOL

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkDescriptorPool wrapper.
//
// Before you create a descriptor set, its layout must be specified by creating
// a DescriptorSetLayout.
//
// You also need to create a DescriptorPool. It is a simple object used to allocate descriptor sets. 
// When creating a descriptor pool, you must specify the maximum number of 
// descriptor sets and descriptors of different types that you are going to allocate from it.
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
    // * descriptorPoolSizes:
    //
    //   - descriptorType (VK_DESCRIPTOR_TYPE_): 
    //     . SAMPLER, COMBINED_IMAGE_SAMPLER, SAMPLED_IMAGE, STORAGE_IMAGE, UNIFORM_TEXEL_BUFFER,
    //       TEXEL_BUFFER, UNIFORM_BUFFER, STORAGE_BUFFER, UNIFORM_BUFFER_DYNAMIC, BUFFER_DYNAMIC,
    //       INPUT_ATTACHMENT, INLINE_UNIFORM_BLOCK_EXT
    //
    //   - descriptorCount of that type to allocate.
    //     If type is VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then descriptorCount 
    //     is the number of bytes to allocate for descriptors of this type.
    //
    // * maxDescriptorSetCount that can be allocated from the pool.
    //
    // * flags bitmask specifying certain supported 
    //   operations on the pool (VK_DESCRIPTOR_POOL_CREATE_):
    //
    //   - FREE_DESCRIPTOR_SET_BIT, UPDATE_AFTER_BIND_BIT_EXT
    //
    // Notes:
    // If multiple VkDescriptorPoolSize structures appear in the poolSizes array 
    // then the pool will be created with enough storage for the total 
    // number of descriptors of each type.
    //
    // Fragmentation of a descriptor pool is possible and may lead to descriptor set allocation 
    // failures.
    // A failure due to fragmentation is defined as failing a descriptor set allocation 
    // despite the sum of all outstanding descriptor set allocations from the pool plus 
    // the requested allocation requiring 
    // no more than the total number of descriptors requested at pool creation.
    // Implementations provide certain guarantees of when fragmentation must not cause 
    // allocation failure.
    //
    // If a descriptor pool has not had any descriptor sets freed since it was created or 
    // most recently reset then fragmentation must not cause an allocation failure 
    // (note that this is always the case for a pool created without the 
    // VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT bit set).
    // Additionally, if all sets allocated from the pool since it was created or most 
    // recently reset use the same number of descriptors (of each type) and the requested 
    // allocation also uses that same number of descriptors(of each type), 
    // then fragmentation must not cause an allocation failure.
    //
    // If an allocation failure occurs due to fragmentation, an application can create an 
    // additional descriptor pool to perform further descriptor set allocations.
    //
    // If flags has the VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT bit set, 
    // descriptor pool creation may fail with the error VK_ERROR_FRAGMENTATION_EXT if the 
    // total number of descriptors across all pools(including this one) created with this bit 
    // set exceeds maxUpdateAfterBindDescriptorsInAllPools, 
    // or if fragmentation of the underlying hardware resources occurs.
    //
    // The global logical device is the device that creates the descriptor pool.
    DescriptorPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
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

    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
};
}

#endif 