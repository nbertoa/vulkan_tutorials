#ifndef UTILS_PIPELINE_VERTEX_INPUT_STATE_FACTORY
#define UTILS_PIPELINE_VERTEX_INPUT_STATE_FACTORY

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
// Factory to easily get most used pipeline states.
class PipelineStateFactory {
public:
    PipelineStateFactory() = delete;
    ~PipelineStateFactory() = delete;
    PipelineStateFactory(const PipelineStateFactory&) = delete;
    const PipelineStateFactory& operator=(const PipelineStateFactory&) = delete;
    PipelineStateFactory(PipelineStateFactory&&) = delete;
    PipelineStateFactory& operator=(PipelineStateFactory&&) = delete;

    // The vertex input state describes the format of the vertex data
    // that will be passed to the vertex shader:
    //
    // Used when you hardcode vertex data directly in the vertex shader.
    static void emptyVertexInputState(VkPipelineVertexInputStateCreateInfo& createInfo);

    static void 
    vertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
                     const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions,
                     VkPipelineVertexInputStateCreateInfo& createInfo);

    // The rasterizer takes the geometry that is shaped by the vertices 
    // from the vertex shader and turns into fragments to be colored by 
    // the fragment shader:
    //
    // No depth clamping
    // Fill polygon
    // Back face culling
    // Clockwise Front-facing
    // No depth value biasing
    static void 
    defaultRasterizationState(VkPipelineRasterizationStateCreateInfo& createInfo);

    // The multisampling is one of the ways to perform anti-aliasing.
    // It works by combining the fragment shader results of multiple polygons
    // that rasterize to the same pixel.
    // This mainly occurs along edges, which is also where the most noticeable
    // multiple times of only one polygon maps to a pixel, it is significantly
    // less expensive than simply rendering to a higher resolution and then 
    // downscaling:
    static void
    disableMultisampleState(VkPipelineMultisampleStateCreateInfo& createInfo);

    // The input assembly describes what kind of geometry will be drawn from
    // the vertices and if primitive restart should be enabled:
    static void 
    createInputAssemblyState(const VkPrimitiveTopology primitiveTopology,
                             const VkBool32 primitiveRestartEnable,
                             VkPipelineInputAssemblyStateCreateInfo& createInfo);

    // Depth and stencil tests:
    //
    // Enable depth test
    // Disable stencil test
    static void 
    defaultDepthStencilState(VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo);

    // Color blending:
    static void disableColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& state);
    static void enableColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& state);

    static void
    colorBlendState(const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState,
                    VkPipelineColorBlendStateCreateInfo& colorBlendStateCreateInfo);

    static void
    colorBlendState(const std::vector<VkPipelineColorBlendAttachmentState>& colorBlendAttachmentStates,
                    VkPipelineColorBlendStateCreateInfo& colorBlendStateCreateInfo);
};
}

#endif
