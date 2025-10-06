#include "terrain/brushes/height_brush.hpp"

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

bee::HeightBrush::HeightBrush() : TerrainBrush("Height Brush")
{
    m_alphaComputeShader = Engine.Resources().Load<Shader>(FileIO::Directory::SharedAssets,
                                                           "/shaders/terrain/terrain_brush_height_default_alpha.comp");

    m_circleComputeShader = Engine.Resources().Load<Shader>(FileIO::Directory::SharedAssets,
                                                            "/shaders/terrain/terrain_brush_height_default_circle.comp");
}

void bee::HeightBrush::Draw(float dt)
{
    BrushData brushData;

    brushData.positionUV = terrain_tools::GetUVCoordinate(m_position);
    brushData.radius = m_diameter / 2.0f * terrain_tools::GetTerrainHeightMapScale();
    brushData.intensity = m_intensity * dt * (m_isRaising ? 1 : -1);

    // Set brush data in the terrain pass
    auto& renderer = Engine.ECS().GetSystem<Renderer>();
    std::unique_ptr<TerrainPass>& terrainPassPtr = renderer.GetTerrainPass();
    terrainPassPtr->SetBrushData(brushData);
}

void bee::HeightBrush::OnPanel()
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

        if (ImGui::RadioButton("Raise", m_isRaising == true))
        {
            m_isRaising = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Lower", m_isRaising == false))
        {
            m_isRaising = false;
        }
        ImGui::Unindent();
    }
#endif
}

void bee::HeightBrush::Input(float)
{
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::LeftBracket))
        m_diameter = std::max(m_diameter - 1.0f, 0.0f);
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::RightBracket))
        m_diameter += 1.0f;
}

void bee::HeightBrush::DispatchCircleComputeShader()
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

void bee::HeightBrush::DispatchAlphaComputeShader()
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

void bee::HeightBrush::DispatchComputeShader()
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
