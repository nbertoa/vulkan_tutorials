#include "App.h"

#include <cassert>

#include "Utils/DebugUtils.h"
#include "Utils/descriptor/WriteDescriptorSet.h"
#include "Utils/device/LogicalDevice.h"
#include "Utils/pipeline/PipelineLayout.h"
#include "Utils/pipeline_stage/PipelineStates.h"
#include "Utils/render_pass/RenderPass.h"
#include "Utils/shader/ShaderModule.h"
#include "Utils/shader/ShaderStages.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vk;

App::App(const uint32_t windowWidth,
         const uint32_t windowHeight,
         const char* windowTitle)
    : mSystemManager(windowWidth,
                     windowHeight,
                     windowTitle)
    , mDescriptorPool(mSystemManager.logicalDevice(),
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                      mSystemManager.swapChain().imageViewCount(),
                      mSystemManager.swapChain().imageViewCount())
{
    initUniformBuffers();
    initVertexBuffer();
    initIndexBuffer();
    initImages();
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
    while (mSystemManager.window().shouldCloseWindow() == false) {
        glfwPollEvents();

        Semaphore& imageAvailableSemaphore = mImageAvailableSemaphores->nextAvailableSemaphore();
        mSystemManager.swapChain().acquireNextImage(imageAvailableSemaphore);

        processCurrentFrame();

        submitCommandBufferAndPresent();
    }

    // We need to wait on the hsot for the completion of outstanding
    // operations for a given queue.
    // vkDeviceWaitIdle is equivalent to submitting fences to all
    // the queues owned yb the device a and waiting with an infinite 
    // timeout for these fences to signal.
    vkChecker(vkDeviceWaitIdle(mSystemManager.logicalDevice().vkDevice()));
}

void
App::processCurrentFrame() {
    // Update uniform buffers
    const uint32_t currentSwapChainImageIndex = mSystemManager.swapChain().currentImageIndex();    
    mMatrixUBO.update(currentSwapChainImageIndex,
                      mSystemManager.swapChain().imageAspectRatio());
    Buffer& uniformBuffer = mUniformBuffers->buffer(currentSwapChainImageIndex);
    uniformBuffer.copyToHostMemory(&mMatrixUBO);
}

void
App::initDescriptorSets() {
    assert(mMatrixUBODescriptorSetLayout == nullptr);

    DescriptorSetLayoutBinding descriptorSetLayoutBinding(0,
                                                          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                          1,
                                                          VK_SHADER_STAGE_VERTEX_BIT);
    mMatrixUBODescriptorSetLayout.reset(new DescriptorSetLayout(mSystemManager.logicalDevice(),
                                                                {descriptorSetLayoutBinding}));

    // Create a descriptor set for each swap chain image, all with the same layout.
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts(
        mSystemManager.swapChain().imageViewCount(),
        mMatrixUBODescriptorSetLayout->vkDescriptorSetLayout()
    );
    mMatrixUBODescriptorSets.reset(new DescriptorSets(mSystemManager.logicalDevice(),
                                                      mDescriptorPool,
                                                      descriptorSetLayouts));

    // The descriptor sets have been allocated now, but the descriptors within still
    // need to be configured.
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    bufferInfos.emplace_back(VkDescriptorBufferInfo{VK_NULL_HANDLE,
                                                    0,
                                                    sizeof(MatrixUBO)});
    for (uint32_t i = 0; i < mSystemManager.swapChain().imageViewCount(); ++i) {   
        bufferInfos.back().buffer = mUniformBuffers->buffer(i).vkBuffer();
        WriteDescriptorSet writeDescriptorSet(bufferInfos,
                                              VK_NULL_HANDLE,
                                              VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                              0);
        mMatrixUBODescriptorSets->updateDescriptorSet(i,
                                                      writeDescriptorSet);
    }
}

void
App::initImages() {
    const std::string path = "../../../external/resources/textures/flowers/dahlia.jpg";
    Image& image = mSystemManager.imageSystem().getOrLoadImage(path,
                                                               mSystemManager.transferCommandPool());
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

    mGpuVertexBuffer.reset(new Buffer(mSystemManager.logicalDevice(),
                                      mSystemManager.physicalDevice(),
                                      verticesSize,
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      VK_SHARING_MODE_EXCLUSIVE));

    mGpuVertexBuffer->copyFromDataToDeviceMemory(screenSpaceVertices.data(),
                                                 verticesSize,
                                                 mSystemManager.physicalDevice(),
                                                 mSystemManager.transferCommandPool());
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

    mGpuIndexBuffer.reset(new Buffer(mSystemManager.logicalDevice(),
                                     mSystemManager.physicalDevice(),
                                     indicesSize,
                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                     VK_SHARING_MODE_EXCLUSIVE));

    mGpuIndexBuffer->copyFromDataToDeviceMemory(indices.data(),
                                                indicesSize,
                                                mSystemManager.physicalDevice(),
                                                mSystemManager.transferCommandPool());
}

