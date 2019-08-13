#ifndef UTILS_PIPELINE_MULTISAMPLE_STATE_FACTORY
#define UTILS_PIPELINE_MULTISAMPLE_STATE_FACTORY

#include <vulkan/vulkan.h>

namespace vk {
// The multisampling is one of the ways to perform anti-aliasing.
// It works by combining the fragment shader results of multiple polygons
// that rasterize to the same pixel.
// This mainly occurs along edges, which is also where the most noticeable
// multiple times of only one polygon maps to a pixel, it is significantly
// less expensive than simply rendering to a higher resolution and then 
// downscaling.
class PipelineMultisampleStateFactory {
public:
    PipelineMultisampleStateFactory() = delete;
    ~PipelineMultisampleStateFactory() = delete;
    PipelineMultisampleStateFactory(const PipelineMultisampleStateFactory&) = delete;
    const PipelineMultisampleStateFactory& operator=(const PipelineMultisampleStateFactory&) = delete;
    PipelineMultisampleStateFactory(PipelineMultisampleStateFactory&&) = delete;
    PipelineMultisampleStateFactory& operator=(PipelineMultisampleStateFactory&&) = delete;

    static VkPipelineMultisampleStateCreateInfo disabledMultisampling();
};
}

#endif