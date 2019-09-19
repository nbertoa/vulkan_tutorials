#ifndef UTILS_SWAP_CHAIN
#define UTILS_SWAP_CHAIN

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class LogicalDevice;
class PhysicalDevice;
class Semaphore;
class Surface;
class Window;

// VkSwapChain wrapper.
// The swapchain provides the ability to present rendering results
// to a surface. A swapchain is an abstraction for an array of
// presentable images that are associated with a surface.
// The swapchain images are represented by VkImage objects.
class SwapChain {
public:
    SwapChain(const LogicalDevice& logicalDevice,
              const Window& window, 
              const Surface& surface);
    ~SwapChain();
    SwapChain(SwapChain&& other) noexcept;
    SwapChain(const SwapChain&) = delete;
    const SwapChain& operator=(const SwapChain&) = delete;

    // Returns the swap chain's acquired image index 
    uint32_t acquireNextImage(const Semaphore& semaphore);

    void present(const Semaphore& waitSemaphore,
                 const uint32_t imageIndex);

    // The viewport describes the region of the framebuffer that the output
    // will be rendered too.
    // It defines the transformation from the image to the framebuffer.
    const VkViewport& viewport() const;

    // While viewports define the transformation from the image to the framebuffer,
    // scissor rectangles define in which regions pixels will actually be stored.
    // Any pixels outside the scissor rectangles will be discarded by the rasterizer.
    const VkRect2D& scissorRect() const;

    VkPipelineViewportStateCreateInfo pipelineViewportCreateInfo() const;

    VkFormat imageFormat() const;

    const std::vector<VkImageView>& imageViews() const;

    uint32_t imageWidth() const;
    uint32_t imageHeight() const;

    // The swap extent is the resolution of the swap chain images
    // and it is almost always exactly equal to the resolution of 
    // the window we are drawing to.
    const VkExtent2D& imageExtent() const;

private:
    static VkSurfaceFormatKHR swapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);

    // The swap chain presentation mode is the most important setting for the swap chain because 
    // it represents the actual conditions for showing images to the screen.
    //
    // VkPresentModeKHR:
    // - VK_PRESENT_MODE_IMMEDIATE_KHR specifies that the presentation engine does 
    //   not wait for a vertical blanking period to update the current image, meaning 
    //   this mode may result in visible tearing.No internal queuing of presentation 
    //   requests is needed, as the requests are applied immediately.
    //
    // - VK_PRESENT_MODE_MAILBOX_KHR specifies that the presentation engine waits for 
    //   the next vertical blanking period to update the current image.Tearing cannot be observed.
    //   An internal single - entry queue is used to hold pending presentation requests.
    //   If the queue is full when a new presentation request is received, the new request 
    //   replaces the existing entry, and any images associated with the prior entry become 
    //   available for re - use by the application.
    //   One request is removed from the queueand processed during each vertical blanking 
    //   period in which the queue is non - empty.
    //
    // - VK_PRESENT_MODE_FIFO_KHR specifies that the presentation engine waits for the 
    //   next vertical blanking period to update the current image.Tearing cannot be observed.
    //   An internal queue is used to hold pending presentation requests.
    //   New requests are appended to the end of the queue, and one request is removed from the 
    //   beginning of the queueand processed during each vertical blanking period in which the 
    //   queue is non - empty.This is the only value of presentMode that is required to be supported.
    //
    // - VK_PRESENT_MODE_FIFO_RELAXED_KHR specifies that the presentation engine generally 
    //   waits for the next vertical blanking period to update the current image.
    //   If a vertical blanking period has already passed since the last update of the current 
    //   image then the presentation engine does not wait for another vertical blanking period 
    //   for the update, meaning this mode may result in visible tearing in this case.
    //   This mode is useful for reducing visual stutter with an application that will mostly 
    //   present a new image before the next vertical blanking period, but may occasionally be 
    //   late, and present a new image just after the next vertical blanking period.
    //   An internal queue is used to hold pending presentation requests.
    //   New requests are appended to the end of the queue, and one request is removed from the 
    //   beginning of the queueand processed during or after each vertical blanking period in which 
    //   the queue is non - empty.
    //
    // - VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR specifies that the presentation engine and 
    //   application have concurrent access to a single image, which is referred to as a shared 
    //   presentable image. The presentation engine is only required to update the current 
    //   image after a new presentation request is received.
    //   Therefore the application must make a presentation request whenever an update is required.
    //   However, the presentation engine may update the current image at any point, meaning 
    //   this mode may result in visible tearing.
    //
    // - VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR specifies that the presentation engine
    //   and application have concurrent access to a single image, which is referred to as a 
    //   shared presentable image. The presentation engine periodically updates the current image 
    //   on its regular refresh cycle.
    //   The application is only required to make one initial presentation request, 
    //   after which the presentation engine must update the current image without any need for 
    //   further presentation requests.
    //   The application can indicate the image contents have been updated by making a presentation 
    //   request, but this does not guarantee the timing of when it will be updated.
    //   This mode may result in visible tearing if rendering to the image is not timed correctly.
    static VkPresentModeKHR swapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes);

    // The swap extent is the resolution of the swap chain images and it is almost always exactly equal to
    // the resolution of the window that we are drawing to.
    //
    // VkSurfaceCapabilitiesKHR:
    // - physicalDevice is the physical device that will be associated with the 
    //   swapchain to be created, as described for vkCreateSwapchainKHR.
    // - surface is the surface that will be associated with the swapchain.
    // - pSurfaceCapabilities is a pointer to an instance of the VkSurfaceCapabilitiesKHR 
    //   structure in which the capabilities are returned.
    static VkExtent2D swapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, 
                                      const uint32_t windowWidth, 
                                      const uint32_t windowHeight);

    // VkSurfaceCapabilitiesKHR:
    // - physicalDevice is the physical device that will be associated with the 
    //   swapchain to be created, as described for vkCreateSwapchainKHR.
    // - surface is the surface that will be associated with the swapchain.
    // - pSurfaceCapabilities is a pointer to an instance of the VkSurfaceCapabilitiesKHR 
    //   structure in which the capabilities are returned.
    static uint32_t swapChainImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

    // VkSwapchainCreateInfoKHR:
    // - device is the device to create the swapchain for.
    // - pCreateInfo is a pointer to a VkSwapchainCreateInfoKHR structure specifying 
    //   the parameters of the created swapchain.
    // - pAllocator is the allocator used for host memory allocated for the swapchain 
    //   object when there is no more specific allocator available(see Memory Allocation).
    // - pSwapchain is a pointer to a VkSwapchainKHR handle in which the created 
    //   swapchain object will be returned.
    static void setQueueFamilies(const PhysicalDevice& physicalDevice, 
                                 VkSwapchainCreateInfoKHR& swapChainCreateInfo);
    
    void createSwapChain(const Window& window,
                         const Surface& surface);

    void setImagesAndViews();
    void setViewportAndScissorRect();

    const LogicalDevice& mLogicalDevice;
    
    VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;

    // Image and views. To use a image in the render pipeline
    // we need to create its corresponding image view.
    // An image view is quite literally a view into an image. 
    // It describes how to access the image and which part of the image to access.
    std::vector<VkImage> mSwapChainImages;
    std::vector<VkImageView> mSwapChainImageViews;

    VkFormat mImageFormat = {};
    VkExtent2D mExtent = {};

    VkViewport mViewport = {};
    VkRect2D mScissorRect = {};
};
}

#endif