#ifndef UTILS_DESCRIPTOR_POOL
#define UTILS_DESCRIPTOR_POOL

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;

// VkDescriptorPool wrapper.
// A descriptor pool maintains a pool of descriptors, from which descriptor 
// sets are allocated. 
// Descriptor pools are externally synchronized, meaning that the application 
// must not allocate and/or free descriptor sets from the same pool in multiple 
// threads simultaneously.
class DescriptorPool {
public:
    // VkDescriptorPoolSize:
    // - type is the type of descriptor.
    //   - VK_DESCRIPTOR_TYPE_SAMPLER specifies a sampler descriptor.
    //   - VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER specifies a combined image sampler descriptor.
    //   - VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE specifies a sampled image descriptor.
    //   - VK_DESCRIPTOR_TYPE_STORAGE_IMAGE specifies a storage image descriptor.
    //   - VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER specifies a uniform texel buffer descriptor.
    //   - VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER specifies a storage texel buffer descriptor.
    //   - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER specifies a uniform buffer descriptor.
    //   - VK_DESCRIPTOR_TYPE_STORAGE_BUFFER specifies a storage buffer descriptor.
    //   - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC specifies a dynamic uniform buffer descriptor.
    //   - VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC specifies a dynamic storage buffer descriptor.
    //   - VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT specifies an input attachment descriptor.
    //   - VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT specifies an inline uniform block.
    // - descriptorCount is the number of descriptors of that type to allocate.
    //   If type is VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then descriptorCount 
    //   is the number of bytes to allocate for descriptors of this type.
    // - maxSets is the maximum number of descriptor sets that can be allocated from the pool.
    // - flags is a bitmask of VkDescriptorPoolCreateFlagBits specifying certain supported operations on the pool.
    //     - VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT specifies that descriptor sets 
    //       can return their individual allocations to the pool, i.e. all of vkAllocateDescriptorSets, 
    //       vkFreeDescriptorSets, and vkResetDescriptorPool are allowed. 
    //       Otherwise, descriptor sets allocated from the pool must not be individually 
    //       freed back to the pool, i.e. only vkAllocateDescriptorSets and vkResetDescriptorPool are allowed.
    //     - VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT specifies that descriptor sets allocated from 
    //       this pool can include bindings with the VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT bit set.
    //       It is valid to allocate descriptor sets that have bindings that do not set the 
    //       VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT bit from a pool that has 
    //       VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT set.
    DescriptorPool(const LogicalDevice& logicalDevice,
                   const VkDescriptorPoolSize& descriptorPoolSize,
                   const uint32_t maxSets,
                   const VkDescriptorPoolCreateFlags flags = 0);
    DescriptorPool(const LogicalDevice& logicalDevice,
                   const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                   const uint32_t maxSets,
                   const VkDescriptorPoolCreateFlags flags = 0);
    ~DescriptorPool();
    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool(const DescriptorPool&) = delete;
    const DescriptorPool& operator=(const DescriptorPool&) = delete;

    VkDescriptorPool vkDescriptorPool() const;

private:
    void createPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
                    const uint32_t maxSets,
                    const VkDescriptorPoolCreateFlags flags);

    const LogicalDevice& mLogicalDevice;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
};
}

#endif 