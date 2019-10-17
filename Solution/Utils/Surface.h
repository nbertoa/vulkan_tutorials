#ifndef UTILS_SURFACE
#define UTILS_SURFACE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class Instance;
class Window;

//
// VkSurfaceKHR wrapper.
//
// Since Vulkan is a platform agnostic API, it cannot interface directly with the window
// system on its own. 
//
// To establish the connection between Vulkan and the window system to present
// results to the screen, we need to use the WSI (Window System Integration) extensions.
//
// A VkSurfaceKHR object abstracts a native platform surface or window object for use 
// with Vulkan. 
//
// The VK_KHR_surface extension declares the VkSurfaceKHR object, and provides a function 
// for destroying VkSurfaceKHR objects. 
//
// Separate platform-specific extensions each provide a function for creating a 
// VkSurfaceKHR object for the respective platform.
//
// You need the Surface to:
// - Create the SwapChain
// - Get the candidate devices to create the PhysicalDevice
//
class Surface {
public:
    // * window is used to create the VkSurfaceKHR object.
    Surface(const Instance& instance,
            const Window& window);
    ~Surface();
    Surface(Surface&& other) noexcept;
    Surface(const Surface&) = delete;
    const Surface& operator=(const Surface&) = delete;

    VkSurfaceKHR 
    vkSurface() const;

    // This is needed to create the SwapChain
    // VkSurfaceCapabilitiesKHR:
    // - physicalDevice is the physical device that will be associated with the 
    //   swapchain to be created, as described for vkCreateSwapchainKHR.
    // - surface is the surface that will be associated with the swapchain.
    // - pSurfaceCapabilities is a pointer to an instance of the VkSurfaceCapabilitiesKHR 
    //   structure in which the capabilities are returned.
    VkSurfaceCapabilitiesKHR 
    physicalDeviceSurfaceCapabilities(const VkPhysicalDevice physicalDevice) const;

    // This is needed to create the SwapChain
    // VkSurfaceFormatKHR:
    // - format is a VkFormat that is compatible with the specified surface.
    // - colorSpace is a presentation VkColorSpaceKHR that is compatible with the surface.
    std::vector<VkSurfaceFormatKHR> 
    physicalDeviceSurfaceFormats(const VkPhysicalDevice physicalDevice) const;

    // This is needed to create the SwapChain
    // VkPresentModeKHR:
    // - VK_PRESENT_MODE_IMMEDIATE_KHR specifies that the presentation engine does 
    //   not wait for a vertical blanking period to update the current image, meaning 
    //   this mode may result in visible tearing.No internal queuing of presentation 
    //   requests is needed, as the requests are applied immediately.
    // - VK_PRESENT_MODE_MAILBOX_KHR specifies that the presentation engine waits for 
    //   the next vertical blanking period to update the current image.Tearing cannot be observed.
    //   An internal single - entry queue is used to hold pending presentation requests.
    //   If the queue is full when a new presentation request is received, the new request 
    //   replaces the existing entry, and any images associated with the prior entry become 
    //   available for re - use by the application.
    //   One request is removed from the queueand processed during each vertical blanking 
    //   period in which the queue is non - empty.
    // - VK_PRESENT_MODE_FIFO_KHR specifies that the presentation engine waits for the 
    //   next vertical blanking period to update the current image.Tearing cannot be observed.
    //   An internal queue is used to hold pending presentation requests.
    //   New requests are appended to the end of the queue, and one request is removed from the 
    //   beginning of the queueand processed during each vertical blanking period in which the 
    //   queue is non - empty.This is the only value of presentMode that is required to be supported.
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
    // - VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR specifies that the presentation engine and 
    //   application have concurrent access to a single image, which is referred to as a shared 
    //   presentable image. The presentation engine is only required to update the current 
    //   image after a new presentation request is received.
    //   Therefore the application must make a presentation request whenever an update is required.
    //   However, the presentation engine may update the current image at any point, meaning 
    //   this mode may result in visible tearing.
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
    std::vector<VkPresentModeKHR> 
    physicalDeviceSurfacePresentModes(const VkPhysicalDevice physicalDevice) const;

    // This is used to determine whether a queue family of a physical device 
    // supports presentation to a given surface.
    bool 
    isPhysicalDeviceSupported(const VkPhysicalDevice physicalDevice,
                              const uint32_t queueFamilyIndex) const;

private:
    const Instance& mInstance;
    VkSurfaceKHR mSurface = VK_NULL_HANDLE;
};
}

#endif