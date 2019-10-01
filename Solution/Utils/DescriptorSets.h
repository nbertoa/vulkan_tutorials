#ifndef UTILS_DESCRIPTOR_SETS
#define UTILS_DESCRIPTOR_SETS

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class DescriptorPool;
class DescriptorSetLayout;
class LogicalDevice;

// VkDescriptorSet list wrapper
// Descriptors are grouped together into descriptor set objects.
// A descriptor set object is an opaque object that contains storage 
// for a set of descriptors, where the types and number of descriptors 
// is defined by a descriptor set layout.
// The layout object may be used to define the association of each 
// descriptor binding with memory or other hardware resources.
// The layout is used both for determining the resources that need 
// to be associated with the descriptor set, and determining the 
// interface between shader stagesand shader resources.
class DescriptorSets {
public:
    // - descriptorPool: is the pool which the descriptor sets
    //   will be allocated from.
    // - descriptorSetLayouts: Array of descriptor set layouts, with each
    //   member specifying how the corresponding descriptor set is allocated.
    DescriptorSets(const LogicalDevice& logicalDevice,
                   const DescriptorPool& descriptorPool,
                   const DescriptorSetLayout& descriptorSetLayout);
    DescriptorSets(const LogicalDevice& logicalDevice,
                   const DescriptorPool& descriptorPool,
                   const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
    DescriptorSets(DescriptorSets&& other) noexcept;
    DescriptorSets(const DescriptorSets&) = delete;
    const DescriptorSets& operator=(const DescriptorSets&) = delete;

    VkDescriptorSet vkDescriptorSet(const size_t descriptorSetIndex) const;

    // Once allocated, descriptor sets can be updated with a combination of write and copy operations.
    //
    // VkWriteDescriptorSet:
    // - dstSet is the destination descriptor set to update. This descriptor set must belong to
    //   this instance of the DescriptorSets class.
    // - dstBinding is the descriptor binding within that set.
    // - dstArrayElement is the starting element in that array.
    //   If the descriptor binding identified by dstSet and dstBinding has a descriptor type of 
    //   VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then dstArrayElement specifies the starting 
    //   byte offset within the binding.
    // - descriptorCount is the number of descriptors to update (the number of elements in pImageInfo, 
    //   pBufferInfo, or pTexelBufferView, or a value matching the dataSize member of an instance of 
    //   VkWriteDescriptorSetInlineUniformBlockEXT in the pNext chain, or a value matching the 
    //   accelerationStructureCount of an instance of VkWriteDescriptorSetAccelerationStructureNV 
    //   in the pNext chain).
    //   If the descriptor binding identified by dstSetand dstBinding has a descriptor type of 
    //   VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT then descriptorCount specifies the number of bytes to update.
    // - descriptorType is a VkDescriptorType specifying the type of each descriptor in pImageInfo, 
    //   pBufferInfo, or pTexelBufferView, as described below.
    //   It must be the same type as that specified in VkDescriptorSetLayoutBinding for dstSet at dstBinding.
    //   The type of the descriptor also controls which array the descriptors are taken from.
    // - pImageInfo is a pointer to an array of VkDescriptorImageInfo structures or is ignored.
    // - pBufferInfo is a pointer to an array of VkDescriptorBufferInfo structures or is ignored.
    // - pTexelBufferView is a pointer to an array of VkBufferView handles or is ignored.
    //
    // pBufferInfo: Possible options
    // VkDescriptorBufferInfo:
    // - buffer is the buffer resource.
    // - offset is the offset in bytes from the start of buffer.
    //   Access to buffer memory via this descriptor uses addressing that is relative to this starting offset.
    // - range is the size in bytes that is used for this descriptor update, 
    //   or VK_WHOLE_SIZE to use the range from offset to the end of the buffer.
    void updateDescriptorSet(const VkWriteDescriptorSet& writeDescriptorSet);
    void updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& writeDescriptorSets);

    void updateDescriptorSet(const VkCopyDescriptorSet& copyDescriptorSet);
    void updateDescriptorSet(const std::vector<VkCopyDescriptorSet>& copyDescriptorSets);

    void updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& writeDescriptorSets,
                             const std::vector<VkCopyDescriptorSet>& copyDescriptorSets);

private:
    void createDescriptorSets(const DescriptorPool& descriptorPool,
                              const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
                              
    const LogicalDevice& mLogicalDevice;
    std::vector<VkDescriptorSet> mDescriptorSets;
};
}

#endif 