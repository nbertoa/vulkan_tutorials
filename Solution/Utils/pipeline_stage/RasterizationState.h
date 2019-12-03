#ifndef UTILS_PIPELINE_STAGE_RASTERIZATION_STATE
#define UTILS_PIPELINE_STAGE_RASTERIZATION_STATE

#include <vulkan/vulkan.hpp>

namespace vk2 {
//
// PipelineRasterizationStateCreateInfo wrapper
//
// The rasterizer takes the geometry that is shaped by 
// the vertices from the vertex shader and turns it 
// into fragments to be colored by the fragment shader. 
//
// It also performs depth testing, face culling and the scissor test, 
// and it can be configured to output fragments that fill entire 
// polygons or just the edges(wireframe rendering).
//
// All this is configured using the PipelineRasterizationStateCreateInfo structure.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class RasterizationState {
public:
    // * depthClampEnable controls whether to clamp the fragment’s depth values. 
    //   If the pipeline is not created with PipelineRasterizationDepthClipStateCreateInfoEXT 
    //   present then enabling depth clamp will also disable clipping primitives to the 
    //   z planes of the frustrum. 
    //   Otherwise depth clipping is controlled by the state set in 
    //   PipelineRasterizationDepthClipStateCreateInfoEXT.
    //
    // * rasterizerDiscardEnable controls whether primitives are discarded immediately 
    //   before the rasterization stage.
    //
    // * polygonMode is the triangle rendering mode (VK_POLYGON_MODE_):
    //
    //   - POINT, LINE, FILL, FILL_RECTANGLE_NV
    //
    // * lineWidth of rasterized line segments.
    //
    // * cullMode is the triangle facing direction used for primitive culling (VK_CULL_MODE_):
    //
    //   - NONE, FRONT_BIT, BACK_BIT, FRONT_AND_BACK
    //
    // * frontFace that specifies the front-facing triangle 
    //   orientation to be used for culling (VK_FRONT_FACE_):
    //
    //   - COUNTER_CLOCKWISE, CLOCKWISE
    //
    // * depthBiasEnable controls whether to bias fragment depth values.
    //
    // * depthBiasConstantFactor controlling the constant depth value added to each fragment.
    //
    // * depthBiasClamp is the maximum(or minimum) depth bias of a fragment.
    //
    // * depthBiasSlopeFactor applied to a fragment’s slope in depth bias calculations.
    RasterizationState(const vk::Bool32 depthClampEnable = VK_FALSE,
                       const vk::Bool32 rasterizerDiscardEnable = VK_FALSE,
                       const vk::PolygonMode polygonMode = vk::PolygonMode::eFill ,
                       const float lineWidth = 1.0f,
                       const vk::CullModeFlags cullMode = vk::CullModeFlagBits::eBack,
                       const vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise,
                       const vk::Bool32 depthBiasEnable = VK_FALSE,
                       const float depthBiasConstantFactor = 0.0f,
                       const float depthBiasClamp = 0.0f,
                       const float depthBiasSlopeFactor = 0.0f);

    const vk::PipelineRasterizationStateCreateInfo&
    state() const;

private:
    vk::PipelineRasterizationStateCreateInfo mCreateInfo = {};
};
}

#endif 