#include "App.h"

#include <cassert>
#include <memory>

#include "Utils/DebugUtils.h"
#include "Utils/SwapChain.h"
#include "Utils/Window.h"
#include "Utils/descriptor/WriteDescriptorSet.h"
#include "Utils/device/LogicalDevice.h"
#include "Utils/device/PhysicalDevice.h"
#include "Utils/pipeline/PipelineLayout.h"
#include "Utils/pipeline_stage/PipelineStates.h"
#include "Utils/render_pass/RenderPass.h"
#include "Utils/shader/ShaderModule.h"
#include "Utils/shader/ShaderModuleSystem.h"
#include "Utils/shader/ShaderStages.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vk2;

App::App() {
    mGraphicsCommandPool.reset(new CommandPool(PhysicalDevice::graphicsSupportQueueFamilyIndex()));
    mTransferCommandPool.reset(new CommandPool(PhysicalDevice::transferSupportQueueFamilyIndex(),
                                               VK_COMMAND_POOL_CREATE_TRANSIENT_BIT));

    initBuffers();
    initDescriptorSets();
    initRenderPass();
    initFrameBuffers();
    initCommandBuffers();
    initSemaphoresAndFences();
    initGraphicsPipeline();    
    recordCommandBuffers();
}

void
App::run() {
    while (Window::shouldCloseWindow() == false) {
        glfwPollEvents();

        vk::Semaphore& imageAvailableSemaphore = mImageAvailableSemaphores->nextAvailableSemaphore();
        mSwapChain.acquireNextImage(imageAvailableSemaphore);

        processCurrentFrame();

        submitCommandBufferAndPresent();
    }

    // We need to wait on the hsot for the completion of outstanding
    // operations for a given queue.
    // vkDeviceWaitIdle is equivalent to submitting fences to all
    // the queues owned yb the device a and waiting with an infinite 
    // timeout for these fences to signal.
    vkChecker(vkDeviceWaitIdle(LogicalDevice::vkDevice()));
}

void
App::processCurrentFrame() {
    // Update uniform buffers
    const uint32_t currentSwapChainImageIndex = mSwapChain.currentImageIndex();
    mMatrixUBO.update(currentSwapChainImageIndex, 
                      mSwapChain.imageAspectRatio());
    Buffer& uniformBuffer = mUniformBuffers->buffer(currentSwapChainImageIndex);
    uniformBuffer.copyToHostMemory(&mMatrixUBO);
}

void
App::initDescriptorSets() {
    assert(mDescriptorPool == nullptr);
    const uint32_t imageViewCount = mSwapChain.imageViewCount();
    mDescriptorPool.reset(new DescriptorPool
    (
        {
            VkDescriptorPoolSize {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageViewCount},
            VkDescriptorPoolSize {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageViewCount}
        },
        imageViewCount
    ));

    assert(mDescriptorSetLayout == nullptr);
    mDescriptorSetLayout.reset(new DescriptorSetLayout(
    {
            DescriptorSetLayoutBinding(0,
                                       VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                       1,
                                       VK_SHADER_STAGE_VERTEX_BIT)
        }
    ));

    // Create a descriptor set for each swap chain image, all with the same layout.
    mDescriptorSets.reset(new DescriptorSets(*mDescriptorPool,
                                             {imageViewCount,
                                              mDescriptorSetLayout->vkDescriptorSetLayout()}));

    // The descriptor sets have been allocated now, but the descriptors within still
    // need to be configured.
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    bufferInfos.emplace_back(VkDescriptorBufferInfo{VK_NULL_HANDLE,
                                                    0,
                                                    sizeof(MatrixUBO)});
    for (uint32_t i = 0; i < imageViewCount; ++i) {
        bufferInfos.back().buffer = mUniformBuffers->buffer(i).vkBuffer();
        WriteDescriptorSet writeDescriptorSet(bufferInfos,
                                              VK_NULL_HANDLE,
                                              VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                              0);
        mDescriptorSets->updateDescriptorSet(i,
                                             writeDescriptorSet);
    }
}

