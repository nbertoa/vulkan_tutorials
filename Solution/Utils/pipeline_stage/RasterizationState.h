#ifndef UTILS_PIPELINE_STAGE_RASTERIZATION_STATE
#define UTILS_PIPELINE_STAGE_RASTERIZATION_STATE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
//
// VkPipelineRasterizationStateCreateInfo wrapper
//
// The rasterizer takes the geometry that is shaped by the vertices from the vertex shader 
// and turns it into fragments to be colored by the fragment shader. It also performs depth testing, 
// face culling and the scissor test, and it can be configured to output fragments that fill entire 
// polygons or just the edges(wireframe rendering).
// All this is configured using the VkPipelineRasterizationStateCreateInfo structure.
//
// You need this class to:
// - Create the GraphicsPipeline
//
class RasterizationState {
public:
    // * depthClampEnable controls whether to clamp the fragment’s depth values. 
    //   If the pipeline is not created with VkPipelineRasterizationDepthClipStateCreateInfoEXT present 
    //   then enabling depth clamp will also disable clipping primitives to the z planes of the frustrum. 
    //   Otherwise depth clipping is controlled by the state set in VkPipelineRasterizationDepthClipStateCreateInfoEXT.
    //
    // * rasterizerDiscardEnable controls whether primitives are discarded immediately before the rasterization stage.
    //
    // * polygonMode is the triangle rendering mode:
    //
    //   - VK_POLYGON_MODE_POINT specifies that polygon vertices are drawn as points.
    //   - VK_POLYGON_MODE_LINE specifies that polygon edges are drawn as line segments.
    //   - VK_POLYGON_MODE_FILL specifies that polygons are rendered using the polygon rasterization rules.
    //   - VK_POLYGON_MODE_FILL_RECTANGLE_NV specifies that polygons are rendered using polygon rasterization rules, 
    //     modified to consider a sample within the primitive if the sample location is inside the axis-aligned bounding box 
    //     of the triangle after projection. Note that the barycentric weights used in attribute interpolation can extend 
    //     outside the range[0, 1] when these primitives are shaded.Special treatment is given to a sample position on the 
    //     boundary edge of the bounding box. In such a case, if two rectangles lie on either side of a common edge 
    //     (with identical endpoints) on which a sample position lies, then exactly one of the triangles must produce a 
    //     fragment that covers that sample during rasterization.
    //     Polygons rendered in VK_POLYGON_MODE_FILL_RECTANGLE_NV mode may be clipped by the frustum or by user clip planes.
    //     If clipping is applied, the triangle is culled rather than clipped.
    //
    // * lineWidth of rasterized line segments.
    //
    // * cullMode is the triangle facing direction used for primitive culling:
    //
    //   - VK_CULL_MODE_NONE specifies that no triangles are discarded
    //   - VK_CULL_MODE_FRONT_BIT specifies that front-facing triangles are discarded
    //   - VK_CULL_MODE_BACK_BIT specifies that back-facing triangles are discarded
    //   - VK_CULL_MODE_FRONT_AND_BACK specifies that all triangles are discarded.
    //
    // * frontFace that specifies the front-facing triangle orientation to be used for culling:
    //
    //   - VK_FRONT_FACE_COUNTER_CLOCKWISE specifies that a triangle with positive area is considered front-facing.
    //   - VK_FRONT_FACE_CLOCKWISE specifies that a triangle with negative area is considered front-facing.
    //
    // * depthBiasEnable controls whether to bias fragment depth values.
    //
    // * depthBiasConstantFactor controlling the constant depth value added to each fragment.
    //
    // * depthBiasClamp is the maximum(or minimum) depth bias of a fragment.
    //
    // * depthBiasSlopeFactor applied to a fragment’s slope in depth bias calculations.
    RasterizationState(const VkBool32 depthClampEnable = VK_FALSE,
                       const VkBool32 rasterizerDiscardEnable = VK_FALSE,
                       const VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
                       const float lineWidth = 1.0f,
                       const VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
                       const VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
                       const VkBool32 depthBiasEnable = VK_FALSE,
                       const float depthBiasConstantFactor = 0.0f,
                       const float depthBiasClamp = 0.0f,
                       const float depthBiasSlopeFactor = 0.0f);

    const VkPipelineRasterizationStateCreateInfo&
    vkState() const;

private:
    VkPipelineRasterizationStateCreateInfo mCreateInfo = {};
};
}

#endif 