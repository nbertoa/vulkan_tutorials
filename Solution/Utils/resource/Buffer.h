#ifndef UTILS_RESOURCE_BUFFER
#define UTILS_RESOURCE_BUFFER

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vulkan {
//
// Buffer wrapper.
//
// Buffers represent linear arrays of data which are used for various 
// purposes by binding them to a graphics or compute pipeline via 
// descriptor sets or via certain commands, or by directly specifying
// them as parameters to certain commands.
//
// Creating a Buffer of certain length does not automatically allocate memory for it. 
// It is a 3-step process that must be manually performed by you. 
// You can also choose to use our Vulkan Memory Allocator library 
// which takes care of the allocation for you
// - 1. Allocate DeviceMemory,
// - 2. Create Buffer,
// - 3. Bind them together using function vkBindBufferMemory.
//
// That is why you must also create a DeviceMemory object. 
// You should not allocate separate DeviceMemory for each Buffer. 
// Instead, you should allocate bigger chunks of memory and assign parts of them to your Buffer. 
// Allocation is a costly operation and there is a limit on maximum number of allocations as well, 
// all of which can be queried from your PhysicalDevice.
//
// You need the Buffer to:
// - Create the BufferView.
//
// To create/use the Buffer you need:
// - DeviceMemory for vkBindBufferMemory
//
class Buffer {
public:
    // This constructor must be used if you want that this buffer also creates
    // its own DeviceMemory.
    //
    // * bufferSize in bytes.
    //
    // * deviceMemoryProperties to create the DeviceMemory
    //
    // * sharingMode of the buffer when it will be accessed by multiple queue families
    //
    // * queueFamilyIndices that will access this buffer
    //   (ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT).
    //
    // Notes:     
    //   - The global logical device owns the buffer and the device memory
    //   - The global physical device is used to create the device memory
    Buffer(const vk::DeviceSize bufferSize,
           const vk::BufferUsageFlags bufferUsage,
           const vk::MemoryPropertyFlags deviceMemoryProperties,
           const vk::SharingMode sharingMode = vk::SharingMode::eExclusive,
           const std::vector<uint32_t>& queueFamilyIndices = {});

    // This constructor must be used if you want to provide
    // the DeviceMemory that the buffer should use.
    // Check the first constructor for an explanation of each parameter.
    // Notes:     
    //   - The global logical device owns the buffer and the device memory
    Buffer(const vk::DeviceSize bufferSize,
           const vk::BufferUsageFlags bufferUsage,
           const vk::DeviceMemory deviceMemory,
           const vk::SharingMode sharingMode = vk::SharingMode::eExclusive,
           const std::vector<uint32_t>& queueFamilyIndices = {});
    ~Buffer();
    Buffer(Buffer&& other) noexcept;
    Buffer(const Buffer&) = delete;
    const Buffer& operator=(const Buffer&) = delete;  
    
    vk::Buffer 
    vkBuffer() const;

    vk::DeviceSize 
    size() const;
    
    // The driver may not immediately copy the data
    // into the buffer memory, for example because
    // of caching.
    // It is also possible that writes to the buffer
    // are not visible in the mapped memory yet.
    //
    // There are two ways to deal with that problem:
    // - Use a memory heap that is host coherent,
    // indicated with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    // - Call vkFlushMappedMemoryRanges to after writing
    // to the mapped memory, and call vkInvalidateMappedMemoryRanges
    // before reading from the mapped memory.
    //
    // The first approach ensures that the mapped memory
    // always matches the contents of the allocated memory.
    // Do keep in mind that this may lead to slightly worse
    // performance than explicit flushing.
    //
    // Flushing memory ranges or using a coherent memory heap
    // means that the driver will be aware of our writes to
    // the buffer, but it does not mean that they are actually
    // visible on the GPU yet. The transfer of data to the GPU
    // is an operation that happens in the background and the
    // specification simply tells us that it is guaranteed
    // to be complete as of the next call to vkQueueSubmit.
    //
    // Preconditions:
    // This method cannot be used if you allocated the buffer
    // from a memory type that is device local like , like 
    // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT or VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    // You should use methods like copyFromBufferToDeviceMemory instead.
    void 
    copyToHostMemory(void* sourceData, 
                     const vk::DeviceSize size,
                     const vk::DeviceSize offset);

    // This method will use as "size" the entire buffer size. 
    void 
    copyToHostMemory(void* sourceData,
                     const vk::DeviceSize offset = 0);

    // These methods assumes the buffer was created with
    // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT.
    //
    // These methods create internal staging buffers to be able to do the copy,
    // and use fences to be signaled once the copy operation finishes.
    //
    //
    // * transferCommandPool that will be used to create 
    //  the CommandBuffer which will do the transfer operation.
    //
    // Notes: The global physical device is used to create the staging buffer
    void 
    copyFromBufferToDeviceMemory(const Buffer& sourceBuffer,
                                 const vk::CommandPool transferCommandPool);
    void
    copyFromDataToDeviceMemory(void* sourceData,
                               const vk::DeviceSize size,
                               const vk::CommandPool transferCommandPool);

    // Creates a staging buffer with flags:
    // - VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    // - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    // - VK_SHARING_MODE_EXCLUSIVE
    // and copies "size" bytes from the sourceData to it
    static Buffer
    createAndFillStagingBuffer(void* sourceData,
                               const vk::DeviceSize size);

private:
    // Read Buffer() constructor to understand the parameters.
    static vk::Buffer 
    createBuffer(const vk::DeviceSize sizeInBytes,
                 const vk::BufferUsageFlags usageFlags,
                 const vk::SharingMode sharingMode,
                 const std::vector<uint32_t>& queueFamilyIndices);
  
    vk::Buffer mBuffer;
    const vk::DeviceSize mSizeInBytes = 0;

    // This is only used if the Buffer is created
    // with the constructor that includes the data needed
    // for DeviceMemory creation.
    // Otherwise, we will use the DeviceMemory provided
    // by the second constructor.
    const bool mHasDeviceMemoryOwnership = true;
    vk::DeviceMemory mDeviceMemory;
};
}

#endif 