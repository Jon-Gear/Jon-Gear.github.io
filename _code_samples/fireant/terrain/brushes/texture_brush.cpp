#include "terrain/brushes/texture_brush.hpp"

#include <imgui/imgui.h>

#include "core/ecs.hpp"
#include "core/engine.hpp"
#include "core/input.hpp"
#include "core/resources.hpp"

#include "platform/opengl/terrain_pass_gl.hpp"
#include "platform/opengl/open_gl.hpp"

#include "rendering/image.hpp"
#include "rendering/render.hpp"
#include "rendering/debug_render.hpp"

#include "terrain/terrain_editor.hpp"

#include "glad/glad.h"
#include <terrain/terrain_tools.hpp>
#include "tools/log.hpp"

bee::TextureBrush::TextureBrush() : TerrainBrush("Texture Brush")
{
    m_circleComputeShader =
        Engine.Resources().Load<Shader>(FileIO::Directory::SharedAssets, "/shaders/terrain/terrain_brush_texture_circle.comp");

    m_alphaComputeShader =
        Engine.Resources().Load<Shader>(FileIO::Directory::SharedAssets, "/shaders/terrain/terrain_brush_texture_alpha.comp");
}

void bee::TextureBrush::Draw(float dt)
{
    BrushData brushData;

    brushData.positionUV = terrain_tools::GetUVCoordinate(m_position);
    brushData.radius = m_diameter / 2.0f * terrain_tools::GetTerrainTextureScale();
    brushData.intensity = m_intensity * dt;

    // Set brush data in the terrain pass
    auto& renderer = Engine.ECS().GetSystem<Renderer>();
    std::unique_ptr<TerrainPass>& terrainPassPtr = renderer.GetTerrainPass();
    terrainPassPtr->SetBrushData(brushData);
}

void bee::TextureBrush::OnPanel()
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

            ImGui::Button("Drop Target ##AlphaTexture", ImVec2(200, 50));
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
        ImGui::Text("Drop an Terrain Texture Here (.png):");

        ImGui::Button("Drop Target ##TerrainTexture", ImVec2(200, 50));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload * assetPayload{ImGui::AcceptDragDropPayload("TEXTURE")})
            {
                const auto* data{static_cast<const char*>(assetPayload->Data)};

                auto relativePath{Engine.FileIO().GetRelativeAssetsPath(data)};

                m_brushTerrainTexturePath = relativePath;
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::Text("Last Dropped: %s", m_brushTerrainTexturePath.c_str());
    }
#endif
}

void bee::TextureBrush::Input(float)
{
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::LeftBracket))
        m_diameter = std::max(m_diameter - 1.0f, 0.0f);
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::RightBracket))
        m_diameter += 1.0f;
}

void bee::TextureBrush::DispatchComputeShader()
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

void bee::TextureBrush::DispatchCircleComputeShader()
{
    if (m_brushTerrainTexturePath.empty())
    {
        Log::Warn("Warning: No brush terrain texture set.");
        return;
    }

    auto& ecs = Engine.ECS();
    auto& currentTerrainEntity = bee::terrain_tools::GetCachedTerrainEntity();
    auto terrain_image_id =
        ecs.Registry.get<TerrainMeshRenderer>(currentTerrainEntity).Material->BaseColorTexture->Image->GetTextureId();

    auto brush_terrain_texture_id =
        Engine.Resources().Load<Image>(FileIO::Directory::Assets, m_brushTerrainTexturePath)->GetTextureId();

    m_circleComputeShader->Activate();

    glBindImageTexture(0, terrain_image_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
    glBindImageTexture(3, brush_terrain_texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

    const int workGroupsX = static_cast<int>(m_diameter * terrain_tools::GetTerrainTextureScale());
    const int workGroupsY = static_cast<int>(m_diameter * terrain_tools::GetTerrainTextureScale());
    glDispatchCompute(workGroupsX, workGroupsY, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    m_circleComputeShader->Deactivate();
}

void bee::TextureBrush::DispatchAlphaComputeShader()
{
    if (m_brushTerrainTexturePath.empty())
    {
        Log::Warn("Warning: No brush terrain texture set.");
        return;
    }

    if (m_brushAlphaTexturePath.empty())
    {
        Log::Warn("Warning: No brush alpha texture set.");
        return;
    }

    auto& ecs = Engine.ECS();
    auto& currentTerrainEntity = bee::terrain_tools::GetCachedTerrainEntity();
    auto terrain_image_id =
        ecs.Registry.get<TerrainMeshRenderer>(currentTerrainEntity).Material->BaseColorTexture->Image->GetTextureId();

    auto brush_terrain_texture_id =
        Engine.Resources().Load<Image>(FileIO::Directory::Assets, m_brushTerrainTexturePath)->GetTextureId();

    auto brush_texture_id = Engine.Resources().Load<Image>(FileIO::Directory::Assets, m_brushAlphaTexturePath)->GetTextureId();

    m_alphaComputeShader->Activate();

    glBindImageTexture(0, terrain_image_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
    glBindImageTexture(2, brush_texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
    glBindImageTexture(3, brush_terrain_texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

    const int workGroupsX = static_cast<int>(m_diameter * terrain_tools::GetTerrainTextureScale());
    const int workGroupsY = static_cast<int>(m_diameter * terrain_tools::GetTerrainTextureScale());
    glDispatchCompute(workGroupsX, workGroupsY, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    m_alphaComputeShader->Deactivate();
}
