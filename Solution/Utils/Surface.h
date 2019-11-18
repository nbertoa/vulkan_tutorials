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
    // - physicalDevice that will be associated with the swapchain to be created, 
    //   as described for vkCreateSwapchainKHR.
    // - surface that will be associated with the swapchain.
    // - pSurfaceCapabilities array in which the capabilities are returned.
    VkSurfaceCapabilitiesKHR 
    physicalDeviceSurfaceCapabilities(const VkPhysicalDevice physicalDevice) const;

    // This is needed to create the SwapChain
    // VkSurfaceFormatKHR:
    // - format that is compatible with the specified surface.
    // - colorSpace that is compatible with the surface.
    std::vector<VkSurfaceFormatKHR> 
    physicalDeviceSurfaceFormats(const VkPhysicalDevice physicalDevice) const;

    // This is needed to create the SwapChain
    // VkPresentModeKHR:
    // - IMMEDIATE_KHR, MAILBOX_KHR, FIFO_KHR, FIFO_RELAXED_KHR, SHARED_DEMAND_REFRESH_KHR, 
    //   SHARED_CONTINUOUS_REFRESH_KHR
    //
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