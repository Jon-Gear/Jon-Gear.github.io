#pragma once

#include <map>
#include <set>
#include <string>
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <glm/glm.hpp>
#include "core/ecs.hpp"
#include "core/engine.hpp"
#include "core/geometry2d.hpp"

namespace bee
{

struct Transform;

// Used to hide entities from hierarchy.
struct HideTransform
{
    uint8_t : 8;
};

class Inspector
{
public:
    Inspector();
    ~Inspector();

    void SetVisible(bool visible) { m_visible = visible; }
    bool GetVisible() const { return m_visible; }
    void Inspect(float dt);

    Entity SelectedEntity() { return m_selectedEntity; }
    void SetSelectedEntity(Entity newEntity) { m_selectedEntity = newEntity; }

    const geometry2d::AABB& GetGameViewportBounds() const { return m_gameViewportBounds; }

    unsigned int InspectorColorbuffer = 0;

private:
    void Dockspace();

    void Toolbar();
    void Hierarchy();
    void Properties();
    void Viewport();
    void Panels();
    void Gizmo();
    void SimulationControls();

    void InspectHierarchy(Entity entity, Transform& transform, std::set<Entity>& inspected);
    void AddToInspected(Entity entity, std::set<Entity>& inspected);

    entt::entity m_selectedEntity = entt::null;
    ImGuizmo::OPERATION m_gizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE m_gizmoMode = ImGuizmo::WORLD;

    bool m_config = false;
    bool m_visible = true;
    std::map<std::string, bool> m_openWindows;
    geometry2d::AABB m_gameViewportBounds = geometry2d::AABB({0, 0}, {0, 0});
};

}  // namespace bee
