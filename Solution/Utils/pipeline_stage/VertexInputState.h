#ifndef UTILS_PIPELINE_STAGE_VERTEX_INPUT_STATE
#define UTILS_PIPELINE_STAGE_VERTEX_INPUT_STATE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkPipelineVertexInputStateCreateInfo wrapper
//
// Applications specify vertex input attribute and 
// vertex input binding descriptions as part of graphics pipeline creation.
//
// The VkPipelineVertexInputStateCreateInfo structure describes the format of the vertex data 
// that will be passed to the vertex shader. It describes this in roughly two ways:
// - Bindings: spacing between data and whether the data is per-vertex or per-instance
// - Attribute descriptions: type of the attributes passed to the vertex shader, 
//   which binding to load them from and at which offset
//
// You need this class to:
// - Create the GraphicsPipeline
//
class VertexInputState {
public:
    // * bindingDescriptions of vertices (spacing between data and whether 
    //   the data is per-vertex or per-instance):
    //
    //   - binding number that this structure describes.
    //   - stride is the distance in bytes between two consecutive elements within the buffer.
    //   - inputRate specifies whether vertex attribute addressing is a 
    //     function of the vertex index or of the instance index.
    //
    // * attributeDescriptions of vertices (type of the attributes passed to the vertex shader, 
    //   which binding to load them from and at which offset):
    //
    //   - location is the shader binding location number for this attribute.
    //   - binding number which this attribute takes its data from.
    //   - format is the size and type of the vertex attribute data.
    //   - offset in bytes of this attribute relative to the start of an element in the vertex input binding. 
    VertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions = {},
                     const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions = {});
    VertexInputState(const VertexInputState& state);
    const VertexInputState& operator=(const VertexInputState& state);

    const VkPipelineVertexInputStateCreateInfo& 
    vkState() const;

private:
    VkPipelineVertexInputStateCreateInfo mCreateInfo = {};

    std::vector<VkVertexInputBindingDescription> mBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> mAttributeDescriptions;
};
}

#endif 