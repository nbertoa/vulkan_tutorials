#include "PipelineStates.h"

namespace vk {
void
PipelineStates::setColorBlendState(const ColorBlendState& colorBlendState) {
    mUseColorBlendState = true;
    mColorBlendState = colorBlendState;
}

void
PipelineStates::setDepthStencilState(const DepthStencilState& depthStencilState) {
    mUseDepthStencilState = true;
    mDepthStencilState = depthStencilState;
}

void
PipelineStates::setDynamicState(const DynamicState& dynamicState) {
    mUseDynamicState = true;
    mDynamicState = dynamicState;
}

void
PipelineStates::setInputAssemblyState(const InputAssemblyState& inputAssemblyState) {
    mUseInputAssemblyState = true;
    mInputAssemblyState = inputAssemblyState;
}

void
PipelineStates::setMultisampleState(const MultisampleState& multisampleState) {
    mUseMultisampleState = true;
    mMultisampleState = multisampleState;
}

void
PipelineStates::setRasterizationState(const RasterizationState& rasterizationState) {
    mUseRasterizationState = true;
    mRasterizationState = rasterizationState;
}

void
PipelineStates::setTessellationState(const TessellationState& tessellationState) {
    mUseTessellationState = true;
    mTessellationState = tessellationState;
}

void
PipelineStates::setVertexInputState(const VertexInputState& vertexInputState) {
    mUseVertexInputState = true;
    mVertexInputState = vertexInputState;
}

void
PipelineStates::setViewportState(const ViewportState& viewportState) {
    mUseViewportState = true;
    mViewportState = viewportState;
}

const ColorBlendState*
PipelineStates::colorBlendState() const {
    if (mUseColorBlendState) {
        return &mColorBlendState;
    } else {
        return nullptr;
    }
}

const DepthStencilState*
PipelineStates::depthStencilState() const {
    if (mUseDepthStencilState) {
        return &mDepthStencilState;
    } else {
        return nullptr;
    }
}

const DynamicState*
PipelineStates::dynamicState() const {
    if (mUseDynamicState) {
        return &mDynamicState;
    } else {
        return nullptr;
    }
}

const InputAssemblyState*
PipelineStates::inputAssemblyState() const {
    if (mUseInputAssemblyState) {
        return &mInputAssemblyState;
    } else {
        return nullptr;
    }
}

const MultisampleState*
PipelineStates::multisampleState() const {
    if (mUseMultisampleState) {
        return &mMultisampleState;
    } else {
        return nullptr;
    }
}

const RasterizationState*
PipelineStates::rasterizationState() const {
    if (mUseRasterizationState) {
        return &mRasterizationState;
    } else {
        return nullptr;
    }
}

const TessellationState*
PipelineStates::tessellationState() const {
    if (mUseTessellationState) {
        return &mTessellationState;
    } else {
        return nullptr;
    }
}

const VertexInputState*
PipelineStates::vertexInputState() const {
    if (mUseVertexInputState) {
        return &mVertexInputState;
    } else {
        return nullptr;
    }
}

const ViewportState*
PipelineStates::viewportState() const {
    if (mUseViewportState) {
        return &mViewportState;
    } else {
        return nullptr;
    }
}
}