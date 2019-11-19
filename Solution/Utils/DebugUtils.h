#ifndef UTILS_DEBUG_UTILS
#define UTILS_DEBUG_UTILS

#include <cassert>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk {
#ifdef _DEBUG
inline void glfwChecker(const int returnCode) {
    assert(returnCode == GLFW_TRUE);
} 
#else
inline void glfwChecker(const int) {}
#endif

void vkChecker(const VkResult result);
}

#endif