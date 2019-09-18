#ifndef UTILS_APP_INSTANCE
#define UTILS_APP_INSTANCE

#include <vector>
#include <vulkan/vulkan.h>

namespace vk {
class DebugMessenger;

// VkInstance wrapper.
// There is no global state in Vulkan and all per-application 
// state is stored in a VkInstance object. 
// Creating a VkInstance object initializes the Vulkan library 
// and allows the application to pass information about itself 
// to the implementation.
class AppInstance {
public:
    // Preconditions:
    // - GLFW must be initialized first.
    AppInstance();
    ~AppInstance();
    AppInstance(AppInstance&& other) noexcept;
    AppInstance(const AppInstance&) = delete;
    const AppInstance& operator=(const AppInstance&) = delete;

    const VkInstance& vkInstance() const;

private:
    void createInstance();

    // Validation layers are optional components that hook into Vulkan function calls
    // to apply additional operations. We use them for debugging functionality.
    static std::vector<const char*> getInstanceLayers();
    static bool areInstanceLayersSupported(const std::vector<const char*>& instanceLayers);

    // Vulkan is a platform agnostic API, which means that we need an extension
    // to interface with different systems, like for example, the window system.
    static std::vector<const char*> getRequiredInstanceExtensions();

#ifndef NDEBUG // Debug
    static VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo();
#endif

    VkInstance mInstance = VK_NULL_HANDLE;

#ifndef NDEBUG // Debug
    DebugMessenger* mMessenger = nullptr;
#endif
};
}

#endif