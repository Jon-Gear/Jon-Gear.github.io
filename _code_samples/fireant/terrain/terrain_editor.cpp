#include "terrain/terrain_editor.hpp"

#include "core/engine.hpp"
#include "core/input.hpp"
#include "core/resources.hpp"
#include "math/geometry.hpp"
#include "rendering/image.hpp"
#include "rendering/model.hpp"
#include "rendering/render_components.hpp"
#include "terrain/brushes/height_brush.hpp"
#include "terrain/brushes/plateau_brush.hpp"
#include "terrain/brushes/none_brush.hpp"
#include "terrain/brushes/prop_brush.hpp"
#include "terrain/brushes/smoothing_brush.hpp"
#include "terrain/brushes/texture_brush.hpp"
#include "terrain/terrain_components.hpp"
#include "terrain/terrain_data.hpp"
#include "terrain/terrain_tools.hpp"
#include "tools/inspector.hpp"
#include "tools/log.hpp"
#include "tools/scene_serialization_tools.hpp"
#include "tools/serialization_cereal.hpp"
#include "tools/tools.hpp"

#include <imgui/imgui.h>

using namespace bee;

TerrainEditor::TerrainEditor()
{
    Title = "Terrain Editor";
#ifdef BEE_INSPECTOR
    m_brushes.push_back(std::make_unique<NoneBrush>());
    m_brushes.push_back(std::make_unique<HeightBrush>());
    m_brushes.push_back(std::make_unique<TextureBrush>());
    m_brushes.push_back(std::make_unique<PlateauBrush>());
    m_brushes.push_back(std::make_unique<PropBrush>());
    // m_brushes.push_back(std::make_unique<SmoothingBrush>());

#endif  // BEE_INSPECTOR
}

#ifdef BEE_INSPECTOR
void TerrainEditor::Update(float dt)
{
    if (!terrain_tools::IsTerrainCached())
    {
        return;
    }

    if (!Engine.Input().IsMouseInViewport())
    {
        return;
    }

    if (m_currentBrushIndex == 0)
    {
        return;
    }

    m_brushes[m_currentBrushIndex]->Update(dt);
}

void TerrainEditor::Render()
{
    if (!terrain_tools::IsTerrainCached())
    {
        return;
    }

    if (!Engine.Input().IsMouseInViewport())
    {
        return;
    }

    if (m_currentBrushIndex == 0)
    {
        return;
    }

    if (!m_renderBrushDebug)
    {
        return;
    }

    m_brushes[m_currentBrushIndex]->Render();
}

void bee::TerrainEditor::DispatchBrushComputeShaders()
{
    if (!terrain_tools::IsTerrainCached())
    {
        return;
    }

    if (!Engine.Input().IsMouseInViewport())
    {
        return;
    }

    if (m_currentBrushIndex == 0)
    {
        return;
    }

    if (!m_brushes[m_currentBrushIndex]->isBrushActivated)
    {
        return;
    }

    m_brushes[m_currentBrushIndex]->DispatchComputeShader();
    m_brushes[m_currentBrushIndex]->isBrushActivated = false;
    terrain_tools::CacheTerrain();
}

#endif

