#include "terrain/terrain_brush.hpp"

#include <core/geometry2d.hpp>

#include "core/ecs.hpp"
#include "core/engine.hpp"
#include "core/input.hpp"
#include "core/transform.hpp"
#include "platform/opengl/image_gl.hpp"
#include "platform/opengl/open_gl.hpp"
#include "rendering/debug_render.hpp"
#include "rendering/render_components.hpp"
#include "terrain/terrain_components.hpp"
#include "terrain/terrain_editor.hpp"
#include "terrain/terrain_tools.hpp"
#include "tools/log.hpp"

void bee::TerrainBrush::Update(float dt)
{
    Input(dt);

    auto result = terrain_tools::GetPointOnTerrain(m_position);

    if (!result)
    {
        return;
    }

    if (!Engine.Input().GetMouseButton(Input::MouseButton::Left))
    {
        return;
    }

    isBrushActivated = true;

    auto& ecs = Engine.ECS();

    auto& currentTerrainEntity = terrain_tools::GetCachedTerrainEntity();

    auto& renderer = ecs.Registry.get<TerrainMeshRenderer>(currentTerrainEntity);

    if (!renderer.Material->UseHeightMapTexture)
    {
        return;
    }

    if (!terrain_tools::IsPointOnTerrain(m_position))
    {
        return;
    }

    Draw(dt);
}

// Draws a circle for the brush
void bee::TerrainBrush::Render() const
{
    const float radius = m_diameter / 2.0f;

    constexpr float dt = glm::two_pi<float>() / 32.0f;  // Step angle for 32 segments
    float t = 0.0f;

    // Generate circle points with adjusted height
    glm::vec3 v0(m_position.x + radius * cos(t), m_position.y - radius, m_position.z + radius * sin(t));

    v0.y = terrain_tools::GetTerrainHeight(v0).second;

    for (; t < glm::two_pi<float>(); t += dt)
    {
        glm::vec3 v1(m_position.x + radius * cos(t + dt), m_position.y - radius, m_position.z + radius * sin(t + dt));

        v1.y = terrain_tools::GetTerrainHeight(v1).second;

        Engine.DebugRenderer().AddLine(DebugCategory::Rendering, v0, v1, Colors::Orange);
        v0 = v1;
    }
}
