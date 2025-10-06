#include "tile_editor/tile_editor.hpp"

#include <imgui/imgui.h>
#include "xsr/include/xsr.hpp"
#include "tools/xsr_debug_render_tools.hpp"

#include "core/transform.hpp"
#include "core/engine.hpp"
#include "core/input.hpp"
#include "core/resource_manager.hpp"

#include <tile_editor/tile_components.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum/magic_enum.hpp>

#include "tools/camera_tools.hpp"
#include "tools/inspector.hpp"
#include <rendering/model_resource.hpp>

/*

A functional tile-based level editor
As a user you can:
- Select a grid size (e.g. 10x12 tiles)
- 2D grid is rendered (using debug drawing)
- Import a GLTF as a tile and list the imported tiles (tileset)
- Select a tile on the grid (from the viewport) and assign one of the imported tiles/meshes to it
- Serialize the level/grid to file (careful with references to the gltfs/tiles)
*/

bee::TileEditor::TileEditor() { Title = "Tile Editor"; }

bee::TileEditor::~TileEditor() {}

void bee::TileEditor::Update(float)
{
    if (!Engine.ECS().Registry.valid(m_selectedGrid) || m_selectedGrid == entt::null)
    {
        return;
    }

    if (ImGuizmo::IsOver() || ImGuizmo::IsUsingAny())
    {
        return;
    }

    if (Engine.Input().GetMouseButtonOnce(Input::MouseButton::Left) && Engine.Input().IsMouseInViewport())
    {
        auto result = camera_tools::GetTileOnGrid(m_selectedGrid);

        m_selectedTile = result.second;
        if (result.first)
        {
            Engine.Inspector().SetSelectedEntity(m_selectedTile);
        }
    }
}

void bee::TileEditor::Render()
{
    auto& ecs = Engine.ECS();

    for (auto [gridEntity, gridTransform, grid] : ecs.Registry.view<Transform, Grid>().each())
    {
        for (auto tileEntity : gridTransform)
        {
            auto& transform = ecs.Registry.get<Transform>(tileEntity);

            debug_draw_tools::draw_square(transform.GetTranslation() + gridTransform.GetTranslation(),
                                          grid.m_tile_size,
                                          debug_draw_tools::Colors::White);

            if (m_selectedTile == tileEntity)
            {
                debug_draw_tools::draw_cross(transform.GetTranslation() + gridTransform.GetTranslation(),
                                             grid.m_tile_size,
                                             debug_draw_tools::Colors::Red);
            }
        }
    }
}

#ifdef BEE_INSPECTOR
void bee::TileEditor::Inspect()
{
    ImGui::Begin(Title.c_str());
    ImGui::Text("this is a tile editor");

    if (ImGui::Button("Create a Grid"))
    {
        CreateGrid();
    }

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Tiles"))
    {
        ImGui::Indent();

        if (ImGui::Button("Import a GLTF File"))
        {
            auto result = Engine.FileIO().OpenDialogue();
            m_importedModelPaths.push_back(result);
        }

        ImGui::Spacing();

        ImGui::Text("Imported Tiles");
        ImGui::Indent();

        for (const auto& pair : m_importedModelPaths)
        {
            std::string path = std::string(magic_enum::enum_name(pair.first)) + " " + pair.second;
            ImGui::Text(path.c_str());
        }
        ImGui::Unindent();
        ImGui::Unindent();
    }
    ImGui::End();
}

void bee::TileEditor::Inspect(Entity entity)
{
    if (Engine.ECS().Registry.any_of<Grid>(entity))
    {
        m_selectedGrid = entity;
        InspectGrid(entity);
    }

    if (Engine.ECS().Registry.any_of<Tile>(entity))
    {
        InspectTile(entity);
    }
}
#endif

void bee::TileEditor::CreateGrid()
{
    auto& ecs = Engine.ECS();

    auto gridEntity = ecs.CreateEntity();

    auto& gridParent = ecs.CreateComponent<Grid>(gridEntity);
    {
        gridParent.m_size = glm::ivec2(3, 3);
        gridParent.m_tile_size = 1.0f;
    }

    auto& gridTransform = ecs.CreateComponent<Transform>(gridEntity);
    {
        gridTransform.Name = "Grid " + std::to_string(gridParent.m_size.x) + " " + std::to_string(gridParent.m_size.y);
    }

    // Create new tiles
    for (int x = 0; x < gridParent.m_size.x; x++)
    {
        for (int y = 0; y < gridParent.m_size.y; y++)
        {
            auto entity = ecs.CreateEntity();
            auto& tileTransform = ecs.CreateComponent<Transform>(entity);
            {
                tileTransform.SetTranslation(glm::vec3(x * gridParent.m_tile_size, 0.0f, y * gridParent.m_tile_size) +
                                             glm::vec3(gridParent.m_tile_size / 2.0f, 0.0f, gridParent.m_tile_size / 2.0f));
                tileTransform.Name = "Tile " + std::to_string(x) + " " + std::to_string(y);
                tileTransform.SetParent(gridEntity);
            }
            auto& tile = ecs.CreateComponent<Tile>(entity);
            {
                tile.X = x;
                tile.Y = y;
            }
        }
    }
}

