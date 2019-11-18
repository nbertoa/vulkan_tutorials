#ifndef UTILS_DESCRIPTOR_COPY_DESCRIPTOR_SET
#define UTILS_DESCRIPTOR_COPY_DESCRIPTOR_SET

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkWriteDescriptorSet wrapper
//
// Describes an describes an operation copying descriptors between sets. 
//
// This is used by DescriptorSets
//
class CopyDescriptorSet {
public:
    // * sourceDescriptorSet, sourceBinding, and sourceArrayElement are the source set, binding, 
    //   and array element, respectively.
    //   If the descriptor binding identified by sourceDescriptorSet and sourceBinding 
    //   has a descriptor type of VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then 
    //   sourceArrayElement specifies the starting byte offset within the binding to copy from.
    //
    // * destSet, destBinding, and destArrayElement are the destination set, binding, 
    //   and array element, respectively.
    //   If the descriptor binding identified by destSet and destBinding has a descriptor type of 
    //   VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then destArrayElement specifies 
    //   the starting byte offset within the binding to copy to.
    //
    // * descriptorCount to copy from the source to destination.
    //   If descriptorCount is greater than the number of remaining array elements 
    //   in the source or destination binding, 
    //   those affect consecutive bindings in a manner similar to WriteDescriptorSet.
    //   If the descriptor binding identified by sourceDescriptorSet and sourceBinding 
    //   has a descriptor type of VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then 
    //   descriptorCount specifies the number of bytes to copy and the 
    //   remaining array elements in the source or destination binding 
    //   refer to the remaining number of bytes in those.
    CopyDescriptorSet(const VkDescriptorSet sourceDescriptorSet,
                      const uint32_t sourceBinding,
                      const uint32_t sourceArrayElement,
                      const VkDescriptorSet destDescriptorSet,
                      const uint32_t destBinding,
                      const uint32_t destArrayElement,
                      const uint32_t descriptorCount);

    const VkCopyDescriptorSet&
    vkDescriptorSet() const;

    VkCopyDescriptorSet&
    refVkDescriptorSet();

private:
    VkCopyDescriptorSet mDescriptorSet = {};
};
}

#endif 