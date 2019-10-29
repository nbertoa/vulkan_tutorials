#include "SubpassDependencies.h"

#include <cassert>

namespace vk {
void
SubpassDependencies::add(const uint32_t sourceSubpassIndex,
                         const uint32_t destSubpassIndex,
                         const VkPipelineStageFlags sourceStageMask,
                         const VkAccessFlags sourceAccessMask,
                         const VkPipelineStageFlags destStageMask,                         
                         const VkAccessFlags destAccessMask,
                         const VkDependencyFlags dependencyFlags) {
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = sourceSubpassIndex;
    dependency.dstSubpass = destSubpassIndex;
    dependency.srcStageMask = sourceStageMask;
    dependency.dstStageMask = destStageMask;
    dependency.srcAccessMask = sourceAccessMask;
    dependency.dstAccessMask = destAccessMask;
    dependency.dependencyFlags = dependencyFlags;
    mVkSubpassDependencies.emplace_back(dependency);
}

uint32_t
SubpassDependencies::size() const {
    return static_cast<uint32_t>(mVkSubpassDependencies.size());
}

bool
SubpassDependencies::empty() const {
    return mVkSubpassDependencies.empty();
}

const VkSubpassDependency&
SubpassDependencies::operator[](const uint32_t index) const {
    assert(index < size());

    return mVkSubpassDependencies[index];
}

}