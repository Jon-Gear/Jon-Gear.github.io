#pragma once
#include "xsr/include/xsr.hpp"
#include <glm/glm.hpp>

namespace bee
{
class ModelResource;
class MeshResource;
class TextureResource;

struct Camera
{
    glm::mat4 Projection;
};

struct MeshRendererXSR
{
    std::shared_ptr<bee::MeshResource> Mesh;
    std::shared_ptr<bee::TextureResource> Texture;

    glm::vec4 Tint = glm::vec4(0.0f);
    glm::vec4 Multiplier = glm::vec4(1.0f);

    MeshRendererXSR() {}

    MeshRendererXSR(std::shared_ptr<bee::MeshResource> mesh, std::shared_ptr<bee::TextureResource> texture)
    {
        Mesh = mesh;
        Texture = texture;
    }
};

struct LightXSR
{
    enum class Type
    {
        Point,
        Directional
    };
    LightXSR() = default;
    LightXSR(const glm::vec3& color, float range, Type type) : m_color(color), m_range(range), m_type(type) {}
    glm::vec3 m_color{glm::vec3(1.0f, 1.0f, 1.0f)};
    float m_range = 1.0f;
    Type m_type = Type::Directional;
};

}  // namespace bee
