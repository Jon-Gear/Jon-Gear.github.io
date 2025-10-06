#include "tile_editor/tile_components.hpp"
#include "entt/entt.hpp"

#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/resource_manager.hpp"
#include <rendering/model_resource.hpp>

void bee::Tile::Save(cereal::JSONOutputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("Tile_" + number, *this));
}

void bee::Tile::Load(cereal::JSONInputArchive& archive, const entt::entity entity, const std::string& number)
{
    archive(cereal::make_nvp("Tile_" + number, *this));

    auto& tile = bee::Engine.ECS().Registry.get<bee::Tile>(entity);

    if (tile.m_modelPath.first == FileIO::Directory::None) return;

    auto model = bee::Engine.Resources().Load<ModelResource>(tile.m_modelPath.first, tile.m_modelPath.second);

    model->Instantiate(entity);
}

template <class Archive>
void bee::Tile::serialize(Archive& ar)
{
    ar(cereal::make_nvp("X", X),
       cereal::make_nvp("Y", Y),
       cereal::make_nvp("Directory", m_modelPath.first),
       cereal::make_nvp("FilePath", m_modelPath.second));
}

// Explicit template instantiations for the types you intend to use
template void bee::Tile::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive&);
template void bee::Tile::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive&);

void bee::Grid::Save(cereal::JSONOutputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("Grid_" + number, *this));
}

void bee::Grid::Load(cereal::JSONInputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("Grid_" + number, *this));
}

template <class Archive>
void bee::Grid::serialize(Archive& ar)
{
    ar(cereal::make_nvp("Tile Size", m_tile_size), cereal::make_nvp("Grid Size", m_size));
}

// Explicit template instantiations for the types you intend to use
template void bee::Grid::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive&);
template void bee::Grid::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive&);
