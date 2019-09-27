#ifndef UTILS_DEBUG_UTILS
#define UTILS_DEBUG_UTILS

#include <cassert>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace vk {
#ifndef NDEBUG // Debug
inline void glfwChecker(const int returnCode) {
    assert(returnCode == GLFW_TRUE);
} 
#else
inline void glfwChecker(const int) {}
#endif

#ifndef NDEBUG // Debug
inline void vkChecker(const VkResult result) {
    assert(result == VK_SUCCESS);
}
#else
inline void vkChecker(const VkResult ) {}
#endif
}

#endif