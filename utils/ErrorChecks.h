#ifndef UTILS_ERROR_CHECKS
#define UTILS_ERROR_CHECKS

#include <cassert>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace vk {
inline void glfwChecker(const int returnCode) {
#ifndef NDEBUG
	assert(returnCode == GLFW_TRUE);
#endif
}

inline void vkChecker(const VkResult result) {
#ifndef NDEBUG
	assert(result == VK_SUCCESS);
#endif
}
}

#endif