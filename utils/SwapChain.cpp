#include "SwapChain.h"

#include <algorithm>
#include <limits>

#include "DebugUtils.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Semaphore.h"
#include "Surface.h"
#include "Window.h"

namespace vk {
SwapChain::SwapChain(const LogicalDevice& logicalDevice,
                     const Window& window,
                     const Surface& surface)
    : mLogicalDevice(logicalDevice) 
{       
    createSwapChain(window,
                    surface);

    setImagesAndViews();

    setViewportAndScissorRect();
}

SwapChain::~SwapChain() {
    assert(mSwapChain != VK_NULL_HANDLE);

    for (const VkImageView view : mSwapChainImageViews) {
        assert(view != VK_NULL_HANDLE);
        vkDestroyImageView(mLogicalDevice.vkDevice(),
                           view,
                           nullptr);
    }

    vkDestroySwapchainKHR(mLogicalDevice.vkDevice(), mSwapChain, nullptr);
}

SwapChain::SwapChain(SwapChain&& other) noexcept
    : mLogicalDevice(other.mLogicalDevice)
    , mSwapChain(other.mSwapChain)
    , mSwapChainImages(std::move(mSwapChainImages))
    , mSwapChainImageViews(std::move(mSwapChainImageViews))
    , mImageFormat(other.mImageFormat)
    , mExtent(other.mExtent)
    , mViewport(other.mViewport)
    , mScissorRect(other.mScissorRect)
{
    other.mSwapChain = VK_NULL_HANDLE;
}

uint32_t 
SwapChain::acquireNextImage(const Semaphore& semaphore) {
    assert(mSwapChain != VK_NULL_HANDLE);

    // The third parameter specifies a timeout in nanoseconds 
    // for an image to become available. Using the maximum
    // vaule of a 64 bit unsigned integer disables the timeout.
    uint32_t imageIndex;
    vkChecker(vkAcquireNextImageKHR(mLogicalDevice.vkDevice(),
                                    mSwapChain,
                                    std::numeric_limits<uint64_t>::max(),
                                    semaphore.vkSemaphore(),
                                    VK_NULL_HANDLE,
                                    &imageIndex));

    return imageIndex;
}

void
SwapChain::present(const VkQueue queue,
                   const Semaphore& waitSemaphore,
                   const uint32_t imageIndex) {
    assert(mSwapChain != VK_NULL_HANDLE);
    assert(queue != VK_NULL_HANDLE);

    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    
    // Semaphores to wait on before presentation can happen.
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &waitSemaphore.vkSemaphore();

    // Set swap chain to present the image and the
    // index of the image for it.
    info.swapchainCount = 1;
    info.pSwapchains = &mSwapChain;
    info.pImageIndices = &imageIndex;

    // This allows you to specify an array of VkResult values to check
    // for every individual swap chain if presentation was successful.
    // It is not necessary if you are only using a single swap chain,
    // because you can simply use the return value of the present method.
    info.pResults = nullptr;

    vkChecker(vkQueuePresentKHR(queue,
                                &info));

}

VkPipelineViewportStateCreateInfo 
SwapChain::pipelineViewportCreateInfo() const {
    assert(mSwapChain != VK_NULL_HANDLE);

    VkPipelineViewportStateCreateInfo createInfo  = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo.viewportCount = 1;
    createInfo.pViewports = &mViewport;
    createInfo.scissorCount = 1;
    createInfo.pScissors = &mScissorRect;

    return createInfo;
}

VkSurfaceFormatKHR
SwapChain::swapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) {
    assert(surfaceFormats.empty() == false);

    for (const VkSurfaceFormatKHR& surfaceFormat : surfaceFormats) {
        // For the color space we will use SRGB if it is available,
        // because it results in more accurate perceived colors.
        // For the format, we will use the standard RGB.
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return surfaceFormat;
        }
    }

    return surfaceFormats[0];
}

