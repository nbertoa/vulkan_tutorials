#ifndef UTILS_RESOURCE_MODEL_SYSTEM
#define UTILS_RESOURCE_MODEL_SYSTEM

#include <string>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

#include "Model.h"
#include "../vertex/PosTexCoordVertex.h"

namespace vulkan {

class ModelSystem {
public:
    ModelSystem() = delete;
    ~ModelSystem() = delete;
    ModelSystem(ModelSystem&&) noexcept = delete;
    ModelSystem(const ModelSystem&) = delete;
    const ModelSystem& operator=(const ModelSystem&) = delete;

    static const Model<PosTexCoordVertex>&
    getOrLoadModelWithPosTexCoordVertex(const std::string& modelFilePath);
    
    static void
    clear();

private:
    using ModelWithPosTexCoordVertexByPath = std::unordered_map<std::string, Model<PosTexCoordVertex>>;
    static ModelWithPosTexCoordVertexByPath mModelWithPosTexCoordVertexByPath;
};

}

#endif