#include "SubpassDependency.h"

namespace vk {
SubpassDependency::SubpassDependency(const uint32_t sourceSubpassIndex,
                                     const uint32_t destSubpassIndex,
                                     const VkPipelineStageFlags sourceStageMask,
                                     const VkAccessFlags sourceAccessMask,
                                     const VkPipelineStageFlags destStageMask,
                                     const VkAccessFlags destAccessMask,
                                     const VkDependencyFlags dependencyFlags) {
    mSubpassDependency.srcSubpass = sourceSubpassIndex;
    mSubpassDependency.dstSubpass = destSubpassIndex;
    mSubpassDependency.srcStageMask = sourceStageMask;
    mSubpassDependency.dstStageMask = destStageMask;
    mSubpassDependency.srcAccessMask = sourceAccessMask;
    mSubpassDependency.dstAccessMask = destAccessMask;
    mSubpassDependency.dependencyFlags = dependencyFlags;
}

}