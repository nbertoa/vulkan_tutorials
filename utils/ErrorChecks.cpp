#ifndef UTILS_ERROR_CHECKS
#define UTILS_ERROR_CHECKS

#include <cassert>
#include <GLFW/glfw3.h>

inline void glfwChecker(const int returnCode) {
#ifdef _DEBUG
	assert(returnCode == GLFW_TRUE);
#endif
}

#endif