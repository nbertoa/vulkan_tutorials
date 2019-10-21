#ifndef UTILS_DESCRIPTOR_SETS
#define UTILS_DESCRIPTOR_SETS

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class DescriptorPool;
class DescriptorSetLayout;
class LogicalDevice;

//
// VkDescriptorSet list wrapper
//
// To allocate a DescriptorSet, you need both DescriptorPool and DescriptorSetLayout 
// to be able to do it. The DescriptorSet represents memory that holds actual descriptors, 
// and it can be configured so that a descriptor points to specific Buffer, BufferView, Image or Sampler.
// You can do it by using function vkUpdateDescriptorSets.
//
// Descriptors are grouped together into descriptor set objects.
// A descriptor set object is an opaque object that contains storage 
// for a set of descriptors, where the types and number of descriptors 
// is defined by a descriptor set layout.
//
// The layout object may be used to define the association of each 
// descriptor binding with memory or other hardware resources.
// The layout is used both for determining the resources that need 
// to be associated with the descriptor set, and determining the 
// interface between shader stagesand shader resources.
//
// You need the DescriptorSets to:
// - Bind them to a CommandBuffer through vkCmdBindDescriptorSets
//
// To create/use the DescriptorSets you need:
// - DescriptorSetLayout
// - DescriptorPool
// - BufferView, ImageView, Sampler, Buffer for vkUpdateDescriptorSets
//
class DescriptorSets {
public:
    // * descriptorPool which the descriptor sets will be allocated from.
    //
    // * descriptorSetLayouts with each member specifying how the corresponding descriptor set is allocated.
    DescriptorSets(const LogicalDevice& logicalDevice,
                   const DescriptorPool& descriptorPool,
                   const DescriptorSetLayout& descriptorSetLayout);
    DescriptorSets(const LogicalDevice& logicalDevice,
                   const DescriptorPool& descriptorPool,
                   const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
    DescriptorSets(DescriptorSets&& other) noexcept;
    DescriptorSets(const DescriptorSets&) = delete;
    const DescriptorSets& operator=(const DescriptorSets&) = delete;

    VkDescriptorSet 
    vkDescriptorSet(const size_t descriptorSetIndex) const;

    // Once allocated, descriptor sets can be updated with a combination of write and copy operations.
    //
    // * writeDescriptorSets to write to. The operations described by writeDescriptorSets are performed first, 
    //   followed by the operations described by copyDescriptorSets. Within each array, the operations are performed in 
    //   the order they appear in the array. Each element in the writeDescriptorSets array describes an operation 
    //   updating the descriptor set using descriptors for resources specified in the structure:    
    //
    //   - dstSet to update. This descriptor set must belong to this instance of the DescriptorSets class.
    //   - dstBinding within that set.
    //   - dstArrayElement is the starting element in that array.
    //     If the descriptor binding identified by dstSet and dstBinding has a descriptor type of 
    //     VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then dstArrayElement specifies the starting 
    //     byte offset within the binding.
    //   - descriptorCount to update (the number of elements in pImageInfo, 
    //     pBufferInfo, or pTexelBufferView).
    //     If the descriptor binding identified by dstSet and dstBinding has a descriptor type of 
    //     VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then descriptorCount specifies the number of bytes to update.
    //   - descriptorType of each descriptor in pImageInfo, pBufferInfo, or pTexelBufferView.
    //     It must be the same type as that specified in VkDescriptorSetLayoutBinding for dstSet at dstBinding.
    //     The type of the descriptor also controls which array the descriptors are taken from.
    //   - pImageInfo is an optional array of VkDescriptorImageInfo structures or is ignored:
    //     . sampler used in descriptor updates for types 
    //       VK_DESCRIPTOR_TYPE_SAMPLERand VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER if 
    //       the binding being updated does not use immutable samplers.
    //     . imageView used in descriptor updates for types 
    //       VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
    //       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT.
    //     . imageLayout that the image subresources accessible from imageView 
    //       will be in at the time this descriptor is accessed. imageLayout is used in descriptor 
    //       updates for types VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
    //       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT.
    //   - pBufferInfo is an optional array of VkDescriptorBufferInfo structures or is ignored:
    //     . buffer resource.
    //     . offset in bytes from the start of buffer.
    //       Access to buffer memory via this descriptor uses addressing that is relative to this starting offset.
    //     . range in bytes that is used for this descriptor update, 
    //       or VK_WHOLE_SIZE to use the range from offset to the end of the buffer.
    //   - pTexelBufferView is an optional array of VkBufferView.
    //
    // * copyDescriptorSets describes an operation copying descriptors between sets. 
    //   If the dstSet member of any element of writeDescriptorSets or copyDescriptorSets is bound, accessed, 
    //   or modified by any command that was recorded to a command buffer which is currently in the recording 
    //   or executable state, and any of the descriptor bindings that are updated were not created with the 
    //   VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT or VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT bits set, 
    //   that command buffer becomes invalid.
    void 
    updateDescriptorSet(const VkWriteDescriptorSet& writeDescriptorSet);
    
    void 
    updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& writeDescriptorSets);

    void 
    updateDescriptorSet(const VkCopyDescriptorSet& copyDescriptorSet);
    
    void 
    updateDescriptorSet(const std::vector<VkCopyDescriptorSet>& copyDescriptorSets);

    void 
    updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& writeDescriptorSets,
                        const std::vector<VkCopyDescriptorSet>& copyDescriptorSets);

private:
    void 
    createDescriptorSets(const DescriptorPool& descriptorPool,
                         const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
                              
    const LogicalDevice& mLogicalDevice;
    std::vector<VkDescriptorSet> mDescriptorSets;
};
}

#endif 