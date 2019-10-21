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
    // - messageSeverity is a bitmask specifying which severity of event(s) will cause this callback to be called:
    //   . VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT specifies the most verbose output indicating 
    //     all diagnostic messages from the Vulkan loader, layers, and drivers should be captured.
    //   . VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT specifies an informational message such as 
    //     resource details that may be handy when debugging an application.
    //   . VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT specifies use of Vulkan that may expose an app bug.
    //     Such cases may not be immediately harmful, such as a fragment shader outputting to a location with no attachment.
    //     Other cases may point to behavior that is almost certainly bad when unintended such as using an image whose memory 
    //     has not been filled. In general if you see a warning but you know that the behavior is intended/desired, then simply ignore the warning.
    //   . VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT specifies that the application has violated a valid usage condition of the specification.
    // - messageType bitmask specifies which type of event(s) will cause this callback to be called.
    //   . VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT specifies that some general event has occurred. 
    //     This is typically a non-specification, non-performance event.
    //   . VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT specifies that something has occurred during validation 
    //     against the Vulkan specification that may indicate invalid behavior.
    //   . VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT specifies a potentially non-optimal use of Vulkan, 
    //     e.g. using vkCmdClearColorImage when setting VkAttachmentDescription::loadOp to 
    //     VK_ATTACHMENT_LOAD_OP_CLEAR would have worked.
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