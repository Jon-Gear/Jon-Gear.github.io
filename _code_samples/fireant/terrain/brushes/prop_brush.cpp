#include "terrain/brushes/prop_brush.hpp"

#include "core/engine.hpp"
#include "core/input.hpp"
#include "core/resources.hpp"

#include "platform/opengl/terrain_pass_gl.hpp"

#include "rendering/model.hpp"

#include "terrain/terrain_editor.hpp"
#include "terrain/terrain_tools.hpp"

#include "tools/asset_manager/engine_ui.hpp"
#include "tools/entity_manip.hpp"
#include "tools/inspector.hpp"
#include "tools/scene_serialization_tools.hpp"
#include "tools/tools.hpp"

#include <imgui/imgui.h>
#include "tools/save_system.hpp"
using SLSystem = bee::SavingLoadingSystem;

void bee::PropBrush::Draw(float)
{
    if (m_payloadType == PayloadType::None)
    {
        return;
    }

    for (int i = 0; i < m_propsPerStroke; i++)
    {
        auto& ecs = Engine.ECS();

        const auto propEntity{ecs.CreateEntity()};
        auto& propTransform{ecs.CreateComponent<Transform>(propEntity)};
        const auto& terrainEntity{terrain_tools::GetCachedTerrainEntity()};

        glm::vec3 translationVariation{rng::RandomPointInCircle(m_diameter / 2.0f)};

        glm::vec3 propTranslation{m_position + translationVariation};
        propTranslation.y = terrain_tools::GetTerrainHeight(propTranslation).second;

        const glm::quat rotationVariation{
            glm::angleAxis(rng::GetRandomFloat(0.0f, glm::two_pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f))};

        glm::quat propRotation{m_placeAlongNormals ? terrain_tools::GetTerrainNormal(propTranslation).second
                                                   : glm::quat(1, 0, 0, 0)};

        propRotation *= rotationVariation;

        propTransform.SetParent(terrainEntity);
        propTransform.SetTranslation(propTranslation);
        propTransform.SetRotation(propRotation);

        // Load the model using the relative path
        switch (m_payloadType)
        {
            case PayloadType::Asset:
            {
                const auto model{Engine.Resources().Load<Model>(FileIO::Directory::Assets, m_propPath)};
                model->Instantiate(propEntity);
                break;
            }
            case PayloadType::Scene:
            {
                const auto duplicateEntity{emanip::Duplicate<Transform>(m_sceneRoot, entt::null)};

                // Make placed prop visible (because the template is hidden).
                Engine.ECS().Registry.remove<HideTransform>(duplicateEntity);
                MeshRenderer::SetVisible(duplicateEntity, true);

                // Place at location.
                auto& transform(Engine.ECS().Registry.get<Transform>(duplicateEntity));
                transform.SetParent(terrainEntity);
                transform.SetTranslation(propTranslation);
                transform.SetRotation(propRotation);
            }
            default:
            {
                break;
            }
        }
    }
}

void bee::PropBrush::OnPanel()
{
#ifdef BEE_INSPECTOR
    if (ImGui::CollapsingHeader("Brush Settings"))
    {
        ImGui::Indent();

        if (ImGui::InputFloat("Brush Size (Diameter)", &m_diameter))
        {
            m_diameter = std::max(m_diameter, 1.0f);
        }
        if (ImGui::InputInt("Props Per Stroke", &m_propsPerStroke))
        {
            m_propsPerStroke = std::max(m_propsPerStroke, 1);
        }

        ImGui::Separator();

        ImGui::Checkbox("Place Along Normals", &m_placeAlongNormals);

        ImGui::Separator();
        ImGui::Text("Drop Asset Here:");

        ImGui::Button("Drop Target", ImVec2(200, 50));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload * assetPayload{ImGui::AcceptDragDropPayload("ASSET")})
            {
                const auto* data{static_cast<const char*>(assetPayload->Data)};

                auto relativePath{Engine.FileIO().GetRelativeAssetsPath(data)};

                m_propPath = relativePath;
                m_payloadType = PayloadType::Asset;
            }
            else if (const ImGuiPayload * scenePayload{ImGui::AcceptDragDropPayload("SCENE")})
            {
                // Extract data as string path to asset.
                const auto* data{static_cast<const char*>(scenePayload->Data)};
                auto relativePath{Engine.FileIO().GetRelativeAssetsPath(data)};

                // Do not allow adding the current scene to itself - creates circular logic and an infinite save/load loop!

                if (SavingLoadingSystem::GetSceneInfo().GetPathname() == std::string(data))
                {
                    Log::Info("Cannot add scene to itself!");
                    m_propPath.clear();
                    m_payloadType = PayloadType::None;
                    return;
                }

                Log::Info("Added scene {} as prop brush payload", relativePath);

                // Load the scene, but first delete the previous scene if it exists.
                // Give it a transform and the special InstancedScene component.
                // Parent the scene to the root and hide all scene components.
                if (m_sceneRoot != entt::null)
                {
                    Engine.ECS().DeleteEntity(m_sceneRoot);
                }

                m_sceneRoot = Engine.ECS().CreateEntity();

                auto& iScene{Engine.ECS().CreateComponent<scene_serialization::InstancedScene>(m_sceneRoot)};
                iScene.InstancePath = relativePath;

                auto& transform{Engine.ECS().CreateComponent<Transform>(m_sceneRoot)};
                const auto startPos{relativePath.find_last_of('/')};
                const auto endPos{relativePath.find_last_of('.')};
                transform.Name = relativePath.substr(startPos + 1, (endPos - startPos - 1));

                scene_serialization::LoadSceneAsChild(m_sceneRoot, FileIO::Directory::Assets, relativePath);

                // Also hide the root entity so it is not part of the actual scene and used only used as a template.
                Engine.ECS().CreateComponent<HideTransform>(m_sceneRoot);
                MeshRenderer::SetVisible(m_sceneRoot, false);

                m_propPath = relativePath;
                m_payloadType = PayloadType::Scene;
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::Text("Last Dropped: %s", m_propPath.c_str());

        ImGui::Text("This is a prop brush");
    }
#endif
}

void bee::PropBrush::Input(float)
{
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::LeftBracket))
    {
        m_diameter = std::max(m_diameter - 1.0f, 0.0f);
    }

    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::RightBracket))
    {
        m_diameter += 1.0f;
    }
}
