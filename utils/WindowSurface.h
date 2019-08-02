#ifndef UTILS_WINDOW_SURFACE
#define UTILS_WINDOW_SURFACE

#include <cassert>
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace vk {
// VkSurface wrapper to be able to create/destroy/get the vulkan surface easily.
// Since Vulkan is a platform agnostic API, it cannot interface directly with the window
// system on its own. 
// To establish the connection between Vulkan and the window system to present
// results to the screen, we need to use the WSI (Window System Integration) extensions.
// In this case, we use the VK_KHR_surface extension.
class WindowSurface {
public:
	WindowSurface(const VkInstance& instance, GLFWwindow& glfwWindow);
	~WindowSurface();

	const VkSurfaceKHR& vkSurface() const { assert(mSurface != VK_NULL_HANDLE); return mSurface; }

private:
	VkInstance mInstance = VK_NULL_HANDLE;
	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
};
}

#endif