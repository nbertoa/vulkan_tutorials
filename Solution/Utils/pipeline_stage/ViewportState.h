#ifndef UTILS_PIPELINE_STAGE_VIEWPORT_STATE
#define UTILS_PIPELINE_STAGE_VIEWPORT_STATE

#include <vulkan/vulkan.h>

namespace vk {
//
// VkPipelineViewportStateCreateInfo wrapper
//
// A viewport basically describes the region of the framebuffer that the output 
// will be rendered to. This will almost always be(0, 0) to (window width, window height)
//
// You need this class to:
// - Create the GraphicsPipeline
//
class ViewportState {
public:
    // * viewport that defines the transformation from the image to the framebuffer:
    //
    //   - x and y are the viewport’s upper left corner(x, y).
    //   - width and height
    //   - minDepth and maxDepth are the depth range for the viewport.
    //     It is valid for minDepth to be greater than or equal to maxDepth.
    //
    // * scissorRectangle that defines in which regions pixels will actually be stored.
    //   Any pixels outside the scissor rectangles will be discarded by the rasterizer:
    //
    //   - offset of the rectangle.
    //   - extent of the rectangle.
    //    
    ViewportState(const VkViewport& viewport = {},
                  const VkRect2D& scissorRectangle = {});
    ViewportState(const ViewportState& state);
    const ViewportState& operator=(const ViewportState& state);
    
    const VkPipelineViewportStateCreateInfo&
    vkState() const;
    
private:
    VkPipelineViewportStateCreateInfo mCreateInfo = {};

    VkViewport mViewport;
    VkRect2D mScissorRectangle;
};
}

#endif 