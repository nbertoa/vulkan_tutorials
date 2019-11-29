#ifndef UTILS_PIPELINE_STAGE_INPUT_ASSEMBLY_STATE
#define UTILS_PIPELINE_STAGE_INPUT_ASSEMBLY_STATE

#include <vulkan/vulkan.h>

namespace vk2 {
//
// VkPipelineInputAssemblyStateCreateInfo wrapper
//
// The VkPipelineInputAssemblyStateCreateInfo struct describes two things: 
// - What kind of geometry will be drawn from the vertices
// - If primitive restart should be enabled.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class InputAssemblyState {
public:
    // * primitiveTopology (VK_PRIMITIVE_TOPOLOGY_):
    //
    //   - POINT_LIST, LINE_LIST, LINE_STRIP, TRIANGLE_LIST, TRIANGLE_STRIP, TRIANGLE_FAN, 
    //     LINE_LIST_WITH_ADJACENCY, LINE_STRIP_WITH_ADJACENCY, TRIANGLE_LIST_WITH_ADJACENCY, 
    //     TRIANGLE_STRIP_WITH_ADJACENCY, PATCH_LIST.
    //
    // * primitiveRestartEnable controls whether a special vertex index value is 
    //   treated as restarting the assembly of primitives. 
    //   This enable only applies to indexed draws (vkCmdDrawIndexed and vkCmdDrawIndexedIndirect), 
    //   and the special index value is either 0xFFFFFFFF when the indexType parameter of 
    //   vkCmdBindIndexBuffer is equal to VK_INDEX_TYPE_UINT32, 0xFF when indexType is equal to 
    //   VK_INDEX_TYPE_UINT8_EXT, or 0xFFFF when indexType is equal to VK_INDEX_TYPE_UINT16. 
    //   Primitive restart is not allowed for “list” topologies.
    InputAssemblyState(const VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                       const VkBool32 primitiveRestartEnable = false);

    const VkPipelineInputAssemblyStateCreateInfo&
    vkState() const;

private:
    VkPipelineInputAssemblyStateCreateInfo mCreateInfo = {};
};
}

#endif 