#ifndef UTILS_PIPELINE_VERTEX_INPUT_STATE_FACTORY
#define UTILS_PIPELINE_VERTEX_INPUT_STATE_FACTORY

#include <vulkan/vulkan.h>

namespace vk {
// The vertex input state describes the format of the vertex data
// that will be passed to the vertex shader.
class PipelineVertexInputStateFactory {
public:
    PipelineVertexInputStateFactory() = delete;
    ~PipelineVertexInputStateFactory() = delete;
    PipelineVertexInputStateFactory(const PipelineVertexInputStateFactory&) = delete;
    const PipelineVertexInputStateFactory& operator=(const PipelineVertexInputStateFactory&) = delete;
    PipelineVertexInputStateFactory(PipelineVertexInputStateFactory&&) = delete;
    PipelineVertexInputStateFactory& operator=(PipelineVertexInputStateFactory&&) = delete;

    // Used when you hardcode vertex data directly in the vertex shader.
    static VkPipelineVertexInputStateCreateInfo noVertexDataCreateInfo();
};
}

#endif
