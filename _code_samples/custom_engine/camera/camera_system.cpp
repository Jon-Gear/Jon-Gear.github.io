#include "camera/camera_system.hpp"

#include "core/device.hpp"
#include "core/engine.hpp"
#include "core/input.hpp"
#include "core/resource_manager.hpp"

#include "rendering/render_xsr.hpp"
#include "rendering/render_xsr_components.hpp"
#include "tools/inspector.hpp"
#include "tools/log.hpp"
#include "tools/tools.hpp"

namespace bee
{
struct FreeFlyingControl
{
    glm::vec2 m_mousePos;
};

}  // namespace bee

using namespace bee;
using namespace glm;

CameraSystem::CameraSystem()
{
    IgnorePause = true;
    auto& ecs = Engine.ECS();

    // Create the camera
    auto cameraEntity = ecs.CreateEntity();
    auto& transform = ecs.CreateComponent<Transform>(cameraEntity);
    transform.Name = "Camera";
    auto& camera = ecs.CreateComponent<Camera>(cameraEntity);
    ecs.CreateComponent<FreeFlyingControl>(cameraEntity);
    ecs.CreateComponent<HideTransform>(cameraEntity);

    transform.SetTranslation(glm::vec3(0, 10, 45));
    transform.SetRotation(glm::quatLookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    camera.Projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 1.f, 300.0f);
}

void CameraSystem::CameraMovementControls(float dt)
{
    auto& ecs = Engine.ECS();

    for (auto [entity, control, transform] : ecs.Registry.view<FreeFlyingControl, Transform>().each())
    {
        // Rotation control
        const auto& mousePos = Engine.Input().GetMousePosition();

        if (Engine.Input().GetMouseButton(Input::MouseButton::Right) && mousePos != control.m_mousePos)
        {
            const float sensitivity = 0.003f;
            const auto& dMouseScaled = (mousePos - control.m_mousePos) * sensitivity;

            quat initial(transform.GetRotation());

            quat yawQuat = angleAxis(-dMouseScaled.x, vec3(0.0f, 1.0f, 0.0f));
            quat pitchQuat = angleAxis(-dMouseScaled.y, vec3(1.0f, 0.0f, 0.0f));

            transform.SetRotation(normalize(yawQuat * initial * pitchQuat));
        }

        control.m_mousePos = mousePos;

        // Translation control

        vec3 dir(0);

        if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::W)) dir.z -= 1;
        if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::S)) dir.z += 1;
        if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::A)) dir.x -= 1;
        if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::D)) dir.x += 1;

        dir = transform.GetRotation() * dir;

        if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::Q)) dir.y -= 1;
        if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::E)) dir.y += 1;

        if (dir != vec3(0))
        {
            float speed = Engine.Input().GetKeyboardKey(Input::KeyboardKey::LeftShift) ? 20.f : 10.f;
            transform.SetTranslation(transform.GetTranslation() + normalize(dir) * speed * dt);
        }
    }
}

void CameraSystem::Update(float dt) { CameraMovementControls(dt); }
