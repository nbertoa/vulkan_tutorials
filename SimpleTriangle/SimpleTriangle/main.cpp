#include "utils/AppInstance.h"
#include "utils/Window.h"

int main() {
	vk::Window window(800, 600, "Vulkan window");
	vk::AppInstance instance(window.glfwWindow());

	while (glfwWindowShouldClose(&window.glfwWindow()) != 0) {
		glfwPollEvents();
	}
}