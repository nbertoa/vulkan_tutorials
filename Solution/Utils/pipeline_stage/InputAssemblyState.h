#ifndef UTILS_PIPELINE_STAGE_INPUT_ASSEMBLY_STATE
#define UTILS_PIPELINE_STAGE_INPUT_ASSEMBLY_STATE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkPipelineInputAssemblyStateCreateInfo wrapper
//
// The VkPipelineInputAssemblyStateCreateInfo struct describes two things: 
// what kind of geometry will be drawn from the vertices and if primitive restart should be enabled.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class InputAssemblyState {
public:
    // * primitiveTopology:
    //
    //   - VK_PRIMITIVE_TOPOLOGY_POINT_LIST specifies a series of separate point primitives.
    //   - VK_PRIMITIVE_TOPOLOGY_LINE_LIST specifies a series of separate line primitives.
    //   - VK_PRIMITIVE_TOPOLOGY_LINE_STRIP specifies a series of connected line primitives 
    //     with consecutive lines sharing a vertex.
    //   - VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST specifies a series of separate triangle primitives.
    //   - VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP specifies a series of connected triangle 
    //     primitives with consecutive triangles sharing an edge.
    //   - VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN specifies a series of connected triangle 
    //     primitives with all triangles sharing a common vertex.
    //   - VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY specifies a series 
    //     separate line primitives with adjacency.
    //   - VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY specifies a series connected line primitives 
    //     with adjacency, with consecutive primitives sharing three vertices.
    //   - VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY specifies a series separate 
    //     triangle primitives with adjacency.
    //   - VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY specifies connected triangle primitives 
    //     with adjacency, with consecutive triangles sharing an edge.
    //   - VK_PRIMITIVE_TOPOLOGY_PATCH_LIST specifies separate patch primitives.
    //
    // * primitiveRestartEnable controls whether a special vertex index value is treated as restarting the 
    //   assembly of primitives. This enable only applies to indexed draws (vkCmdDrawIndexed and vkCmdDrawIndexedIndirect), 
    //   and the special index value is either 0xFFFFFFFF when the indexType parameter of vkCmdBindIndexBuffer 
    //   is equal to VK_INDEX_TYPE_UINT32, 0xFF when indexType is equal to VK_INDEX_TYPE_UINT8_EXT, or 0xFFFF when 
    //   indexType is equal to VK_INDEX_TYPE_UINT16. Primitive restart is not allowed for “list” topologies.
    InputAssemblyState(const VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                       const VkBool32 primitiveRestartEnable = false);

    const VkPipelineInputAssemblyStateCreateInfo&
    vkState() const;

private:
    VkPipelineInputAssemblyStateCreateInfo mCreateInfo = {};
};
}

#endif 