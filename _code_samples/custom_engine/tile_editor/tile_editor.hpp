#include "core/ecs.hpp"
#include "core/transform.hpp"
#include "core/fileio.hpp"

#include <string>
#include <vector>

namespace bee
{

class TileEditor : public bee::System
{
public:
    TileEditor();
    ~TileEditor();
    void Update(float) override;
    void Render() override;

#ifdef BEE_INSPECTOR
    void Inspect() override;
    void Inspect(Entity) override;
#endif
private:
    void CreateGrid();
    void UpdateGridSize(entt::entity);
    void UpdateGridTileSize(entt::entity);
    void UpdateTile(entt::entity, std::pair<FileIO::Directory, std::string>);

    void InspectGrid(entt::entity);
    void InspectTile(entt::entity);

    entt::entity m_selectedTile = entt::null;
    entt::entity m_selectedGrid = entt::null;

    std::vector<std::pair<FileIO::Directory, std::string>> m_importedModelPaths;
};

}  // namespace bee