#ifdef BEE_INSPECTOR
void TerrainEditor::OnEntity(entt::entity entity)
{
    if (!Engine.ECS().Registry.all_of<Terrain>(entity))
    {
        return;
    }

    if (ImGui::CollapsingHeader("Terrain", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto& _terrain = Engine.ECS().Registry.get<Terrain>(entity);

        static int32_t res = 20;
        if (ImGui::DragInt("Regenerate mesh with resolution", &res, 1.0f, 1, 100))
        {
            auto& renderer = Engine.ECS().Registry.get<TerrainMeshRenderer>(entity);
            renderer.Mesh = terrain_tools::CreateTerrainMesh(_terrain, res);
        }
        static constexpr size_t name_buffer_size{50};
        static char name_buffer[name_buffer_size]{"Default\0"};

        if (ImGui::InputText("Set Terrain File Name", name_buffer, name_buffer_size))
        {
            bool is_name_valid{strlen(name_buffer) > 1};

            if (is_name_valid)
            {
                _terrain.TerrainFileName = name_buffer;
            }
        }
        ImGui::Text("Terrain Height Map Path: %s", _terrain.HeightMapImagePath.c_str());
        ImGui::Text("Terrain Texture Map Path: %s", _terrain.TerrainTextureImagePath.c_str());
    }
}

void TerrainEditor::OnPanel()
{
    if (ImGui::Button("Recache Terrain"))
    {
        terrain_tools::CacheTerrain();
    }

    if (!terrain_tools::IsTerrainCached())
    {
        ImGui::Text("No terrain cached. Please cache the terrain.");
        return;
    }

    auto& transform = Engine.ECS().Registry.get<Transform>(terrain_tools::GetCachedTerrainEntity());
    std::string text = "Current Terrain: " + transform.Name;
    ImGui::Text("%s", text.c_str());

    if (ImGui::CollapsingHeader("Terraforming"))
    {
        ImGui::Checkbox("Debug Brush Render", &m_renderBrushDebug);

        ImGui::Combo(
            "Select Brush",
            &m_currentBrushIndex,
            [](void* data, int idx, const char** out_text)
            {
                auto& brushes = *static_cast<std::vector<std::unique_ptr<TerrainBrush>>*>(data);
                if (idx >= 0 && idx < static_cast<int>(brushes.size()))
                {
                    *out_text = brushes[idx]->GetName().c_str();
                    return true;
                }
                return false;
            },
            &m_brushes,
            static_cast<int>(m_brushes.size()));

        m_brushes[m_currentBrushIndex]->OnPanel();

        ImGui::Indent();
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Saving Loading"))
    {
        ImGui::Dummy({0, 5});
        ImGui::Text("Save Terrain Data");
        ImGui::Separator();
        {
            static constexpr size_t name_buffer_size{50};
            static char name_buffer[name_buffer_size]{"TerrainDataName\0"};

            ImGui::InputText("Save Terrain Data Name", name_buffer, name_buffer_size);

            // Save the terrain as a separate scene.
            if (ImGui::Button("Save Terrain Data"))
            {
                bool is_name_valid{strlen(name_buffer) > 1};

                if (is_name_valid)
                {
                    auto& terrain = Engine.ECS().Registry.get<Terrain>(terrain_tools::GetCachedTerrainEntity());

                    terrain.TerrainFileName = name_buffer;

                    std::string filename{"terrain/"};

                    filename.append(name_buffer).append(".tscn");

                    scene_serialization::SavePrefab(terrain_tools::GetCachedTerrainEntity(),
                                                    FileIO::Directory::Assets,
                                                    filename);
                }
            }
        }

        ImGui::Dummy({0, 5});
        ImGui::Text("Load Terrain Data");
        ImGui::Separator();
        {
            static constexpr size_t name_buffer_size{50};
            static char name_buffer[name_buffer_size]{"TerrainDataName\0"};

            ImGui::InputText("Load Terrain Data Name", name_buffer, name_buffer_size);

            // Save the grid as a JSON file that can be loaded by the game. Requires a flow field name.
            if (ImGui::Button("Load Terrain Data"))
            {
                bool is_name_valid{strlen(name_buffer) > 1};

                if (is_name_valid)
                {
                    std::string filename{"terrain/"};

                    filename.append(name_buffer).append(Engine.FileIO().GetExtension(FileIO::FileType::Scene));

                    scene_serialization::LoadScene(FileIO::Directory::Assets, filename);
                }
            }
        }
    }

    if (ImGui::CollapsingHeader("Navigation"))
    {
        m_flowFieldEditor.Show();
    }
}
#endif
