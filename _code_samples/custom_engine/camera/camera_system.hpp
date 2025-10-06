#pragma once

#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "core/ecs.hpp"
#include "core/transform.hpp"

namespace bee
{

struct Camera;
struct Transform;

class CameraSystem : public bee::System
{
public:
    CameraSystem();
    void Update(float dt) override;

private:
    void CameraMovementControls(float dt);
};
}  // namespace bee