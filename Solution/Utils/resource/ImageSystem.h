#ifndef UTILS_RESOURCE_IMAGE_SYSTEM
#define UTILS_RESOURCE_IMAGE_SYSTEM

#include <string>
#include <unordered_map>

namespace vk {
class CommandPool;
class Image;
class LogicalDevice;
class PhysicalDevice;

class ImageSystem {
public:
    ImageSystem(const LogicalDevice& logicalDevice,
                const PhysicalDevice& physicalDevice);
    ~ImageSystem();
    ImageSystem(ImageSystem&&) noexcept = delete;
    ImageSystem(const ImageSystem&) = delete;
    const ImageSystem& operator=(const ImageSystem&) = delete;

    Image&
    getOrLoadImage(const std::string& imageFilePath,
                   const CommandPool& transferCommandPool);

    void
    eraseImage(const std::string& imageFilePath);

    void
    clear();

private:
    const LogicalDevice& mLogicalDevice;
    const PhysicalDevice& mPhysicalDevice;

    using ImageByPath = std::unordered_map<std::string, Image*>;
    ImageByPath mImageByPath;
};
}

#endif