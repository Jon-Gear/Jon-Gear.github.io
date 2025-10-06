#pragma once
#include "core/fileio.hpp"
#include "entt/entt.hpp"

#include <cereal/cereal.hpp>
#include "tools/glm_cereal.hpp"
#include <cereal/archives/json.hpp>

namespace bee
{

struct Grid
{
    float m_tile_size;
    glm::ivec2 m_size;

    template <class Archive>
    void serialize(Archive& ar);

    void Save(cereal::JSONOutputArchive&, const entt::entity, const std::string&);
    void Load(cereal::JSONInputArchive&, const entt::entity, const std::string&);
};

struct Tile
{
    int X;
    int Y;
    std::pair<FileIO::Directory, std::string> m_modelPath;

    template <class Archive>
    void serialize(Archive& ar);

    void Save(cereal::JSONOutputArchive&, const entt::entity, const std::string&);
    void Load(cereal::JSONInputArchive&, const entt::entity, const std::string&);
};

}  // namespace bee
