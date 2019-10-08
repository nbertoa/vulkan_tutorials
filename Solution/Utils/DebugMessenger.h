#ifndef UTILS_DEBUG_MESSENGER
#define UTILS_DEBUG_MESSENGER

#include <vulkan/vulkan.h>

namespace vk {
class Instance;

// VkDebugUtilsMessengerEXT wrapper.
// A VkDebugUtilsMessengerEXT is a messenger object which handles passing 
// along debug messages to a provided debug callback.
class DebugMessenger {
public:
    DebugMessenger(const Instance& instance);
    ~DebugMessenger();
    DebugMessenger(DebugMessenger&& other) noexcept;
    DebugMessenger(const DebugMessenger&) = delete;
    const DebugMessenger& operator=(const DebugMessenger&) = delete;

    static VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo();

private:
    const Instance& mInstance;
    VkDebugUtilsMessengerEXT mMessenger = VK_NULL_HANDLE;
};
}

#endif