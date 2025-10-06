#include "terrain/terrain_data.hpp"

#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/fileio.hpp"
#include "tools/log.hpp"

#include "rendering/image.hpp"

#include "terrain/terrain_components.hpp"
#include "terrain/terrain_tools.hpp"

#include <cereal/archives/json.hpp>

#include "tools/asset_manager/engine_ui.hpp"
#include <tools/save_system.hpp>

using namespace bee;

template <class Archive>
void Terrain::serialize(Archive& archive)
{
    archive(cereal::make_nvp("HeightMapPath", HeightMapImagePath),
            cereal::make_nvp("TerrainTexturePath", TerrainTextureImagePath),
            cereal::make_nvp("TerrainSize", TerrainSize));
}

// Explicit template instantiations for the types you intend to use
template void Terrain::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive&);
template void Terrain::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive&);

void bee::Terrain::Save(cereal::JSONOutputArchive& archive, const entt::entity entity)
{
    TerrainFileName = SavingLoadingSystem::GetSceneInfo().GetFilename();

    auto& renderer = Engine.ECS().Registry.get<TerrainMeshRenderer>(entity);
    auto& heightMapImage = renderer.Material->HeightMapTexture->Image;
    auto& terrainTextureImage = renderer.Material->BaseColorTexture->Image;

    std::vector<unsigned char> heightMapTextureData = terrain_tools::GetHeightMapTexture(entity);
    std::vector<unsigned char> terrainTextureData = terrain_tools::GetTerrainTexture(entity);

    std::string heightMapImageLocation{"terrain/textures/"};
    heightMapImageLocation.append(TerrainFileName).append("HM.png");  // HM for height map

    std::string terrainTextureImageLocation{"terrain/textures/"};
    terrainTextureImageLocation.append(TerrainFileName).append("TT.png");  // TT for terrain texture

    Engine.FileIO().WriteImageFilePNG(FileIO::Directory::Assets,
                                      heightMapImageLocation,
                                      heightMapTextureData,
                                      heightMapImage->GetWidth(),
                                      heightMapImage->GetHeight(),
                                      4);

    Engine.FileIO().WriteImageFilePNG(FileIO::Directory::Assets,
                                      terrainTextureImageLocation,
                                      terrainTextureData,
                                      terrainTextureImage->GetWidth(),
                                      terrainTextureImage->GetHeight(),
                                      4);

    HeightMapImagePath = heightMapImageLocation;

    TerrainTextureImagePath = terrainTextureImageLocation;

    archive(cereal::make_nvp("Terrain", *this));
}

void bee::Terrain::Load(cereal::JSONInputArchive& archive, const entt::entity entity)
{
    archive(cereal::make_nvp("Terrain", *this));

    auto& terrain = Engine.ECS().Registry.get<Terrain>(entity);

    auto material = terrain_tools::CreateTerrainMaterial(terrain);
    constexpr auto resolution{20u};
    auto mesh = terrain_tools::CreateTerrainMesh(terrain, resolution);

    Engine.ECS().CreateComponent<TerrainMeshRenderer>(entity, mesh, material);

    terrain_tools::CacheTerrain();
}
