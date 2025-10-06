#pragma once
#include <glm/vec3.hpp>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <core/geometry2d.hpp>

namespace bee
{

struct BrushData
{
    glm::vec2 positionUV{glm::vec2(0.0f)};
    float radius{0.0f};
    float intensity{0.0f};
    float targetHeight{0.0f};
};

// Interface for terrain brushes.
class TerrainBrush
{
public:
    TerrainBrush(const std::string& brushName = "None") : m_name(brushName) {}

    virtual ~TerrainBrush() = default;

    void Update(float dt);
    void Render() const;

    virtual void OnPanel() = 0;
    virtual void DispatchComputeShader() = 0;

    const std::string& GetName() const
    {
        return m_name;
    }

    bool isBrushActivated{false};

protected:
    virtual void Input(float dt) = 0;
    virtual void Draw(float dt) = 0;

    std::string m_name;

    glm::vec3 m_position{glm::vec3(0.0f)};
    float m_diameter{25.0f};
    float m_intensity{10.0f};
};

}  // namespace bee
