#ifndef UTILS_WINDOW
#define UTILS_WINDOW

#include <cassert>
#include <GLFW/glfw3.h>

namespace vk {
// GLFWwindow wrapper that provide useful methods to create/destroy/get it.
class Window {
public:
	Window(const int width, const int height, const char* title);
	~Window();

	GLFWwindow& glfwWindow() const { assert(mWindow != nullptr); return *mWindow; }

private:
	GLFWwindow* mWindow = nullptr;
};
}

#endif