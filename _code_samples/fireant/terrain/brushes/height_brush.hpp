#pragma once
#include "terrain/terrain_brush.hpp"
#include "rendering/render.hpp"

namespace bee
{

class HeightBrush : public TerrainBrush
{
public:
    HeightBrush();

    void Draw(float dt) override;

    void DispatchComputeShader() override;

    void OnPanel() override;

    void Input(float) override;

private:
    void DispatchCircleComputeShader();
    void DispatchAlphaComputeShader();

    bool m_isRaising = true;
    bool m_isCircle = true;
    std::shared_ptr<Shader> m_alphaComputeShader = nullptr;
    std::shared_ptr<Shader> m_circleComputeShader = nullptr;

    std::string m_brushAlphaTexturePath{""};
};

}  // namespace bee
