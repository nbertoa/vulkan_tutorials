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

    // VkVertexInputBindingDescription: 
    // - binding is the binding number that this structure describes.
    // - stride is the distance in bytes between two consecutive elements within the buffer.
    // - inputRate is a VkVertexInputRate value specifying whether vertex attribute addressing is 
    //   a function of the vertex index or of the instance index.
    //
    // VkVertexInputAttributeDescription:
    // - location is the shader binding location number for this attribute.
    // - binding is the binding number which this attribute takes its data from.
    // - format is the sizeand type of the vertex attribute data.
    // - offset is a byte offset of this attribute relative to the start of 
    //   an element in the vertex input binding.
    static void 
    vertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
                     const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions,
                     VkPipelineVertexInputStateCreateInfo& createInfo);

    // VkPipelineRasterizationStateCreateInfo:
    // - sType is the type of this structure.
    // - pNext is NULL or a pointer to an extension - specific structure.
    // - flags is reserved for future use.
    // - depthClampEnable controls whether to clamp the fragment’s depth values.
    //   If the pipeline is not created with VkPipelineRasterizationDepthClipStateCreateInfoEXT present 
    //   then enabling depth clamp will also disable clipping primitives to the z planes of the frustrum.
    // - rasterizerDiscardEnable controls whether primitives are discarded immediately before the rasterization stage.
    // - polygonMode is the triangle rendering mode.See VkPolygonMode.
    // - cullMode is the triangle facing direction used for primitive culling.See VkCullModeFlagBits.
    // - frontFace is a VkFrontFace value specifying the front - facing triangle orientation to be used for culling.
    // - depthBiasEnable controls whether to bias fragment depth values.
    // - depthBiasConstantFactor is a scalar factor controlling the constant depth value added to each fragment.
    // - depthBiasClamp is the maximum(or minimum) depth bias of a fragment.
    // - depthBiasSlopeFactor is a scalar factor applied to a fragment’s slope in depth bias calculations.
    // - lineWidth is the width of rasterized line segments.
      
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

    // VkPipelineMultisampleStateCreateInfo:
    // - sType is the type of this structure.
    // - pNext is NULL or a pointer to an extension - specific structure.
    // - flags is reserved for future use.
    // - rasterizationSamples is a VkSampleCountFlagBits specifying the number of 
    //   samples used in rasterization.
    // - sampleShadingEnable can be used to enable Sample Shading.
    // - minSampleShading specifies a minimum fraction of sample shading if 
    //   sampleShadingEnable is set to VK_TRUE.
    // - pSampleMask is a bitmask of static coverage information that is ANDed with 
    //   the coverage information generated during rasterization.
    // - alphaToCoverageEnable controls whether a temporary coverage value is generated 
    //   based on the alpha component of the fragment’s first color output..
    // - alphaToOneEnable controls whether the alpha component of the fragment’s first 
    //   color output is replaced with one.

    // The multisampling is one of the ways to perform anti-aliasing.
    // It works by combining the fragment shader results of multiple polygons
    // that rasterize to the same pixel.
    // This mainly occurs along edges, which is also where the most noticeable
    // multiple times of only one polygon maps to a pixel, it is significantly
    // less expensive than simply rendering to a higher resolution and then 
    // downscaling:
    static void
    disableMultisampleState(VkPipelineMultisampleStateCreateInfo& createInfo);

    // VkPipelineInputAssemblyStateCreateInfo:
    // - sType is the type of this structure.
    // - pNext is NULL or a pointer to an extension - specific structure.
    // - flags is reserved for future use.
    // - topology is a VkPrimitiveTopology defining the primitive topology.
    // - primitiveRestartEnable controls whether a special vertex index value is 
    //   treated as restarting the assembly of primitives.
    // - This enable only applies to indexed draws(vkCmdDrawIndexedand vkCmdDrawIndexedIndirect), 
    //   and the special index value is either 0xFFFFFFFF when the indexType parameter of 
    //   vkCmdBindIndexBuffer is equal to VK_INDEX_TYPE_UINT32, 0xFF when indexType is equal t
    //   o VK_INDEX_TYPE_UINT8_EXT, or 0xFFFF when indexType is equal to VK_INDEX_TYPE_UINT16.
    //   Primitive restart is not allowed for “list” topologies.

    // The input assembly describes what kind of geometry will be drawn from
    // the vertices and if primitive restart should be enabled:
    static void 
    createInputAssemblyState(const VkPrimitiveTopology primitiveTopology,
                             const VkBool32 primitiveRestartEnable,
                             VkPipelineInputAssemblyStateCreateInfo& createInfo);

    // Depth and stencil tests:
    // VkPipelineDepthStencilStateCreateInfo:
    // - sType is the type of this structure.
    // - pNext is NULL or a pointer to an extension - specific structure.
    // - flags is reserved for future use.
    // - depthTestEnable controls whether depth testing is enabled.
    // - depthWriteEnable controls whether depth writes are enabled when depthTestEnable is VK_TRUE.
    //   Depth writes are always disabled when depthTestEnable is VK_FALSE.
    // - depthCompareOp is the comparison operator used in the depth test.
    // - depthBoundsTestEnable controls whether depth bounds testing is enabled.
    // - stencilTestEnable controls whether stencil testing is enabled.
    // - frontand back control the parameters of the stencil test.
    // - minDepthBoundsand maxDepthBounds define the range of values used in the depth bounds test.
        
    // Enable depth test
    // Disable stencil test
    static void 
    defaultDepthStencilState(VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo);

    // Color blending:
    // VkPipelineColorBlendAttachmentState:
    // - blendEnable controls whether blending is enabled for the corresponding color attachment.
    //   If blending is not enabled, the source fragment’s color for that attachment is passed through unmodified.
    // - srcColorBlendFactor selects which blend factor is used to determine the source factors(Sr, Sg, Sb).
    // - dstColorBlendFactor selects which blend factor is used to determine the destination factors(Dr, Dg, Db).
    // - colorBlendOp selects which blend operation is used to calculate the RGB values to write to the color attachment.
    // - srcAlphaBlendFactor selects which blend factor is used to determine the source factor Sa.
    // - dstAlphaBlendFactor selects which blend factor is used to determine the destination factor Da.
    // - alphaBlendOp selects which blend operation is use to calculate the alpha values to write to the color attachment.
    // - colorWriteMask is a bitmask of VkColorComponentFlagBits specifying which of the R, G, B, and /or A components 
    //   are enabled for writing.

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
