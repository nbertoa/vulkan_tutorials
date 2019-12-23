#ifndef UTILS_RESOURCE_MODEL
#define UTILS_RESOURCE_MODEL

#include <vector>

namespace vulkan {
template<typename T>
struct Model {
std::vector<T> mVertices;
std::vector<uint32_t> mIndices;
};
}

#endif
