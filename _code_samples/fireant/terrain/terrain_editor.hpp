#pragma once
#include "core/ecs.hpp"
#include "tools/inspectable.hpp"
#include <imgui/IconsFontAwesome.h>
#include "terrain/terrain_brush.hpp"

#ifdef BEE_INSPECTOR
#include "flow_field/flow_field_editor.hpp"
#include "ai/unit/unit_editor.hpp"
#endif

namespace bee
{

class TerrainEditor : public System, IPanel, IEntityInspector
{
public:
    TerrainEditor();
    ~TerrainEditor() override = default;

#ifdef BEE_INSPECTOR
    void Update(float) override;
    void Render() override;

    void DispatchBrushComputeShaders();

#endif

#ifdef BEE_INSPECTOR
    void OnEntity(entt::entity entity) override;
    void OnPanel() override;

    std::string GetName() const override
    {
        return Title;
    }

    std::string GetIcon() const override
    {
        return ICON_FA_GLOBE;
    }
#endif

private:
#ifdef BEE_INSPECTOR
    FlowFieldEditor m_flowFieldEditor{};
    ai::UnitEditor m_unitEditor{};

    std::vector<std::unique_ptr<TerrainBrush>> m_brushes;

    int m_currentBrushIndex{0};

    bool m_renderBrushDebug{true};
#endif
};

}  // namespace bee
