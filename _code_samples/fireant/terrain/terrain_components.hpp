#pragma once
#include "platform/opengl/mesh_gl.hpp"
#include "terrain/terrain_data.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

namespace bee
{

struct TerrainMaterial : ModelMaterial
{
    TerrainMaterial() = default;

    bool UseHeightMapTexture = true;
    std::shared_ptr<Texture> HeightMapTexture;
};

struct TerrainMeshRenderer
{
    std::shared_ptr<bee::TerrainMesh> Mesh;
    std::shared_ptr<bee::TerrainMaterial> Material;

    TerrainMeshRenderer() {}

    TerrainMeshRenderer(std::shared_ptr<bee::TerrainMesh> mesh, std::shared_ptr<bee::TerrainMaterial> material)
    {
        Mesh = mesh;
        Material = material;
    }
};
}  // namespace bee
