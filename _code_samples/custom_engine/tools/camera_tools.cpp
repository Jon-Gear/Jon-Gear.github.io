#include "tools/camera_tools.hpp"
#include "core/engine.hpp"
#include "core/input.hpp"
#include "core/transform.hpp"

#include "rendering/render_xsr_components.hpp"
#include "tile_editor/tile_components.hpp"

std::pair<bool, entt::entity> camera_tools::GetTileOnGrid(entt::entity gridEntity)
{
    entt::entity foundTile = entt::null;

    for (const auto& [entity, camera, cam_transform] : bee::Engine.ECS().Registry.view<bee::Camera, bee::Transform>().each())
    {
        auto ray = GetRayFromScreenToWorld(cam_transform, camera.Projection);

        foundTile = FindIntersection(cam_transform.GetTranslation(), ray, gridEntity);

        if (foundTile != entt::null)
        {
            return std::make_pair(true, foundTile);
        }
    }

    return std::make_pair(false, entt::null);
}

glm::vec3 camera_tools::GetRayFromScreenToWorld(bee::Transform& cameraTransform, const glm::mat4& cameraProjection)
{
    const glm::vec2 mousePosition = bee::Engine.Input().GetMousePositionInViewport();

    // Derive the view matrix from camera transform (assuming it's a world transform)
    const glm::mat4 viewMatrix = inverse(cameraTransform.World());  // Convert world-space transform to view matrix

    // Compute the inverse of the View-Projection matrix
    const glm::mat4 invVP = inverse(cameraProjection * viewMatrix);

    // Unproject near and far points
    const glm::vec4 farPoint = invVP * glm::vec4(mousePosition, 1.0f, 1.0f);

    const glm::vec3 nearPos = cameraTransform.GetTranslation();
    const glm::vec3 farPos = glm::vec3(farPoint) / farPoint.w;

    // Compute ray direction
    const glm::vec3 dir = normalize(farPos - nearPos);

    return dir;
}

entt::entity camera_tools::FindIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, entt::entity gridEntity)
{
    // Check if the ray is parallel to the plane
    if (glm::abs(rayDirection.y) < 1e-6f)
    {
        // Ray is parallel to the plane
        return entt::null;
    }

    auto& gridTransform = bee::Engine.ECS().Registry.get<bee::Transform>(gridEntity);
    auto& grid = bee::Engine.ECS().Registry.get<bee::Grid>(gridEntity);

    // Solve for t
    float t = (gridTransform.GetTranslation().y - rayOrigin.y) / rayDirection.y;

    if (t < 0)
    {
        // Intersection is behind the ray's origin
        return entt::null;
    }

    // Calculate the hit point
    glm::vec3 hit = rayOrigin + t * rayDirection;

    for (auto child : gridTransform)
    {
        auto& tileTransform = bee::Engine.ECS().Registry.get<bee::Transform>(child);

        glm::vec3 position = tileTransform.GetTranslation() + gridTransform.GetTranslation();

        if ((position.x - grid.m_tile_size / 2.0f < hit.x && hit.x < position.x + grid.m_tile_size / 2.0f) &&
            (position.z - grid.m_tile_size / 2.0f < hit.z && hit.z < position.z + grid.m_tile_size / 2.0f))
        {
            return child;
        }
    }

    return entt::null;
}
