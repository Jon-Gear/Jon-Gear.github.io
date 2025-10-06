#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

namespace bee
{

struct Terrain
{
    Terrain() = default;

    Terrain(int terrainSize, std::string heightMapImagePath, std::string terrainTextureImagePath)
        : TerrainSize{terrainSize}, HeightMapImagePath{heightMapImagePath}, TerrainTextureImagePath{terrainTextureImagePath}
    {
    }

    template <class Archive>
    void serialize(Archive& archive);

    void Save(cereal::JSONOutputArchive&, const entt::entity);

    void Load(cereal::JSONInputArchive&, const entt::entity);

    std::string TerrainFileName{"Default"};

    int TerrainSize{0};

    // int HeightMapWidth{0};
    // int HeightMapLength{0};
    std::string HeightMapImagePath{};

    // int TerrainTextureWidth{0};
    // int TerrainTextureLength{0};
    std::string TerrainTextureImagePath{};
};

}  // namespace bee
