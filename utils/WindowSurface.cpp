#include "WindowSurface.h"

#include "DebugUtils.h"

namespace vk {
WindowSurface::WindowSurface(const VkInstance instance, GLFWwindow& glfwWindow)
	: mInstance(instance)
{
	assert(mInstance != VK_NULL_HANDLE);
	vkChecker(glfwCreateWindowSurface(mInstance, &glfwWindow, nullptr, &mSurface));
	assert(mSurface != VK_NULL_HANDLE);
}

WindowSurface::~WindowSurface() {
	assert(mInstance != VK_NULL_HANDLE);
	assert(mSurface != VK_NULL_HANDLE);
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
}
}