#pragma once
#include "terrain/terrain_brush.hpp"

#include <entt/entity/entity.hpp>

namespace bee
{

class PropBrush : public TerrainBrush
{
public:
    PropBrush() : TerrainBrush("Prop Brush") {}

    void Draw(float) override;

    void OnPanel() override;

    void Input(float) override;

    void DispatchComputeShader() override {}

private:
    std::string m_propPath{};

    entt::entity m_sceneRoot{entt::null};

    int m_propsPerStroke{1};
    bool m_placeAlongNormals{true};

    enum class PayloadType : uint8_t
    {
        Asset,
        Scene,
        None,
    };
    PayloadType m_payloadType{PayloadType::None};
};

}  // namespace bee
