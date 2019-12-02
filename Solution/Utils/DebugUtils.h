#ifndef UTILS_DEBUG_UTILS
#define UTILS_DEBUG_UTILS

#include <cassert>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace vk2 {
#ifdef _DEBUG
inline void glfwChecker(const int returnCode) {
    assert(returnCode == GLFW_TRUE);
} 
#else
inline void glfwChecker(const int) {}
#endif

void vkChecker(const VkResult result);
void vkChecker(const vk::Result result);
}

#endif