#include "ModelSystem.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace vulkan {
ModelSystem::ModelWithPosTexCoordVertexByPath
ModelSystem::mModelWithPosTexCoordVertexByPath = {};

const Model<PosTexCoordVertex>&
ModelSystem::getOrLoadModelWithPosTexCoordVertex(const std::string& modelFilepath) {
    Model<PosTexCoordVertex> model;

    ModelWithPosTexCoordVertexByPath::const_iterator findIt =
        mModelWithPosTexCoordVertexByPath.find(modelFilepath);
    if (findIt != mModelWithPosTexCoordVertexByPath.end()) {
        return findIt->second;
    } else {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warnings;
        std::string errors;

        if (tinyobj::LoadObj(&attrib,
                             &shapes,
                             &materials,
                             &warnings,
                             &errors,
                             modelFilepath.c_str()) == false) {
            throw std::runtime_error(warnings + errors);
        }

        const bool objFile = modelFilepath.substr(modelFilepath.find_last_of(".") + 1) == "obj";

        std::unordered_map<PosTexCoordVertex, uint32_t> uniqueVertices;

        for (const tinyobj::shape_t& shape : shapes) {
            for (const tinyobj::index_t index : shape.mesh.indices) {
                PosTexCoordVertex vertex;

                const size_t basePosIndex = 3 * index.vertex_index;
                vertex.mPosition.x = attrib.vertices[basePosIndex];
                vertex.mPosition.y = attrib.vertices[basePosIndex + 1];
                vertex.mPosition.z = attrib.vertices[basePosIndex + 2];

                const size_t baseTexCoordIndex = 2 * index.texcoord_index;
                vertex.mTexCoord.x = attrib.texcoords[baseTexCoordIndex];

                // The OBJ format assumes a coordinate system where a vertical coordinate of 0 means
                // the bottom of the image, however we’ve uploaded our image into Vulkan in a
                // top to bottom orientation where 0 means the top of the image.Solve this by
                // flipping the vertical component of the texture coordinates
                if (objFile) {
                    vertex.mTexCoord.y = 1.0f - attrib.texcoords[baseTexCoordIndex + 1];
                } else {
                    vertex.mTexCoord.y = attrib.texcoords[baseTexCoordIndex + 1];
                }                

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(model.mVertices.size());
                    model.mVertices.push_back(vertex);
                }
                model.mIndices.push_back(uniqueVertices[vertex]); 
            }
        }

        Model<PosTexCoordVertex>& containerModel = mModelWithPosTexCoordVertexByPath[modelFilepath];
        containerModel = model;

        return containerModel;
    }
}

void
ModelSystem::clear() {
    mModelWithPosTexCoordVertexByPath.clear();
}
}