void
App::initUniformBuffers() {
    assert(mUniformBuffers == nullptr);

    mUniformBuffers.reset(new Buffers(mSystemManager.logicalDevice(),
                                      mSystemManager.physicalDevice(),
                                      mSystemManager.swapChain().imageViewCount(),
                                      sizeof(MatrixUBO),
                                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      VK_SHARING_MODE_EXCLUSIVE));
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
                                mSystemManager.swapChain().imageExtent());

        commandBuffer.bindPipeline(*mGraphicsPipeline);

        commandBuffer.bindVertexBuffer(*mGpuVertexBuffer);

        commandBuffer.bindIndexBuffer(*mGpuIndexBuffer,
                                      VK_INDEX_TYPE_UINT32);

        commandBuffer.bindDescriptorSet(mGraphicsPipeline->pipelineLayout(),
                                        (*mMatrixUBODescriptorSets)[i]);

        commandBuffer.drawIndexed(static_cast<uint32_t>(mGpuIndexBuffer->size() / sizeof(uint32_t)));

        commandBuffer.endPass();

        commandBuffer.endRecording();
    }
}

void
App::initGraphicsPipeline() {
    assert(mGraphicsPipeline == nullptr);
    assert(mMatrixUBODescriptorSetLayout != nullptr);

    PipelineStates pipelineStates;
    initPipelineStates(pipelineStates);

    ShaderStages shaderStages;
    initShaderStages(shaderStages);

    PipelineLayout pipelineLayout(mSystemManager.logicalDevice(),
                                  mMatrixUBODescriptorSetLayout.get());

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

    pipelineStates.setRasterizationState(RasterizationState());

    pipelineStates.setMultisampleState({});

    const ColorBlendAttachmentState colorBlendAttachmentState;
    pipelineStates.setColorBlendState({colorBlendAttachmentState});
}

void
App::initShaderStages(ShaderStages& shaderStages) {
    ShaderModuleSystem& shaderModuleSystem = mSystemManager.shaderModuleSystem();
    shaderStages.addShaderModule(
        shaderModuleSystem.getOrLoadShaderModule("../../QuadWithTexture/resources/shaders/vert.spv",
                                                 VK_SHADER_STAGE_VERTEX_BIT)
    );
    shaderStages.addShaderModule(
        shaderModuleSystem.getOrLoadShaderModule("../../QuadWithTexture/resources/shaders/frag.spv",
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
    attachmentDescriptions.emplace_back(mSystemManager.swapChain().imageFormat(),
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

    mRenderPass.reset(new RenderPass(mSystemManager.logicalDevice(),
                                     attachmentDescriptions,
                                     subpassDescriptions,
                                     subpassDependencies));
}

void
App::submitCommandBufferAndPresent() {
    assert(mCommandBuffers != nullptr);

    const Fence& fence = mFences->nextAvailableFence();
    fence.waitAndReset();

    // This semaphore was already obtained in run()
    Semaphore& imageAvailableSemaphore = mImageAvailableSemaphores->currentSemaphore();

    Semaphore& renderFinishedSemaphore = mRenderFinishedSemaphores->nextAvailableSemaphore();

    // The next image was already obtained in run()
    const uint32_t swapChainImageIndex = mSystemManager.swapChain().currentImageIndex();
    assert(swapChainImageIndex < mCommandBuffers->bufferCount());

    CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(swapChainImageIndex);
    commandBuffer.submit(mSystemManager.logicalDevice().graphicsQueue(),
                         &imageAvailableSemaphore,
                         &renderFinishedSemaphore,
                         fence,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    mSystemManager.swapChain().present(renderFinishedSemaphore,
                                       swapChainImageIndex);
}

void
App::initFrameBuffers() {
    assert(mFrameBuffers == nullptr);
    assert(mRenderPass != nullptr);

    mFrameBuffers.reset(new FrameBuffers(mSystemManager.logicalDevice(),
                                         *mRenderPass,
                                         mSystemManager.swapChain().imageViews(),
                                         mSystemManager.swapChain().imageWidth(),
                                         mSystemManager.swapChain().imageHeight()));
}

void
App::initCommandBuffers() {
    assert(mCommandBuffers == nullptr);
    assert(mFrameBuffers != nullptr);

    mCommandBuffers.reset(new CommandBuffers(mSystemManager.logicalDevice(),
                                             mSystemManager.graphicsCommandPool(),
                                             mFrameBuffers->bufferCount(),
                                             VK_COMMAND_BUFFER_LEVEL_PRIMARY));
}

void
App::initSemaphoresAndFences() {
    assert(mImageAvailableSemaphores == nullptr);
    assert(mRenderFinishedSemaphores == nullptr);
    assert(mFences == nullptr);

    mImageAvailableSemaphores.reset(new Semaphores(mSystemManager.logicalDevice(),
                                                   mFrameBuffers->bufferCount()));
    mRenderFinishedSemaphores.reset(new Semaphores(mSystemManager.logicalDevice(),
                                                   mFrameBuffers->bufferCount()));
    mFences.reset(new Fences(mSystemManager.logicalDevice(),
                             mFrameBuffers->bufferCount()));
}