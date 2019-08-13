#ifndef UTILS_PIPELINE_INPUT_ASSEMBLY_STATE_FACTORY
#define UTILS_PIPELINE_INPUT_ASSEMBLY_STATE_FACTORY

#include <vulkan/vulkan.h>

namespace vk {
class PipelineInputAssemblyStateFactory {
public:
    PipelineInputAssemblyStateFactory() = delete;
    ~PipelineInputAssemblyStateFactory() = delete;
    PipelineInputAssemblyStateFactory(const PipelineInputAssemblyStateFactory&) = delete;
    const PipelineInputAssemblyStateFactory& operator=(const PipelineInputAssemblyStateFactory&) = delete;
    PipelineInputAssemblyStateFactory(PipelineInputAssemblyStateFactory&&) = delete;
    PipelineInputAssemblyStateFactory& operator=(PipelineInputAssemblyStateFactory&&) = delete;

    static VkPipelineInputAssemblyStateCreateInfo createInfo(const VkPrimitiveTopology primitiveTopology,
                                                             const VkBool32 primitiveRestartEnable = VK_FALSE);
};
}

#endif