#ifndef UTILS_PIPELINE_COLOR_BLEND_STATE_FACTORY
#define UTILS_PIPELINE_COLOR_BLEND_STATE_FACTORY

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class PipelineColorBlendStateFactory {
public:
    PipelineColorBlendStateFactory() = delete;
    ~PipelineColorBlendStateFactory() = delete;
    PipelineColorBlendStateFactory(const PipelineColorBlendStateFactory&) = delete;
    const PipelineColorBlendStateFactory& operator=(const PipelineColorBlendStateFactory&) = delete;
    PipelineColorBlendStateFactory(PipelineColorBlendStateFactory&&) = delete;
    PipelineColorBlendStateFactory& operator=(PipelineColorBlendStateFactory&&) = delete;
        
    static VkPipelineColorBlendAttachmentState disableBlending();
    static VkPipelineColorBlendAttachmentState enableBlending();

    static VkPipelineColorBlendStateCreateInfo 
    pipelineColorBlendStateCreateInfo(const VkPipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState);

    static VkPipelineColorBlendStateCreateInfo 
    pipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& pipelineColorBlendAttachmentStates);
};
}

#endif
