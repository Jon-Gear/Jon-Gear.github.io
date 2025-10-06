#pragma once

#include <tinygltf/tiny_gltf.h>

#include <memory>
#include <string>
#include <vector>

#include "core/ecs.hpp"
#include "core/resource.hpp"
#include "rendering/render_xsr_components.hpp"

namespace bee
{

class MeshResource;
class TextureResource;

class ModelResource : public Resource
{
public:
    ModelResource(FileIO::Directory directory, const std::string& filename);
    const tinygltf::Model& GetDocument() const { return m_model; }

    void Instantiate(Entity parent);

    MeshRendererXSR CreateMeshRendererFromNode(const std::string& nodeName) const;

private:
    void InstantiateNode(uint32_t nodeIdx, Entity parent) const;

    int GetTextureIndex(const int&) const;

protected:
    tinygltf::Model m_model;
    std::vector<std::shared_ptr<MeshResource>> m_meshes;
    std::vector<std::shared_ptr<TextureResource>> m_textures;
};
}  // namespace bee
