#pragma once

#include "terrain/terrain_brush.hpp"

namespace bee
{

class SmoothingBrush : public TerrainBrush
{
public:
    SmoothingBrush() : TerrainBrush("Smoothing Brush") {}

    void Draw(float dt) override;

    void OnPanel() override;

    void Input(float) override;

    void DispatchComputeShader() override {}

private:
    [[maybe_unused]] bool isRaising = true;
    [[maybe_unused]] bool isAverageTerrainOn = false;
};

}  // namespace bee
