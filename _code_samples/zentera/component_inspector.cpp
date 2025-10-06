#include "component_inspector.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/md_icons.hpp>
#include <imgui/imgui_helper.h>
#include "tools/imgui_tools.hpp"

#include <glm/glm.hpp>

#include "tools/diff_util.hpp"
#include "tools/serialization.hpp"
#include "tools/tools.hpp"
#include "tools/file_dialog.hpp"
#include "systems/input.hpp"
#include "resources.hpp"

#include "engine.hpp"
#include "ecs.hpp"
#include "kudzu/systems/particle_system.hpp"

#include "components/particle_system_component.hpp"
#include "editor/inspector.hpp"
#include "editor_colors.hpp"


// ============= Particle System Component Inspector =============

void kudzu::ParticleSystemComponentInspector::on_component(entt::entity entity) {
    ParticleSystemComponent& comp = Engine.ecs().get_component<ParticleSystemComponent>(entity);

    if (!Engine.is_playing()) {
        if (comp.is_playing) {
            if (ImGui::Button("Stop")) {
                comp.is_playing = false;
                comp.current_particle_count = 0;  // Reset particle count when stopping
            }
        } else {
            if (ImGui::Button("Play")) {
                comp.is_playing = true;
                comp.current_particle_count = 0;  // Reset particle count when starting
            }
        }
    } else {
        ImGui::Text("Engine is playing. Cannot preview particle");
    }

    if (ImGui::CollapsingHeader("Main Settings")) {
        ImGui::Indent();
        ImGui::InputFloat("Duration", &comp.duration);
        ImGui::Checkbox("Looping", &comp.looping);

        ImGui::InputFloat("Start Lifetime", &comp.start_lifetime);
        ImGui::InputFloat("Start Speed", &comp.start_speed);

        ImGui::InputFloat3("Start Size", glm::value_ptr(comp.start_size));
        ImGui::InputFloat3("Start Rotation", glm::value_ptr(comp.start_rotation));
        ImGui::ColorEdit3("Start Color", glm::value_ptr(comp.start_color));

        ImGui::Checkbox("Apply Gravity", &comp.apply_gravity);
        if (comp.apply_gravity) {
            ImGui::InputFloat("Gravity (m/s)", &comp.gravity);
            ImGui::InputFloat("Gravity Modifier", &comp.gravity_modifier);
        }

        if (!comp.is_playing) {
            ImGui::InputInt("Max Particles", &comp.max_particle_count);
        } else {
            ImGui::Text("Max Particles: %d", comp.max_particle_count);
        }

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Voxel Renderer")) {
        ImGui::Indent();
        ImGui::Checkbox("Cast Shadows", &comp.cast_shadows);
        ImGui::Checkbox("Emissive", &comp.emissive);
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeaderWithCheckbox("Emission", &comp.enable_emission)) {
        ImGui::Indent();
        ImGui::InputInt("Rate over Time", &comp.rate_over_time);
        ImGui::InputInt("Rate over Distance", &comp.rate_over_distance);

        if (ImGui::TreeNode("Bursts")) {
            // List existing bursts
            for (size_t i = 0; i < comp.bursts.size(); ++i) {
                ParticleSystemComponent::Burst& burst = comp.bursts[i];
                ImGui::PushID(static_cast<int>(i));

                ImGui::Text("Burst %zu", i);
                ImGui::SliderFloat("Time", &burst.emission_time, 0.0f, comp.duration, "%.2f");
                ImGui::InputInt("Count", &burst.count);

                if (ImGui::Button("Remove Burst")) {
                    comp.bursts.erase(comp.bursts.begin() + i);
                    ImGui::PopID();
                    break;  // break to avoid iterator invalidation
                }

                ImGui::Separator();
                ImGui::PopID();
            }

            // Add new burst
            if (ImGui::Button("Add Burst")) {
                comp.bursts.push_back(ParticleSystemComponent::Burst {0.0f, 10});
            }

            ImGui::TreePop();
        }

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeaderWithCheckbox("Shape", &comp.enable_shape)) {
        ImGui::Indent();

        if (ImGui::BeginCombo("Shape", magic_enum::enum_name(comp.shape).data())) {
            for (int n = 0; n < (int)magic_enum::enum_count<ParticleSystemComponent::EmitterShape>(); n++) {
                ParticleSystemComponent::EmitterShape shape = static_cast<ParticleSystemComponent::EmitterShape>(n);
                bool is_selected = (comp.shape == shape);
                if (ImGui::Selectable(magic_enum::enum_name(shape).data(), is_selected)) {
                    comp.shape = shape;
                }

                if (is_selected) ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (comp.shape == ParticleSystemComponent::EmitterShape::Sphere) {
            ImGui::InputFloat("Radius", &comp.radius);
        }

        if (comp.shape == ParticleSystemComponent::EmitterShape::Cone) {
            ImGui::InputFloat("Radius", &comp.radius);
            ImGui::InputFloat("Angle", &comp.angle);
        }

        ImGui::Separator();

        glm::vec3 translation(comp.shape_translation);
        if (ImGui::DragFloat3("Position", glm::value_ptr(translation), 0.01f)) {
            comp.shape_translation = translation;
        }

        glm::vec3 scale(comp.shape_scale);
        if (ImGui::DragFloat3Colored("Scale", scale, 0.01f)) {
            comp.shape_scale = scale;
        }

        static glm::vec3 degrees;
        if (ImGui::DragFloat3Colored("Rotation (Degrees)", degrees, 0.5f)) {
            comp.shape_rotation = glm::quat(glm::radians(degrees));
        } else {
            degrees = glm::degrees(glm::eulerAngles(comp.shape_rotation));
        }

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeaderWithCheckbox("Color over Lifetime", &comp.enable_color_over_lifetime)) {
        ImGui::Indent();
        ImGui::GradientInput("Color", comp.color_gradient);
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeaderWithCheckbox("Size over Lifetime", &comp.enable_size_over_lifetime)) {
        ImGui::Indent();

        glm::vec3 start_size(comp.start_size);
        if (ImGui::DragFloat3Colored("Start Size", start_size, 0.01f)) {
            comp.start_size = start_size;
        }

        glm::vec3 end_size(comp.end_size);
        if (ImGui::DragFloat3Colored("End Size", end_size, 0.01f)) {
            comp.end_size = end_size;
        }

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeaderWithCheckbox("Rotation over Lifetime", &comp.enable_rotation_over_lifetime)) {
        ImGui::Indent();

        static glm::vec3 start_degrees;
        if (ImGui::DragFloat3Colored("Start Rotation (Degrees)", start_degrees, 0.5f)) {
            comp.start_rotation = glm::quat(glm::radians(start_degrees));
        } else {
            start_degrees = glm::degrees(glm::eulerAngles(comp.start_rotation));
        }

        static glm::vec3 end_degrees;
        if (ImGui::DragFloat3Colored("End Rotation (Degrees)", end_degrees, 0.5f)) {
            comp.end_rotation = glm::quat(glm::radians(end_degrees));
        } else {
            end_degrees = glm::degrees(glm::eulerAngles(comp.end_rotation));
        }

        ImGui::Unindent();
    }
}

