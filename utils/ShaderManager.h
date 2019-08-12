#ifndef UTILS_SHADER_MANAGER
#define UTILS_SHADER_MANAGER

#include <mutex>
#include <tbb\concurrent_unordered_set.h>
#include <vector>

namespace vk {
class ShaderManager {
public:
    ShaderManager() = delete;
    ~ShaderManager() = delete;
    ShaderManager(const ShaderManager&) = delete;
    const ShaderManager& operator=(const ShaderManager&) = delete;
    ShaderManager(ShaderManager&&) = delete;
    ShaderManager& operator=(ShaderManager&&) = delete;

    static const std::vector<char>& readFile(const std::string& filename);

private:
    static tbb::concurrent_unordered_set<std::vector<char>*> mShaders;
    static std::mutex mMutex;
};
}

#endif