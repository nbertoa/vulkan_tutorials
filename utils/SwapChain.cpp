#include "SwapChain.h"

#include <algorithm>
#include <limits>

#include "DebugUtils.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Window.h"
#include "WindowSurface.h"

namespace vk {
SwapChain::SwapChain(const LogicalDevice& logicalDevice,
                     const Window& window,
                     const WindowSurface& windowSurface)
    : mLogicalDevice(logicalDevice) {
       
    createSwapChain(window,
                    windowSurface);

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
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return surfaceFormat;
        }
    }

    return surfaceFormats[0];
}

VkPresentModeKHR
SwapChain::swapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
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
    // How the image data should be interpreted?
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = mImageFormat;
    // Default components
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

    mViewport.x = 0.0f;
    mViewport.y = 0.0f;
    mViewport.width = static_cast<float>(mExtent.width);
    mViewport.height = static_cast<float>(mExtent.height);
    mViewport.minDepth = 0.0f;
    mViewport.maxDepth = 1.0f;

    mScissorRect.offset = {0, 0};
    mScissorRect.extent = mExtent;
}

void
SwapChain::createSwapChain(const Window& window,
                           const WindowSurface& windowSurface) {
    const PhysicalDevice& physicalDevice = mLogicalDevice.physicalDevice();

    uint32_t windowWidth;
    uint32_t windowHeight;
    window.widthAndHeight(windowWidth, windowHeight);

    const VkSurfaceFormatKHR surfaceFormat =
        swapChainSurfaceFormat(windowSurface.surfaceFormats(physicalDevice.vkPhysicalDevice()));
    const VkSurfaceCapabilitiesKHR surfaceCapabilities =
        windowSurface.surfaceCapabilities(physicalDevice.vkPhysicalDevice());
    mExtent = swapChainExtent(surfaceCapabilities, windowWidth, windowHeight);
    mImageFormat = surfaceFormat.format;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = windowSurface.vkSurface();
    createInfo.minImageCount = swapChainImageCount(surfaceCapabilities);
    createInfo.imageFormat = mImageFormat;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = mExtent;
    // More than this is needed for stereoscopic 3D applications
    createInfo.imageArrayLayers = 1;
    // Render to them directly (color attachment)
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode =
        swapChainPresentMode(windowSurface.presentModes(physicalDevice.vkPhysicalDevice()));
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    setQueueFamilies(physicalDevice, createInfo);

    vkChecker(vkCreateSwapchainKHR(mLogicalDevice.vkDevice(),
                                   &createInfo,
                                   nullptr,
                                   &mSwapChain));
    assert(mSwapChain != VK_NULL_HANDLE);
}
}