VkPresentModeKHR
SwapChain::swapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
    // VK_PRESENT_MODE_IMMEDIATE_KHR:
    // Images submitted by your application are transferred to the screen
    // right away, which may result in tearing.
    // 
    // VK_PRESENT_MODE_FIFO_KHR:
    // The swap chain is a queue where the display takes an image
    // from the front of the queue when the display is refreshed
    // and the program inserts rendered images at the back of the queue.
    // If the queue is full then the program has to wait.
    // This is most similar to vertical sync as found in modern games.
    // The moment that the display is refreshed is known as "vertical blank".
    //
    // VK_PRESENT_MODE_FIFO_RELAXED_KHR:
    // This mode only differs from the previous one if the application
    // is late and the queue was empty at the last vertical blank.
    // Instead of waiting for the next vertical blank, the image is
    // transferred right away when it finally arrives.
    // This may result in visible tearing.
    //
    // VK_PRESENT_MODE_MAILBOX_KHR:
    // This is another variation of the second mode.
    // Instead of blocking the application when the queue is full,
    // the images that are already queued are simply replaced with the
    // newer ones. This mode can be used to implement triple buffering,
    // which allows you to avoid tearing with significantly less 
    // latency issues than standard vertical sync that uses
    // double buffering.

    // Some drivers currently do not properly support VK_PRESENT_MODE_FIFO_KHR.
    // So we should prefer VK_PRESENT_MODE_IMMEDIATE_KHR if VK_PRESENT_MODE_MAILBOX_KHR
    // is not available.
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const VkPresentModeKHR presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        } else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    return bestMode;
}

VkExtent2D
SwapChain::swapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, 
                           const uint32_t windowWidth, 
                           const uint32_t windowHeight) {
    // Vulkan tells us to match the resolution of the window by setting the width 
    // and height in the currentExtent member.
    // However, some window managers do allow us to differ here and 
    // this is indicated by setting the width and height in currentExtent to a special value. 
    // It is the maximum value of uint32_t.
    // In that case, we will pick the resolution that best matches the window 
    // within the minImageExtent and maxImageExtent bounds.

    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return surfaceCapabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {windowWidth, windowHeight};

        actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, 
                                      std::min(surfaceCapabilities.maxImageExtent.width, 
                                               actualExtent.width));
        actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, 
                                       std::min(surfaceCapabilities.maxImageExtent.height, 
                                                actualExtent.height));

        return actualExtent;
    }
}

uint32_t
SwapChain::swapChainImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) {
    // As we may sometimes have to wait on the driver to complete internal operations
    // before we can acquire another image to render to, then we request at least
    // one more image than the minumum.
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

    // We should also make sure to not exceed the maximum number of images while
    // doing this, where 0 is a special value that means that there is no maximum.
    if (surfaceCapabilities.maxImageCount > 0 && 
        imageCount > surfaceCapabilities.maxImageCount) {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    return imageCount;
}

void
SwapChain::setQueueFamilies(const PhysicalDevice& physicalDevice, 
                            VkSwapchainCreateInfoKHR& swapChainCreateInfo) {
    // VK_SHARING_MODE_EXCLUSIVE:
    // An image is owned by one queue family at a time and
    // the ownership must be explicitly transfered before using it
    // in another queue family. This options offers the best performances.
    //
    // VK_SHARING_MODE_CONCURRENT:
    // Images can be used across multiple queue families
    // without explicit ownership transfers.
    
    // If the graphics queue family and presentation queue family are the same, which will be
    // the case on most hardware, then we should stick to exclusive mode (best performance).
    const uint32_t queueFamilyIndices[] =
    {
        physicalDevice.graphicsSupportQueueFamilyIndex(),
        physicalDevice.presentationSupportQueueFamilyIndex()
    };

    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }
}

