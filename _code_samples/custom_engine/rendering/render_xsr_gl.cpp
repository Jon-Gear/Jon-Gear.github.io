#include "platform/opengl/render_xsr_gl.hpp"

#include <imgui/imgui.h>
#include <tinygltf/stb_image.h>  // Implementation of stb_image is in gltf_loader.cpp

#include <xsr/include/xsr.hpp>

#include <glm/glm.hpp>

#include "core/device.hpp"
#include "core/ecs.hpp"
#include "core/engine.hpp"
#include "core/resource_manager.hpp"
#include "core/transform.hpp"
#include "tools/inspector.hpp"

#include "tools/log.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "tools/xsr_render_tools.hpp"

#include "rendering/mesh_resource.hpp"
#include "rendering/texture_resource.hpp"

using namespace bee;
using namespace glm;
using namespace std;

RendererXSR::RendererXSR()
{
    Title = "Renderer XSR";

    xsr::device_configuration device_config;
    device_config.width = Engine.Device().GetWidth();
    device_config.height = Engine.Device().GetHeight();
    device_config.title = "XSR Demo";

    // Initialize the device and window
    bool success = false;
    success = xsr::device::initialize(device_config, Engine.Device().GetWindow());
    assert(success);

    // Create render configuration
    xsr::render_configuration render_config;
    render_config.enable_shadows = true;
    render_config.texture_filter = xsr::render_configuration::texture_filtering::nearest;

    // Initialize the renderer
    success = xsr::initialize(render_config);
    assert(success);

    CreateFrameBuffers();
}

RendererXSR::~RendererXSR()
{
    DeleteFrameBuffers();
    xsr::shutdown();
}

void RendererXSR::CreateFrameBuffers()
{
    m_width = Engine.Device().GetWidth();
    m_height = Engine.Device().GetHeight();

    m_mainFrameBuffer = std::make_unique<FrameBuffer>(m_width, m_height);
}

void RendererXSR::DeleteFrameBuffers()
{
    // todo: delete the frame buffer;
}

void RendererXSR::Render()
{
    m_mainFrameBuffer->Bind();

    for (const auto& [e, light, lightTransform] : Engine.ECS().Registry.view<LightXSR, Transform>().each())
    {
        if (light.m_type == LightXSR::Type::Directional)
        {
            xsr::render_directional_light(glm::value_ptr(glm::conjugate(lightTransform.GetRotation())),
                                          glm::value_ptr(light.m_color));
        }

        if (light.m_type == LightXSR::Type::Point)
        {
            xsr::render_point_light(glm::value_ptr(lightTransform.GetTranslation()),
                                    light.m_range,
                                    glm::value_ptr(light.m_color));
        }
    }

    for (const auto& [e, camera, cameraTransform] : Engine.ECS().Registry.view<Camera, Transform>().each())
    {
        for (const auto& [entity, model, transform] : Engine.ECS().Registry.view<MeshRendererXSR, Transform>().each())
        {
            xsr::render_mesh(glm::value_ptr(transform.World()),
                             *model.Mesh->GetHandleXSR(),
                             *model.Texture->GetHandleXSR(),
                             glm::value_ptr(model.Multiplier),
                             glm::value_ptr(model.Tint));
        }

        xsr::render(glm::value_ptr(inverse(cameraTransform.World())), glm::value_ptr(camera.Projection));
    }
    m_mainFrameBuffer->Unbind();

    bool blitToScreen = false;

#ifdef BEE_INSPECTOR
    Engine.Inspector().InspectorColorbuffer = m_mainFrameBuffer->GetColorBuffer();
    if (!Engine.Inspector().GetVisible()) blitToScreen = true;
#else
    blitToScreen = true;
#endif

    if (blitToScreen)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mainFrameBuffer->GetColorBuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_BACK);
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}
