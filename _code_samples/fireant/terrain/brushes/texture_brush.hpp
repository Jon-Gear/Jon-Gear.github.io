#pragma once
#include "terrain/terrain_brush.hpp"
#include "rendering/render.hpp"

namespace bee
{

class TextureBrush : public TerrainBrush
{
public:
    TextureBrush();
    void Draw(float dt) override;

    void OnPanel() override;

    void Input(float) override;

    void DispatchComputeShader() override;

private:
    void DispatchCircleComputeShader();
    void DispatchAlphaComputeShader();

    bool m_isCircle = true;
    std::shared_ptr<Shader> m_alphaComputeShader = nullptr;
    std::shared_ptr<Shader> m_circleComputeShader = nullptr;

    std::string m_brushTerrainTexturePath{""};
    std::string m_brushAlphaTexturePath{""};
};

}  // namespace bee
