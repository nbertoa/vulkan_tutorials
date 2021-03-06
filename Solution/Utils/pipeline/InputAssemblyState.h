#ifndef UTILS_PIPELINE_STAGE_INPUT_ASSEMBLY_STATE
#define UTILS_PIPELINE_STAGE_INPUT_ASSEMBLY_STATE

#include <vulkan/vulkan.hpp>

namespace vulkan {
//
// PipelineInputAssemblyStateCreateInfo wrapper
//
// The PipelineInputAssemblyStateCreateInfo struct describes two things: 
// - What kind of geometry will be drawn from the vertices
// - If primitive restart should be enabled.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class InputAssemblyState {
public:
    // * primitiveRestartEnable controls whether a special vertex index value is 
    //   treated as restarting the assembly of primitives. 
    //   This enable only applies to indexed draws (vkCmdDrawIndexed and vkCmdDrawIndexedIndirect), 
    //   and the special index value is either 0xFFFFFFFF when the indexType parameter of 
    //   vkCmdBindIndexBuffer is equal to VK_INDEX_TYPE_UINT32, 0xFF when indexType is equal to 
    //   VK_INDEX_TYPE_UINT8_EXT, or 0xFFFF when indexType is equal to VK_INDEX_TYPE_UINT16. 
    //   Primitive restart is not allowed for �list� topologies.
    InputAssemblyState(const vk::PrimitiveTopology primitiveTopology = vk::PrimitiveTopology::eTriangleList ,
                       const vk::Bool32 primitiveRestartEnable = false);

    const vk::PipelineInputAssemblyStateCreateInfo&
    state() const;

private:
    vk::PipelineInputAssemblyStateCreateInfo mCreateInfo = {};
};
}

#endif 