void 
App::initBuffers() {
    initUniformBuffers();
    initVertexBuffer();
    initIndexBuffer();    
}

void 
App::initVertexBuffer() {
    assert(mGpuVertexBuffer == nullptr);

    std::vector<PosColorVertex> screenSpaceVertices
    {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
    };

    const size_t verticesSize = sizeof(PosColorVertex) * screenSpaceVertices.size();

    mGpuVertexBuffer.reset(new Buffer(verticesSize,
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    mGpuVertexBuffer->copyFromDataToDeviceMemory(screenSpaceVertices.data(),
                                                 verticesSize,
                                                 *mTransferCommandPool);
}

void 
App::initIndexBuffer() {
    assert(mGpuIndexBuffer == nullptr);

    std::vector<uint32_t> indices
    {
        0, 1, 2, // upper-right triangle
        2, 3, 0, // bottom-left triangle
    };

    const size_t indicesSize = sizeof(uint32_t) * indices.size();

    mGpuIndexBuffer.reset(new Buffer(indicesSize,
                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    mGpuIndexBuffer->copyFromDataToDeviceMemory(indices.data(),
                                                indicesSize,
                                                *mTransferCommandPool);
}

void
App::initUniformBuffers() {
    assert(mUniformBuffers == nullptr);

    mUniformBuffers.reset(new Buffers(mSwapChain.imageViewCount(),
                                      sizeof(MatrixUBO),
                                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
}

void
App::recordCommandBuffers() {
    assert(mCommandBuffers != nullptr);
    assert(mFrameBuffers != nullptr);
    for (uint32_t i = 0; i < mCommandBuffers->bufferCount(); ++i) {
        CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(i);

        commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        commandBuffer.beginPass(*mRenderPass,
                                mFrameBuffers->buffer(i),
                                mSwapChain.imageExtent());

        commandBuffer.bindPipeline(*mGraphicsPipeline);

        commandBuffer.bindVertexBuffer(*mGpuVertexBuffer);

        commandBuffer.bindIndexBuffer(*mGpuIndexBuffer,
                                      VK_INDEX_TYPE_UINT32);

        commandBuffer.bindDescriptorSet(mGraphicsPipeline->pipelineLayout(),
                                        (*mDescriptorSets)[i]);

        commandBuffer.drawIndexed(static_cast<uint32_t>(mGpuIndexBuffer->size() / sizeof(uint32_t)));

        commandBuffer.endPass();

        commandBuffer.endRecording();
    }
}

void
App::initGraphicsPipeline() {
    assert(mGraphicsPipeline == nullptr);
    assert(mDescriptorSetLayout != nullptr);

    PipelineStates pipelineStates;
    initPipelineStates(pipelineStates);

    ShaderStages shaderStages;
    initShaderStages(shaderStages);

    PipelineLayout pipelineLayout(mDescriptorSetLayout.get());

    mGraphicsPipeline.reset(new GraphicsPipeline(pipelineLayout,
                                                 pipelineStates,
                                                 shaderStages,
                                                 *mRenderPass));
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

    pipelineStates.setViewportState({mSwapChain.viewport(),
                                     mSwapChain.scissorRect()});

    pipelineStates.setRasterizationState(RasterizationState());

    pipelineStates.setMultisampleState({});

    const ColorBlendAttachmentState colorBlendAttachmentState;
    pipelineStates.setColorBlendState({colorBlendAttachmentState});
}

void
App::initShaderStages(ShaderStages& shaderStages) {
    shaderStages.addShaderModule(
        ShaderModuleSystem::getOrLoadShaderModule("../../QuadWithUniformBuffer/resources/shaders/vert.spv",
                                                  VK_SHADER_STAGE_VERTEX_BIT)
    );
    shaderStages.addShaderModule(
        ShaderModuleSystem::getOrLoadShaderModule("../../QuadWithUniformBuffer/resources/shaders/frag.spv",
                                                  VK_SHADER_STAGE_FRAGMENT_BIT)
    );
}

void
App::initRenderPass() {
    assert(mRenderPass == nullptr);

    std::vector<AttachmentDescription> attachmentDescriptions;

    // The format of the color attachment should match the format 
    // of the swap chain images.
    //
    // No multisampling
    //
    // Using VK_IMAGE_LAYOUT_UNDEFINED for initial layout means
    // that we do not care what previous layout the image was in.
    // We want the image to be ready for presentation using the swap chain 
    // after rendering, which is why we use VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    // for the final layout.
    attachmentDescriptions.emplace_back(mSwapChain.imageFormat(),
                                        VK_ATTACHMENT_LOAD_OP_CLEAR,
                                        VK_ATTACHMENT_STORE_OP_STORE,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    std::vector<VkAttachmentReference> colorAttachmentReferences {
        {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
    };
    std::vector<SubpassDescription> subpassDescriptions;
    subpassDescriptions.emplace_back(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                     colorAttachmentReferences);

    // VK_SUBPASS_EXTERNAL: Implicit subpass before or after the render pass 
    // depending on whether it is specified in srcSubpass or dstSubpass.
    //
    // Specify the operations to wait on and the stages in which these operations occur.
    // We need to wait for the swap chain to finish reading from the image
    // before we can access it.
    // This can be accomplished by waiting on the color attachment output stage itself.
    //
    // The operations that should wait on this are in the color attachment stage
    // adn involve the reading and writing of the color attachment.
    // These settings will prevent the transition from happening until it is
    // actually necessary (and allowed): when we want to start writing colors
    // to it.
    std::vector<SubpassDependency> subpassDependencies;
    subpassDependencies.emplace_back(VK_SUBPASS_EXTERNAL,
                                     0,
                                     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                     0,
                                     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                     VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                     VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    mRenderPass.reset(new RenderPass(attachmentDescriptions,
                                     subpassDescriptions,
                                     subpassDependencies));
}

void
App::submitCommandBufferAndPresent() {
    assert(mCommandBuffers != nullptr);

    const Fence& fence = mFences->nextAvailableFence();
    fence.waitAndReset();

    // This semaphore was already obtained in run()
    vk::Semaphore& imageAvailableSemaphore = mImageAvailableSemaphores->currentSemaphore();
    vk::Semaphore& renderFinishedSemaphore = mRenderFinishedSemaphores->nextAvailableSemaphore();

    // The next image was already obtained in run()
    const uint32_t swapChainImageIndex = mSwapChain.currentImageIndex();
    assert(swapChainImageIndex < mCommandBuffers->bufferCount());

    CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(swapChainImageIndex);
    commandBuffer.submit(LogicalDevice::graphicsQueue(),
                         &imageAvailableSemaphore,
                         &renderFinishedSemaphore,
                         fence,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    mSwapChain.present(renderFinishedSemaphore,
                       swapChainImageIndex);
}

void
App::initFrameBuffers() {
    assert(mFrameBuffers == nullptr);
    assert(mRenderPass != nullptr);

    mFrameBuffers.reset(new FrameBuffers(*mRenderPass,
                                         mSwapChain.imageViews(),
                                         mSwapChain.imageWidth(),
                                         mSwapChain.imageHeight()));
}

void
App::initCommandBuffers() {
    assert(mCommandBuffers == nullptr);
    assert(mFrameBuffers != nullptr);

    mCommandBuffers.reset(new CommandBuffers(*mGraphicsCommandPool,
                                             mFrameBuffers->bufferCount(),
                                             VK_COMMAND_BUFFER_LEVEL_PRIMARY));
}

void
App::initSemaphoresAndFences() {
    assert(mImageAvailableSemaphores == nullptr);
    assert(mRenderFinishedSemaphores == nullptr);
    assert(mFences == nullptr);

    mImageAvailableSemaphores.reset(new Semaphores(mFrameBuffers->bufferCount()));
    mRenderFinishedSemaphores.reset(new Semaphores(mFrameBuffers->bufferCount()));
    mFences.reset(new Fences(mFrameBuffers->bufferCount()));
}