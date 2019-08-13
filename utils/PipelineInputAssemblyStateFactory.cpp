#include "PipelineInputAssemblyStateFactory.h"

namespace vk {
VkPipelineInputAssemblyStateCreateInfo
PipelineInputAssemblyStateFactory::createInfo(const VkPrimitiveTopology primitiveTopology,
                                              const VkBool32 primitiveRestartEnable) {
    VkPipelineInputAssemblyStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.topology = primitiveTopology;
    createInfo.primitiveRestartEnable = primitiveRestartEnable;

    return createInfo;
}
}