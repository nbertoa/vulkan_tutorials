#include "Window.h"

#include <cassert>

#include "DebugUtils.h"

namespace vk {
Window::Window(const int width, const int height, const char* title) {
	assert(width > 0);
	assert(height > 0);
	assert(title != nullptr);

	glfwChecker(glfwInit());
	
	// Avoid OpenGL context creation
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Avoid window resizing
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	mWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
	assert(mWindow != nullptr && "Window creation failed");
}

Window::~Window() {
	assert(mWindow != nullptr);
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}
}