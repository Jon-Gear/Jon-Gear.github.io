#include "rendering/model_resource.hpp"

#include "rendering/mesh_resource.hpp"
#include "rendering/texture_resource.hpp"

#include "core/resource_manager.hpp"
#include "core/transform.hpp"

#include "tools/tools.hpp"
#include "tools/log.hpp"
#include "tools/xsr_render_tools.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace bee;
using namespace std;

bee::ModelResource::ModelResource(FileIO::Directory directory, const std::string& filename)
{
    m_path = filename;

    const string fullFilename = Engine.FileIO().GetPath(directory, filename);

    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool res = false;

    // Check which format to load
    if (StringEndsWith(filename, ".gltf"))
    {
        res = loader.LoadASCIIFromFile(&m_model, &err, &warn, fullFilename);
    }
    else if (StringEndsWith(filename, ".glb"))
    {
        res = loader.LoadBinaryFromFile(&m_model, &err, &warn, fullFilename);
    }

    if (!warn.empty()) Log::Warn(warn);

    if (!err.empty()) Log::Error(err);

    if (!res)
        Log::Error("Failed to load glTF: {}", filename);
    else
        Log::Info("Loaded glTF: {}", filename);

    // Load meshes
    for (int i = 0; i < static_cast<int>(m_model.meshes.size()); i++)
    {
        auto mesh = Engine.Resources().Load<MeshResource>(*this, i);
        m_meshes.push_back(mesh);
    }

    // Load textures
    for (int i = 0; i < static_cast<int>(m_model.images.size()); i++)
    {
        auto image = Engine.Resources().Load<TextureResource>(*this, i);
        m_textures.push_back(image);
    }
}

void ModelResource::InstantiateNode(uint32_t nodeIdx, Entity parent) const
{
    const auto& node = m_model.nodes[nodeIdx];
    const auto entity = Engine.ECS().CreateEntity();

    // Transform
    auto& transform = Engine.ECS().CreateComponent<Transform>(entity);
    transform.Name = node.name;
    if (parent != entt::null) transform.SetParent(parent);

    if (!node.matrix.empty())
    {
        glm::mat4 transformGLM = glm::make_mat4(node.matrix.data());
        transform.SetFromMatrix(transformGLM);
    }
    else
    {
        if (!node.scale.empty()) transform.SetScale(to_vec3(node.scale));
        if (!node.rotation.empty()) transform.SetRotation(to_quat(node.rotation));
        if (!node.translation.empty()) transform.SetTranslation(to_vec3(node.translation));
    }

    // Mesh
    if (node.mesh != -1)
    {
        const auto& mesh = m_model.meshes[node.mesh];
        assert(!mesh.primitives.empty());

        const auto& osmMesh = m_meshes[node.mesh];
        const auto& textureIndex = GetTextureIndex(node.mesh);

        if (textureIndex != -1)
        {
            const auto& osmTexture = m_textures[textureIndex];
            Engine.ECS().CreateComponent<MeshRendererXSR>(entity, osmMesh, osmTexture);
        }
        else
        {
            const auto& osmTexture =
                Engine.Resources().Load<TextureResource>(FileIO::Directory::SharedAssets, "textures/white.png");
            Engine.ECS().CreateComponent<MeshRendererXSR>(entity, osmMesh, osmTexture);
        }
    }

    // Load children
    for (auto childNode : node.children) InstantiateNode(childNode, entity);
}

int bee::ModelResource::GetTextureIndex(const int& meshIndex) const
{
    const auto gltfMaterialIndex = m_model.meshes[meshIndex].primitives[0].material;
    if (gltfMaterialIndex == -1)
    {
        return -1;
    }

    const auto baseColorTextureIndex = m_model.materials[gltfMaterialIndex].pbrMetallicRoughness.baseColorTexture.index;
    if (baseColorTextureIndex == -1)
    {
        return -1;
    }

    const auto imageSourceIndex = m_model.textures[baseColorTextureIndex].source;
    if (imageSourceIndex == -1)
    {
        return -1;
    }

    return imageSourceIndex;
}

void bee::ModelResource::Instantiate(Entity parent)
{
    for (const uint32_t node : m_model.scenes[0].nodes) InstantiateNode(node, parent);
}

MeshRendererXSR bee::ModelResource::CreateMeshRendererFromNode(const std::string& nodeName) const
{
    MeshRendererXSR result;

    const tinygltf::Node* node = nullptr;

    for (const auto& n : m_model.nodes)
        if (n.name == nodeName) node = &n;

    if (node && node->mesh != -1)
    {
        result.Mesh = m_meshes[node->mesh];

        const auto& textureIndex = GetTextureIndex(node->mesh);

        if (textureIndex != -1)
        {
            result.Texture = m_textures[textureIndex];
        }
        else
        {
            result.Texture = Engine.Resources().Load<TextureResource>(FileIO::Directory::SharedAssets, "textures/white.png");
        }
    }

    return result;
}
