#include "terrain/brushes/plateau_brush.hpp"

#include <core/engine.hpp>
#include <imgui/imgui.h>

#include "terrain/terrain_tools.hpp"
#include "terrain/terrain_editor.hpp"
#include "terrain/terrain_components.hpp"
#include "platform/opengl/terrain_pass_gl.hpp"
#include "platform/opengl/open_gl.hpp"

#include "core/input.hpp"
#include "glad/glad.h"
#include "rendering/image.hpp"
#include "core/resources.hpp"
#include "tools/log.hpp"

bee::PlateauBrush::PlateauBrush() : TerrainBrush("Plateau Brush")
{
    m_alphaComputeShader = Engine.Resources().Load<Shader>(FileIO::Directory::SharedAssets,
                                                           "/shaders/terrain/terrain_brush_height_plateau_alpha.comp");

    m_circleComputeShader = Engine.Resources().Load<Shader>(FileIO::Directory::SharedAssets,
                                                            "/shaders/terrain/terrain_brush_height_plateau_circle.comp");
}

void bee::PlateauBrush::Draw(float dt)
{
    if (m_selectTargetHeightFromTerrain)
    {
        const auto result = terrain_tools::GetTerrainHeight(m_position);

        if (result.first)
        {
            m_targetHeight = result.second;
        }
        m_selectTargetHeightFromTerrain = false;
        m_diameter = m_dummyDiameter;
        return;
    }

    BrushData brushData;

    brushData.positionUV = terrain_tools::GetUVCoordinate(m_position);
    brushData.radius = m_diameter / 2.0f * terrain_tools::GetTerrainHeightMapScale();
    brushData.intensity = m_intensity * dt;
    brushData.targetHeight = m_targetHeight;

    // Set brush data in the terrain pass
    auto& renderer = Engine.ECS().GetSystem<Renderer>();
    std::unique_ptr<TerrainPass>& terrainPassPtr = renderer.GetTerrainPass();
    terrainPassPtr->SetBrushData(brushData);
}

void bee::PlateauBrush::OnPanel()
{
#ifdef BEE_INSPECTOR
    if (ImGui::CollapsingHeader("Brush Settings"))
    {
        ImGui::Indent();

        ImGui::Text("Brush Type");

        if (ImGui::RadioButton("Default Circle", m_isCircle == true))
        {
            m_isCircle = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Custom Alpha Texture", m_isCircle == false))
        {
            m_isCircle = false;
        }

        if (!m_isCircle)
        {
            ImGui::Text("Drop an Alpha Texture Here (.png):");

            ImGui::Button("Drop Target", ImVec2(200, 50));
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload * assetPayload{ImGui::AcceptDragDropPayload("TEXTURE")})
                {
                    const auto* data{static_cast<const char*>(assetPayload->Data)};

                    auto relativePath{Engine.FileIO().GetRelativeAssetsPath(data)};

                    m_brushAlphaTexturePath = relativePath;
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Text("Last Dropped: %s", m_brushAlphaTexturePath.c_str());
        }

        ImGui::Separator();

        if (ImGui::InputFloat("Brush Size (Diameter)", &m_diameter))
        {
            m_diameter = std::max(m_diameter, 1.0f);
        }
        if (ImGui::InputFloat("Brush Intensity", &m_intensity))
        {
            m_intensity = std::max(m_intensity, 0.0f);
        }

        ImGui::Separator();

        ImGui::InputFloat("Target Height", &m_targetHeight);

        if (ImGui::Button("Set Target Height From Terrain"))
        {
            if (!m_selectTargetHeightFromTerrain)
            {
                m_selectTargetHeightFromTerrain = true;
                m_dummyDiameter = m_diameter;
                m_diameter = 1.0f;
            }
        }

        if (m_selectTargetHeightFromTerrain)
        {
            ImGui::Text("Click on your desired point on the terrain to set the target height.");
        }

        ImGui::Unindent();
    }
#endif
}

void bee::PlateauBrush::Input(float)
{
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::LeftBracket))
        m_diameter = std::max(m_diameter - 1.0f, 0.0f);
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::RightBracket))
        m_diameter += 1.0f;
}

void bee::PlateauBrush::DispatchComputeShader()
{
    if (m_isCircle)
    {
        DispatchCircleComputeShader();
    }
    else
    {
        DispatchAlphaComputeShader();
    }
}

void bee::PlateauBrush::DispatchCircleComputeShader()
{
    auto& ecs = Engine.ECS();
    auto& currentTerrainEntity = bee::terrain_tools::GetCachedTerrainEntity();
    auto terrain_image_id =
        ecs.Registry.get<TerrainMeshRenderer>(currentTerrainEntity).Material->HeightMapTexture->Image->GetTextureId();

    m_circleComputeShader->Activate();

    glBindImageTexture(0, terrain_image_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

    const int workGroupsX = static_cast<int>(m_diameter * terrain_tools::GetTerrainHeightMapScale());
    const int workGroupsY = static_cast<int>(m_diameter * terrain_tools::GetTerrainHeightMapScale());
    glDispatchCompute(workGroupsX, workGroupsY, 1);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    m_circleComputeShader->Deactivate();
}

void bee::PlateauBrush::DispatchAlphaComputeShader()
{
    if (m_brushAlphaTexturePath.empty())
    {
        Log::Warn("Warning: No brush alpha texture set.");
        return;
    }

    auto& ecs = Engine.ECS();
    auto& currentTerrainEntity = bee::terrain_tools::GetCachedTerrainEntity();
    auto terrain_image_id =
        ecs.Registry.get<TerrainMeshRenderer>(currentTerrainEntity).Material->HeightMapTexture->Image->GetTextureId();
    auto texture_id = Engine.Resources().Load<Image>(FileIO::Directory::Assets, m_brushAlphaTexturePath)->GetTextureId();

    m_alphaComputeShader->Activate();

    glBindImageTexture(0, terrain_image_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
    glBindImageTexture(2, texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

    const int workGroupsX = static_cast<int>(m_diameter * terrain_tools::GetTerrainHeightMapScale());
    const int workGroupsY = static_cast<int>(m_diameter * terrain_tools::GetTerrainHeightMapScale());
    glDispatchCompute(workGroupsX, workGroupsY, 1);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    m_alphaComputeShader->Deactivate();
}
