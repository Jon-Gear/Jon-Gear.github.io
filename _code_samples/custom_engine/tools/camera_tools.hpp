#pragma once
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "core/transform.hpp"

namespace camera_tools
{
std::pair<bool, entt::entity> GetTileOnGrid(entt::entity gridEntity);
glm::vec3 GetRayFromScreenToWorld(bee::Transform& cameraTransform, const glm::mat4& cameraProjection);
entt::entity FindIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, entt::entity gridEntity);

}  // namespace camera_tools
