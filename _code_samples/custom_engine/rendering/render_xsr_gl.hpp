#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "core/ecs.hpp"
#include "core/fileio.hpp"
#include "rendering/render_xsr_components.hpp"
#include "platform/opengl/xsr_framebuffer.hpp"

namespace bee
{

class RendererXSR : public System
{
public:
    RendererXSR();
    ~RendererXSR() override;
    void Render() override;

private:
    void CreateFrameBuffers();
    void DeleteFrameBuffers();

    int m_width = -1;
    int m_height = -1;

    std::unique_ptr<FrameBuffer> m_mainFrameBuffer{};
};

}  // namespace bee
