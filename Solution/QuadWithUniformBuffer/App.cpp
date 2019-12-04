#include "App.h"

#include <cassert>
#include <memory>

#include "Utils/DebugUtils.h"
#include "Utils/SwapChain.h"
#include "Utils/Window.h"
#include "Utils/device/LogicalDevice.h"
#include "Utils/device/PhysicalDevice.h"
#include "Utils/pipeline_stage/PipelineStates.h"
#include "Utils/shader/ShaderModule.h"
#include "Utils/shader/ShaderModuleSystem.h"
#include "Utils/shader/ShaderStages.h"
#include "Utils/vertex/PosColorVertex.h"

using namespace vk2;

App::App() {
    mGraphicsCommandPool.reset(new CommandPool(PhysicalDevice::graphicsQueueFamilyIndex()));
    mTransferCommandPool.reset(new CommandPool(PhysicalDevice::transferQueueFamilyIndex(),
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
    vkChecker(vkDeviceWaitIdle(LogicalDevice::device()));
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
    assert(mDescriptorPool.get() == VK_NULL_HANDLE);
    const uint32_t imageViewCount = mSwapChain.imageViewCount();

    vk::DescriptorPoolSize descPoolSizes[2];
    descPoolSizes[0].setDescriptorCount(imageViewCount);
    descPoolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
    descPoolSizes[1].setDescriptorCount(imageViewCount);
    descPoolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler);

    vk::DescriptorPoolCreateInfo descPoolInfo;
    descPoolInfo.setMaxSets(imageViewCount);
    descPoolInfo.setPoolSizeCount(2);
    descPoolInfo.setPPoolSizes(descPoolSizes);

    mDescriptorPool = LogicalDevice::device().createDescriptorPoolUnique(descPoolInfo);

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
    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo.setRange(sizeof(MatrixUBO));
    for (uint32_t i = 0; i < imageViewCount; ++i) {
        bufferInfo.buffer = mUniformBuffers->buffer(i).vkBuffer();
        vk::WriteDescriptorSet writeDescriptorSet;
        writeDescriptorSet.setDstBinding(0);
        writeDescriptorSet.setDescriptorCount(1);
        writeDescriptorSet.setDescriptorType(vk::DescriptorType::eUniformBuffer);
        writeDescriptorSet.setPBufferInfo(&bufferInfo);        
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
    assert(mFrameBuffers.empty() == false);
    for (uint32_t i = 0; i < mCommandBuffers->bufferCount(); ++i) {
        CommandBuffer& commandBuffer = mCommandBuffers->commandBuffer(i);

        commandBuffer.beginRecording(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        commandBuffer.beginPass(*mRenderPass,
                                mFrameBuffers[i].get(),
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

    vk::DescriptorSetLayout descSetLayout(mDescriptorSetLayout->vkDescriptorSetLayout());
    vk::PipelineLayoutCreateInfo createInfo
    {
        vk::PipelineLayoutCreateFlags(),
        1, // layout count
        &descSetLayout,
        0, // push contant range count
        nullptr, // push contant ranges
    };
    
    vk::UniquePipelineLayout pipelineLayout = 
        LogicalDevice::device().createPipelineLayoutUnique(createInfo);

    mGraphicsPipeline.reset(new GraphicsPipeline(pipelineLayout,
                                                 pipelineStates,
                                                 shaderStages,
                                                 mRenderPass.get()));
}

void
App::initPipelineStates(PipelineStates& pipelineStates) const {
    std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
    PosColorVertex::vertexInputBindingDescriptions(vertexInputBindingDescriptions);

    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
    PosColorVertex::vertexInputAttributeDescriptions(vertexInputAttributeDescriptions);

    pipelineStates.setVertexInputState({vertexInputBindingDescriptions,
                                        vertexInputAttributeDescriptions});

    pipelineStates.setInputAssemblyState({vk::PrimitiveTopology::eTriangleList,
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
                                                  vk::ShaderStageFlagBits::eVertex)
    );
    shaderStages.addShaderModule(
        ShaderModuleSystem::getOrLoadShaderModule("../../QuadWithUniformBuffer/resources/shaders/frag.spv",
                                                  vk::ShaderStageFlagBits::eFragment)
    );
}

void
App::initRenderPass() {
    assert(mRenderPass.get() == VK_NULL_HANDLE);

    std::vector<vk::AttachmentDescription> attachmentDescriptions;

    // The format of the color attachment should match the format 
    // of the swap chain images.
    //
    // No multisampling
    //
    // Using ImageLayout::eUndefined for initial layout means
    // that we do not care what previous layout the image was in.
    // We want the image to be ready for presentation using the swap chain 
    // after rendering, which is why we use ImageLayout::ePresentSrcKHR
    // for the final layout.
    attachmentDescriptions.emplace_back(vk::AttachmentDescription
    {
        vk::AttachmentDescriptionFlags(),
        mSwapChain.imageFormat(),
        vk::SampleCountFlagBits::e1, // sample count
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare, // depth/stencil buffer
        vk::AttachmentStoreOp::eDontCare, // depth/stencil buffer
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    });

    const std::vector<vk::AttachmentReference> colorAttachments {
        {0, vk::ImageLayout::eColorAttachmentOptimal}
    };
    std::vector<vk::SubpassDescription> subpassDescriptions;
    subpassDescriptions.emplace_back(vk::SubpassDescription
    {
        vk::SubpassDescriptionFlags(),
        vk::PipelineBindPoint::eGraphics,
        0, // input attachment count
        nullptr, // input attachments
        static_cast<uint32_t>(colorAttachments.size()),
        colorAttachments.data(),
    });

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
    std::vector<vk::SubpassDependency> subpassDependencies;
    subpassDependencies.emplace_back(vk::SubpassDependency
    {
        VK_SUBPASS_EXTERNAL, // source subpass
        0, // dest subpass
        vk::PipelineStageFlagBits::eColorAttachmentOutput, // src stage mask
        vk::PipelineStageFlagBits::eColorAttachmentOutput, // dest stage mask
        vk::AccessFlags(), // src access flags
        vk::AccessFlagBits::eColorAttachmentRead |
        vk::AccessFlagBits::eColorAttachmentWrite, // dest access flags
        vk::DependencyFlags()
    });

    vk::RenderPassCreateInfo createInfo =
    {
        vk::RenderPassCreateFlags(),
        static_cast<uint32_t>(attachmentDescriptions.size()),
        attachmentDescriptions.empty() ? nullptr : attachmentDescriptions.data(),
        static_cast<uint32_t>(subpassDescriptions.size()),
        subpassDescriptions.empty() ? nullptr : subpassDescriptions.data(),
        static_cast<uint32_t>(subpassDependencies.size()),
        subpassDependencies.empty() ? nullptr : subpassDependencies.data()
    };

    mRenderPass = LogicalDevice::device().createRenderPassUnique(createInfo);
}

void
App::submitCommandBufferAndPresent() {
    assert(mCommandBuffers != nullptr);

    const vk::Fence& fence = mFences->nextAvailableFence();
    vk::Device device(LogicalDevice::device());
    vkChecker(device.waitForFences({fence},
                                   VK_TRUE,
                                   std::numeric_limits<uint64_t>::max()));
    device.resetFences({fence});

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
    assert(mFrameBuffers.empty());
    assert(mRenderPass.get() != VK_NULL_HANDLE);
    assert(mSwapChain.imageViews().empty() == false);

    vk::FramebufferCreateInfo createInfo =
    {
        vk::FramebufferCreateFlags(),
        mRenderPass.get(),
        1,
        nullptr, // This will be updated with each iamge view.
        mSwapChain.imageWidth(),
        mSwapChain.imageHeight(),
        1 // number of layers
    };

    const std::vector<vk::UniqueImageView>& imageViews = mSwapChain.imageViews();
    mFrameBuffers.resize(imageViews.size());
    for (size_t i = 0; i < imageViews.size(); ++i) {
        assert(imageViews[i].get() != VK_NULL_HANDLE);
        const vk::ImageView attachments[] = {imageViews[i].get()};
        createInfo.setPAttachments(&imageViews[i].get());

        mFrameBuffers[i] = LogicalDevice::device().createFramebufferUnique(createInfo);
    }
}

void
App::initCommandBuffers() {
    assert(mCommandBuffers == nullptr);
    assert(mFrameBuffers.empty() == false);

    mCommandBuffers.reset(new CommandBuffers(*mGraphicsCommandPool,
                                             mFrameBuffers.size(),
                                             VK_COMMAND_BUFFER_LEVEL_PRIMARY));
}

void
App::initSemaphoresAndFences() {
    assert(mImageAvailableSemaphores == nullptr);
    assert(mRenderFinishedSemaphores == nullptr);
    assert(mFences == nullptr);

    mImageAvailableSemaphores.reset(new Semaphores(mFrameBuffers.size()));
    mRenderFinishedSemaphores.reset(new Semaphores(mFrameBuffers.size()));
    mFences.reset(new Fences(mFrameBuffers.size()));
}