void bee::TileEditor::UpdateGridSize(entt::entity gridEntity)
{
    auto& ecs = Engine.ECS();

    auto& gridParent = ecs.Registry.get<Grid>(gridEntity);

    auto& transformParent = ecs.Registry.get<Transform>(gridEntity);
    {
        transformParent.Name = "Grid " + std::to_string(gridParent.m_size.x) + " " + std::to_string(gridParent.m_size.y);

        if (transformParent.HasChildren())
        {
            for (auto child : transformParent)
            {
                Engine.ECS().DeleteEntity(child);
            }
        }
    }

    // Create new tiles
    for (int x = 0; x < gridParent.m_size.x; x++)
    {
        for (int y = 0; y < gridParent.m_size.y; y++)
        {
            auto entity = ecs.CreateEntity();
            auto& transform = ecs.CreateComponent<Transform>(entity);
            {
                transform.SetTranslation(glm::vec3(x * gridParent.m_tile_size, 0.0f, y * gridParent.m_tile_size) +
                                         glm::vec3(gridParent.m_tile_size / 2.0f, 0.0f, gridParent.m_tile_size / 2.0f));
                transform.Name = "Tile " + std::to_string(x) + " " + std::to_string(y);
                transform.SetParent(gridEntity);
            }
            auto& tile = ecs.CreateComponent<Tile>(entity);
            {
                tile.X = x;
                tile.Y = y;
            }
        }
    }
}

void bee::TileEditor::UpdateGridTileSize(entt::entity gridEntity)
{
    auto& ecs = Engine.ECS();

    auto& gridParent = ecs.Registry.get<Grid>(gridEntity);

    auto& transformParent = ecs.Registry.get<Transform>(gridEntity);

    // Update tile size

    for (auto tileEntity : transformParent)
    {
        auto& tileTransform = Engine.ECS().Registry.get<Transform>(tileEntity);
        auto& tile = Engine.ECS().Registry.get<Tile>(tileEntity);

        tileTransform.SetTranslation(glm::vec3(tile.X * gridParent.m_tile_size, 0.0f, tile.Y * gridParent.m_tile_size) +
                                     glm::vec3(gridParent.m_tile_size / 2.0f, 0.0f, gridParent.m_tile_size / 2.0f));
    }
}

void bee::TileEditor::UpdateTile(entt::entity tileEntity, std::pair<FileIO::Directory, std::string> pair)
{
    auto& transform = Engine.ECS().Registry.get<Transform>(tileEntity);

    if (transform.HasChildren())
    {
        for (auto child : transform)
        {
            Engine.ECS().DeleteEntity(child);
        }
    }
    auto& tile = Engine.ECS().Registry.get<Tile>(tileEntity);

    tile.m_modelPath = pair;

    auto model = Engine.Resources().Load<ModelResource>(pair.first, pair.second);

    model->Instantiate(tileEntity);
}

void bee::TileEditor::InspectGrid(entt::entity entity)
{
    if (ImGui::CollapsingHeader("Grid"))
    {
        ImGui::Indent();

        auto& gridParent = Engine.ECS().Registry.get<Grid>(entity);

        if (ImGui::InputInt2("Grid Size", glm::value_ptr(gridParent.m_size)))
        {
            UpdateGridSize(entity);
        }
        if (ImGui::InputFloat("Tile Size", &gridParent.m_tile_size))
        {
            UpdateGridTileSize(entity);
        }

        ImGui::Unindent();
    }
}

void bee::TileEditor::InspectTile(entt::entity entity)
{
    if (ImGui::CollapsingHeader("Tile"))
    {
        ImGui::Indent();

        ImGui::Text("Set Tile");

        ImGui::Indent();

        for (const auto& pair : m_importedModelPaths)
        {
            std::string path = std::string(magic_enum::enum_name(pair.first)) + " " + pair.second;
            if (ImGui::Button(path.c_str()))
            {
                UpdateTile(entity, pair);
            }
        }

        ImGui::Unindent();

        ImGui::Unindent();
    }
}
