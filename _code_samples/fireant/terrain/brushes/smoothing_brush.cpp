#include "terrain/brushes/smoothing_brush.hpp"

#include <core/engine.hpp>
#include <imgui/imgui.h>
#include <terrain/terrain_tools.hpp>

#include "core/input.hpp"

void bee::SmoothingBrush::Draw(float) {}

void bee::SmoothingBrush::OnPanel()
{
#ifdef BEE_INSPECTOR

    ImGui::InputFloat("Brush Size (Diameter)", &m_diameter);
    ImGui::InputFloat("Brush Intensity", &m_intensity);

    ImGui::Text("This is a smoothing brush.");
#endif
}

void bee::SmoothingBrush::Input(float)
{
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::LeftBracket))
        m_diameter = std::max(m_diameter - 1.0f, 0.0f);
    if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::RightBracket))
        m_diameter += 1.0f;
}
