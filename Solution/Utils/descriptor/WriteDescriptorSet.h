#ifndef UTILS_DESCRIPTOR_WRITE_DESCRIPTOR_SET
#define UTILS_DESCRIPTOR_WRITE_DESCRIPTOR_SET

#include <vector>
#include <vulkan/vulkan.h>

//
// VkWriteDescriptorSet wrapper
//
// Describes an operation updating the descriptor set using descriptors 
// for resources specified in the structure.
//
// This is used by DescriptorSets
//

namespace vk {
class WriteDescriptorSet {
public:
    // * destDescriptorSet to update.
    //
    // * descriptorType of each descriptor in imageInfos, bufferInfos, or texelBufferViews.
    //   It must be the same type as that specified in VkDescriptorSetLayoutBinding 
    //   for destDescriptorSet at destBinding:
    //
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
    // * destBinding within the destDescriptorSet.
    // 
    // * destArrayStartIndex is the starting element in the array.
    //   If the descriptor binding identified by destDescriptorSet and destBinding has a descriptor type of 
    //   VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then destArrayStartIndex specifies the starting 
    //   byte offset within the binding.
    //
    // * imageInfos:
    //
    //   - sampler used in descriptor updates for types 
    //     VK_DESCRIPTOR_TYPE_SAMPLER and VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER if 
    //     the binding being updated does not use immutable samplers.
    //   - imageView used in descriptor updates for types 
    //     VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
    //     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT.
    //   - imageLayout that the image subresources accessible from imageView 
    //     will be in at the time this descriptor is accessed. imageLayout is used in descriptor 
    //     updates for types VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
    //     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT.
    //
    // * bufferInfos:
    //
    //   - buffer resource.
    //   - offset in bytes from the start of buffer.
    //     Access to buffer memory via this descriptor uses addressing that is relative to this starting offset.
    //   - range in bytes that is used for this descriptor update, 
    //     or VK_WHOLE_SIZE to use the range from offset to the end of the buffer.
    //
    // * texelBufferViews
    //
    // Notes:
    // - Arrays must not be empty
    WriteDescriptorSet(const std::vector<VkDescriptorImageInfo>& imageInfos,
                       const VkDescriptorSet destDescriptorSet,
                       const VkDescriptorType descriptorType,
                       const uint32_t destBinding,
                       const uint32_t destArrayStartIndex = 0);

    WriteDescriptorSet(const std::vector<VkDescriptorBufferInfo>& bufferInfos,
                       const VkDescriptorSet destDescriptorSet,
                       const VkDescriptorType descriptorType,
                       const uint32_t destBinding,
                       const uint32_t destArrayStartIndex = 0);

    WriteDescriptorSet(const std::vector<VkBufferView>& texelBufferViews,
                       const VkDescriptorSet destDescriptorSet,
                       const VkDescriptorType descriptorType,
                       const uint32_t destBinding,
                       const uint32_t destArrayStartIndex = 0);

    const VkWriteDescriptorSet& 
    vkDescriptorSet() const;

    VkWriteDescriptorSet&
    refVkDescriptorSet();

private:
    VkWriteDescriptorSet mDescriptorSet = {};
};
}

#endif 