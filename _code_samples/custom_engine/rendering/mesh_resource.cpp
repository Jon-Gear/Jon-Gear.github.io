#include "rendering/mesh_resource.hpp"
#include "rendering/model_resource.hpp"

#include "tools/gltf_tools.hpp"

bee::MeshResource::MeshResource(const ModelResource& model, int index)
{
    const auto& document = model.GetDocument();
    auto mesh = document.meshes[index];

    m_path = GetPath(model, index);

    assert(!mesh.primitives.empty());
    auto primitive = mesh.primitives[0];

    // Process Vertex data
    auto [positions, vertex_count] = gltf_tools::ReadAttributeGLTF<float>(document, primitive, "POSITION");
    auto [normals, normal_count] = gltf_tools::ReadAttributeGLTF<float>(document, primitive, "NORMAL");
    auto [texture_coordinates, uv_count] = gltf_tools::ReadAttributeGLTF<float>(document, primitive, "TEXCOORD_0");
    auto [colors, color_count] = gltf_tools::ReadAttributeGLTF<float>(document, primitive, "COLOR_0");

    // Process Indices
    const auto& index_accessor = document.accessors[primitive.indices];
    auto index_count = (int)index_accessor.count;

    // Indices (they need to be type handled because they can be either unsigned int or unsigned short)
    std::vector<unsigned int> indices = gltf_tools::ConvertIndicesToUnsignedInt(index_accessor, document);

    m_mesh = xsr::create_mesh(indices.data(),
                              index_count,
                              positions,
                              normals,
                              texture_coordinates,
                              colors,
                              static_cast<unsigned int>(vertex_count));
}

std::string bee::MeshResource::GetPath(const ModelResource& model, int index)
{
    const auto& mesh = model.GetDocument().meshes[index];
    return model.GetPath() + " | Mesh-" + std::to_string(index) + ": " + mesh.name;
}
