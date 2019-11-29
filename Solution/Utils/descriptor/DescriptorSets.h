#ifndef UTILS_DESCRIPTOR_DESCRIPTOR_SETS
#define UTILS_DESCRIPTOR_DESCRIPTOR_SETS

#include <vector>
#include <vulkan/vulkan.h>

namespace vk2 {
class CopyDescriptorSet;
class DescriptorPool;
class DescriptorSetLayout;
class WriteDescriptorSet;

//
// VkDescriptorSet list wrapper
//
// To allocate a DescriptorSet, you need both DescriptorPool and DescriptorSetLayout 
// to be able to do it. The DescriptorSet represents memory that holds actual descriptors, 
// and it can be configured so that a descriptor points to specific Buffer, 
// BufferView, Image or Sampler.
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
    // * descriptorSetLayouts with each member specifying how the corresponding 
    //   descriptor set is allocated.
    //
    // Notes: The global logical device is the device that owns the descriptor pool
    DescriptorSets(const DescriptorPool& descriptorPool,
                   const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
    DescriptorSets(DescriptorSets&& other) noexcept;
    DescriptorSets(const DescriptorSets&) = delete;
    const DescriptorSets& operator=(const DescriptorSets&) = delete;

    VkDescriptorSet 
    operator[](const uint32_t index) const;

    uint32_t
    size() const;

    // Once allocated, descriptor sets can be updated with a combination of write 
    // and copy operations.
    //
    // * writeDescriptorSets to write to. The operations described by writeDescriptorSets are 
    //   performed first, followed by the operations described by copyDescriptorSets. 
    //   Within each array, the operations are performed in the order they appear in the array. 
    //   Each element in the writeDescriptorSets array describes an operation 
    //   updating the descriptor set using descriptors for resources specified in the structure
    //
    // * copyDescriptorSets describes an operation copying descriptors between sets. 
    //   If the dstSet member of any element of writeDescriptorSets or copyDescriptorSets is bound, accessed, 
    //   or modified by any command that was recorded to a command buffer which is currently 
    //   in the recording or executable state, and any of the descriptor bindings that are 
    //   updated were not created with the VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT or 
    //   VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT bits set, 
    //   that command buffer becomes invalid.
    //
    // Preconditions:
    // The methods that do not take a descriptorSetIndex must refer to a
    // descriptor set that belongs to this instance of DescriptorSets.
    void 
    updateDescriptorSet(const WriteDescriptorSet& writeDescriptorSet);

    void
    updateDescriptorSet(const uint32_t descriptorSetIndex,
                        WriteDescriptorSet& writeDescriptorSet);
    
    void 
    updateDescriptorSet(const std::vector<WriteDescriptorSet>& writeDescriptorSets);

    void 
    updateDescriptorSet(const CopyDescriptorSet& copyDescriptorSet);
    
    void 
    updateDescriptorSet(const std::vector<CopyDescriptorSet>& copyDescriptorSets);

    void 
    updateDescriptorSet(const std::vector<WriteDescriptorSet>& writeDescriptorSets,
                        const std::vector<CopyDescriptorSet>& copyDescriptorSets);

private:                              
    std::vector<VkDescriptorSet> mDescriptorSets;
};
}

#endif 