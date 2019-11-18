#ifndef UTILS_DEBUG_MESSENGER
#define UTILS_DEBUG_MESSENGER

#include <vulkan/vulkan.h>

namespace vk {
class Instance;

//
// VkDebugUtilsMessengerEXT wrapper.
//
// A VkDebugUtilsMessengerEXT is a messenger object which handles passing 
// along debug messages to a provided debug callback.
//
class DebugMessenger {
public:
    DebugMessenger(const Instance& instance);
    ~DebugMessenger();
    DebugMessenger(DebugMessenger&& other) noexcept;
    DebugMessenger(const DebugMessenger&) = delete;
    const DebugMessenger& operator=(const DebugMessenger&) = delete;

    // VkDebugUtilsMessengerCreateInfoEXT:
    // - messageSeverity is a bitmask specifying which severity of event(s) will cause 
    //   this callback to be called (VK_DEBUG_UTILS_MESSAGE_SEVERITY_):
    //   . VERBOSE_BIT_EXT, WARNING_BIT_EXT, ERROR_BIT_EXT
    // - messageType bitmask specifies which type of event(s) will cause this callback 
    //   to be called (VK_DEBUG_UTILS_MESSAGE_TYPE_):
    //   . GENERAL_BIT_EXT, VALIDATION_BIT_EXT, PERFORMANCE_BIT_EXT
    // - pfnUserCallback function to call.
    // - pUserData to be passed to the callback.
    static VkDebugUtilsMessengerCreateInfoEXT 
    messengerCreateInfo();

private:
    const Instance& mInstance;
    VkDebugUtilsMessengerEXT mMessenger = VK_NULL_HANDLE;
};
}

#endif