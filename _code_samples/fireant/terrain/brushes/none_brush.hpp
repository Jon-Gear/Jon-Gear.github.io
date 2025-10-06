#pragma once

#include "terrain/terrain_brush.hpp"

namespace bee
{

class NoneBrush : public TerrainBrush
{
public:
    NoneBrush() : TerrainBrush("None") {}

    void Draw(float) override {}

    void OnPanel() override {}

    void Input(float) override {}

    void DispatchComputeShader() override {}
};

}  // namespace bee
