#ifndef UTILS_PIPELINE_STAGE_PIPELINE_STATES 
#define UTILS_PIPELINE_STAGE_PIPELINE_STATES

#include "ColorBlendState.h"
#include "DepthStencilState.h"
#include "DynamicState.h"
#include "InputAssemblyState.h"
#include "MultisampleState.h"
#include "RasterizationState.h"
#include "TessellationState.h"
#include "VertexInputState.h"
#include "ViewportState.h"

namespace vk {
//
// This class is used to initialize all the needed pipeline states.
// If you do not initialize a pipeline state (with its specific setter)
// then you will get nullptr (with its specific getter).
//
// This class is used for GraphicsPipeline's construction
//
class PipelineStates {
public:
    void 
    setColorBlendState(const ColorBlendState& colorBlendState);

    void
    setDepthStencilState(const DepthStencilState& depthStencilState);

    void
    setDynamicState(const DynamicState& dynamicState);

    void
    setInputAssemblyState(const InputAssemblyState& inputAssemblyState);

    void
    setMultisampleState(const MultisampleState& multisampleState);

    void
    setRasterizationState(const RasterizationState& rasterizationState);

    void
    setTessellationState(const TessellationState& tessellationState);

    void
    setVertexInputState(const VertexInputState& vertexInputState);

    void
    setViewportState(const ViewportState& viewportState);

    const ColorBlendState* 
    colorBlendState() const;

    const DepthStencilState*
    depthStencilState() const;

    const DynamicState*
    dynamicState() const;

    const InputAssemblyState*
    inputAssemblyState() const;

    const MultisampleState*
    multisampleState() const;

    const RasterizationState*
    rasterizationState() const;

    const TessellationState*
    tessellationState() const;

    const VertexInputState*
    vertexInputState() const;

    const ViewportState*
    viewportState() const;

private:
    bool mUseColorBlendState = false;
    ColorBlendState mColorBlendState = {};

    bool mUseDepthStencilState = false;
    DepthStencilState mDepthStencilState = {};

    bool mUseDynamicState = false;
    DynamicState mDynamicState = {};

    bool mUseInputAssemblyState = false;
    InputAssemblyState mInputAssemblyState = {};

    bool mUseMultisampleState = false;
    MultisampleState mMultisampleState = {};

    bool mUseRasterizationState = false;
    RasterizationState mRasterizationState = {};

    bool mUseTessellationState = false;
    TessellationState mTessellationState = {};

    bool mUseVertexInputState = false;
    VertexInputState mVertexInputState = {};

    bool mUseViewportState = false;
    ViewportState mViewportState = {};
};
}

#endif
