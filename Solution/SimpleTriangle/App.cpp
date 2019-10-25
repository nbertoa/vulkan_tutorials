#include "App.h"

#include <cassert>

#include "Utils/PipelineLayout.h"
#include "Utils/ShaderModule.h"
#include "Utils/pipeline_stage/PipelineStates.h"
#include "Utils/pipeline_stage/ShaderStages.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vk;

App::App(const uint32_t windowWidth,
         const uint32_t windowHeight,
         const char* windowTitle,
         const vk::RenderPassCreator& renderPassCreator)
    : BaseApp(windowWidth,
              windowHeight,
              windowTitle,
              renderPassCreator)
{
    createGraphicsPipeline();
    createBuffers();
    recordCommandBuffers();
}

void 
App::createBuffers() {
    assert(mGpuVertexBuffer == nullptr);

    std::vector<vk::PosColorVertex> vertices
    {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    const size_t verticesSize = sizeof(PosColorVertex) * vertices.size();

    Buffer cpuVertexBuffer(mSystemManager.logicalDevice(),
                           mSystemManager.physicalDevice(),
                           verticesSize,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           VK_SHARING_MODE_EXCLUSIVE);

    cpuVertexBuffer.copyToHostMemory(vertices.data(),
                                     verticesSize,
                                     0);

    mGpuVertexBuffer.reset(new Buffer(mSystemManager.logicalDevice(),
                                      mSystemManager.physicalDevice(),
                                      verticesSize,
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,                                      
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      VK_SHARING_MODE_EXCLUSIVE));

    mGpuVertexBuffer->copyFromBufferToDeviceMemory(cpuVertexBuffer,
                                                   mSystemManager.transferCommandPool());
}

void
App::recordCommandBuffers() {
    assert(mCommandBuffers != nullptr);
    assert(mFrameBuffers != nullptr);
    for (size_t i = 0; i < mCommandBuffers->bufferCount(); ++i) {
        CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(i);

        commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        commandBuffer.beginPass(*mRenderPass,
                                mFrameBuffers->buffer(i),
                                mSystemManager.swapChain().imageExtent());

        commandBuffer.bindPipeline(*mGraphicsPipeline);

        commandBuffer.bindVertexBuffer(*mGpuVertexBuffer);

        commandBuffer.draw(static_cast<uint32_t>(mGpuVertexBuffer->size() / sizeof(PosColorVertex)));

        commandBuffer.endPass();

        commandBuffer.endRecording();
    }
}

void
App::createGraphicsPipeline() {
    mGraphicsPipeline.reset();

    PipelineStates pipelineStates;
    initPipelineStates(pipelineStates);

    ShaderStages shaderStages;
    initShaderStages(shaderStages);

    PipelineLayout pipelineLayout(mSystemManager.logicalDevice());

    mGraphicsPipeline.reset(new GraphicsPipeline(mSystemManager.logicalDevice(),
                                                 *mRenderPass,
                                                 0,
                                                 pipelineLayout,
                                                 pipelineStates,
                                                 shaderStages));
}

void
App::initPipelineStates(PipelineStates& pipelineStates) const {
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    PosColorVertex::vertexInputBindingDescriptions(vertexInputBindingDescriptions);

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
    PosColorVertex::vertexInputAttributeDescriptions(vertexInputAttributeDescriptions);

    pipelineStates.setVertexInputState({vertexInputBindingDescriptions,
                                       vertexInputAttributeDescriptions});

    pipelineStates.setInputAssemblyState({VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                         VK_FALSE});

    pipelineStates.setViewportState({mSystemManager.swapChain().viewport(),
                                    mSystemManager.swapChain().scissorRect()});

    pipelineStates.setRasterizationState({});

    pipelineStates.setMultisampleState({});

    const ColorBlendAttachmentState colorBlendAttachmentState;
    pipelineStates.setColorBlendState({colorBlendAttachmentState});
}

void
App::initShaderStages(ShaderStages& shaderStages) {
    ShaderModuleSystem& shaderModuleSystem = mSystemManager.shaderModuleSystem();
    shaderStages.addShaderModule(shaderModuleSystem.getOrLoadShaderModule("../../SimpleTriangle/resources/shaders/vert.spv",
                                                                          VK_SHADER_STAGE_VERTEX_BIT));
    shaderStages.addShaderModule(shaderModuleSystem.getOrLoadShaderModule("../../SimpleTriangle/resources/shaders/frag.spv",
                                                                          VK_SHADER_STAGE_FRAGMENT_BIT));
}