void 
SwapChain::setImagesAndViews() {
    assert(mSwapChainImages.empty());
    assert(mSwapChain != VK_NULL_HANDLE);

    // Images
    uint32_t swapChainImageCount;
    vkChecker(vkGetSwapchainImagesKHR(mLogicalDevice.vkDevice(),
                                      mSwapChain,
                                      &swapChainImageCount,
                                      nullptr));
    mSwapChainImages.resize(swapChainImageCount);
    vkChecker(vkGetSwapchainImagesKHR(mLogicalDevice.vkDevice(),
                                      mSwapChain,
                                      &swapChainImageCount,
                                      mSwapChainImages.data()));

    // Image views
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    // How the image data should be interpreted.
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = mImageFormat;
    // Allows you to swizzle the color channels around.
    // For example, you can map all of the channels to the red channel
    // for a monocrhome texture. You can also map constant values
    // of 0 and 1 to a channel.
    // We stick to the default mapping.
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    // Subresource range field describes what the image's purpose
    // is and which part of the image should be accessed.
    // Our images will be used as color targets without any 
    // mipmapping levels or multiple layers.
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    mSwapChainImageViews.resize(swapChainImageCount);
    for (uint32_t i = 0; i < swapChainImageCount; ++i) {
        createInfo.image = mSwapChainImages[i];
        vkChecker(vkCreateImageView(mLogicalDevice.vkDevice(),
                                    &createInfo,
                                    nullptr,
                                    &mSwapChainImageViews[i]));
    }
}

void
SwapChain::setViewportAndScissorRect() {
    assert(mSwapChain != VK_NULL_HANDLE);

    // This will almost always be (0, 0) and width and height.
    mViewport.x = 0.0f;
    mViewport.y = 0.0f;
    mViewport.width = static_cast<float>(mExtent.width);
    mViewport.height = static_cast<float>(mExtent.height);
    // Specify the range of depth values to use for the framebuffer.
    mViewport.minDepth = 0.0f;
    mViewport.maxDepth = 1.0f;

    // We want to drwa to the entire framebuffer, so we 
    // will specify a scissor rectangle that covers it entirely.
    mScissorRect.offset = {0, 0};
    mScissorRect.extent = mExtent;
}

void
SwapChain::createSwapChain(const Window& window,
                           const Surface& surface) {
    const PhysicalDevice& physicalDevice = mLogicalDevice.physicalDevice();

    uint32_t windowWidth;
    uint32_t windowHeight;
    window.widthAndHeight(windowWidth, windowHeight);

    const VkSurfaceFormatKHR surfaceFormat = 
        swapChainSurfaceFormat(surface.formats(physicalDevice.vkPhysicalDevice()));
    const VkSurfaceCapabilitiesKHR surfaceCapabilities = 
        surface.capabilities(physicalDevice.vkPhysicalDevice());
    mExtent = swapChainExtent(surfaceCapabilities, windowWidth, windowHeight);
    mImageFormat = surfaceFormat.format;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    // Which surface the swap chain should be tied to.
    createInfo.surface = surface.vkSurface();
    createInfo.minImageCount = swapChainImageCount(surfaceCapabilities);
    createInfo.imageFormat = mImageFormat;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = mExtent;
    // Specifies the amount of layers each image consists of.
    // More than this is needed only for stereoscopic 3D applications
    createInfo.imageArrayLayers = 1;
    // Specifies waht kind of operations we will
    //use the images in the swap chain for.
    // In our case, we will render to them directly,
    // which means that they are used as color attachment.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // Specify if a certain transform should be applied to images
    // in the swap chain (for example, a 90 degree clockwise rotation
    // or horizontal flip).
    // We do not want any transformation, so we specify the current
    // transformation.
    createInfo.preTransform = surfaceCapabilities.currentTransform;
    // Specify if the alpha channel should be used for
    // blending with other windows in the window system.
    // We want to ignore the alpha channel.
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // Refer to swapChainPresentMode() method
    createInfo.presentMode =
        swapChainPresentMode(surface.presentModes(physicalDevice.vkPhysicalDevice()));
    // Specify if you want to ignore the color of pixels that are 
    // obscured, for example because another window is in 
    // front of them.
    createInfo.clipped = VK_TRUE;
    // With Vulkan it is possible that your swap chain becomes
    // invalid or unoptimized while your application is running,
    // for example because the window was resized. In that case,
    // the swap chain actually needs to be recreated from
    // scratch and a refeence to the old one must be specified 
    // in this field. 
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    setQueueFamilies(physicalDevice, createInfo);

    vkChecker(vkCreateSwapchainKHR(mLogicalDevice.vkDevice(),
                                   &createInfo,
                                   nullptr,
                                   &mSwapChain));
    assert(mSwapChain != VK_NULL_HANDLE);
}
}