#include "particles/particle_system.hpp"
#include "particles/particle_components.hpp"

#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/transform.hpp"
#include "tools/inspector.hpp"

#include "tools/log.hpp"
#include "imgui/imgui.h"
#include <tools/tools.hpp>
#include "tools/serialization_cereal.hpp"
#include "tools/xsr_render_tools.hpp"
#include "rendering/model_resource.hpp"
#include <rendering/render_xsr_components.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum/magic_enum.hpp>

#include "rendering/render_xsr.hpp"
#include "tools/xsr_debug_render_tools.hpp"

#include "tools/inspector.hpp"
#include "core/resource_manager.hpp"

using namespace bee;

namespace internal
{
static MeshRendererXSR GetParticleShape(bee::ParticleEmitter::PARTICLE_SHAPE shape)
{
    switch (shape)
    {
        case bee::ParticleEmitter::PARTICLE_SHAPE::Cube:
        {
            auto model = Engine.Resources().Load<ModelResource>(FileIO::Directory::SharedAssets, "models/BoxAndCylinder.gltf");

            return model->CreateMeshRendererFromNode("Cube");
        }
        case bee::ParticleEmitter::PARTICLE_SHAPE::Cylinder:
        {
            auto model = Engine.Resources().Load<ModelResource>(FileIO::Directory::SharedAssets, "models/BoxAndCylinder.gltf");

            return model->CreateMeshRendererFromNode("Cylinder");
        }
        case bee::ParticleEmitter::PARTICLE_SHAPE::Sphere:
        {
            auto model = Engine.Resources().Load<ModelResource>(FileIO::Directory::SharedAssets, "models/Sphere.gltf");

            return model->CreateMeshRendererFromNode("Icosphere");
        }
        default:
        {
            Log::Error("Error: Particle shape invalid. Returning default cube");
            auto model = Engine.Resources().Load<ModelResource>(FileIO::Directory::SharedAssets, "models/BoxAndCylinder.gltf");
            return model->CreateMeshRendererFromNode("Cube");
        }
    }
}
}  // namespace internal

bee::ParticleSystem::ParticleSystem()
{
    Title = "Particle System";

    {
        m_accumulator = 0.0f;
        m_isFixedTimeStep = false;
        m_frameRate = 60.0f;
        m_fixedDt = 1.0f / m_frameRate;
    }
}

bee::ParticleSystem::~ParticleSystem() {}

void bee::ParticleSystem::Update(float dt)
{
    if (m_isFixedTimeStep)
    {
        m_accumulator += dt;
        while (m_accumulator >= m_fixedDt)
        {
            UpdateLifeTime(m_fixedDt);
            UpdateSpawnParticles(m_fixedDt);
            UpdateColors(m_fixedDt);
            UpdatePhysics(m_fixedDt);
            m_accumulator -= m_fixedDt;
        }
    }
    else
    {
        UpdateLifeTime(dt);
        UpdateSpawnParticles(dt);
        UpdateColors(dt);
        UpdatePhysics(dt);
    }
}

void bee::ParticleSystem::UpdateLifeTime(float dt)
{
    auto& ecs = Engine.ECS();

    for (auto [e, particle, lifetime] : ecs.Registry.view<Particle, LifeTime>().each())
    {
        const float totalLifeSpan = lifetime.m_lifeSpan;
        const float dtAsPercentageOfLifeSpan = dt / totalLifeSpan;

        lifetime.m_timeAsPercentage += dtAsPercentageOfLifeSpan;

        if (lifetime.m_timeAsPercentage > 1.0f)
        {
            if (auto* emitter = ecs.Registry.try_get<ParticleEmitter>(particle.emitterEntity))
            {
                emitter->m_currentPoolCount--;
            }
            ecs.DeleteEntity(e);
            continue;
        }
    }

    for (auto [e, emitter, lifetime] : ecs.Registry.view<ParticleEmitter, LifeTime>().each())
    {
        const float totalLifeSpan = lifetime.m_lifeSpan;
        const float dtAsPercentageOfLifeSpan = dt / totalLifeSpan;

        lifetime.m_timeAsPercentage += dtAsPercentageOfLifeSpan;

        if (lifetime.m_timeAsPercentage > 1.0f)
        {
            ecs.DeleteEntity(e);
            continue;
        }
    }
}

void bee::ParticleSystem::UpdateSpawnParticles(float)
{
    auto& ecs = Engine.ECS();

    for (auto [e, emitter] : ecs.Registry.view<ParticleEmitter>().each())
    {
        if (emitter.m_currentPoolCount < emitter.m_poolSize)
        {
            for (int j = 0; j < emitter.m_emissionRate; j++)
            {
                SpawnParticle(e);
                emitter.m_currentPoolCount++;
            }
        }
    }
}

void bee::ParticleSystem::UpdateColors(float)
{
    auto& ecs = Engine.ECS();

    for (auto [e, particle, model, lifetime] : ecs.Registry.view<Particle, MeshRendererXSR, LifeTime>().each())
    {
        if (auto* emitter = ecs.Registry.try_get<ParticleEmitter>(particle.emitterEntity))
        {
            if (emitter->m_colorMode != ParticleEmitter::COLOR_MODE::GRADIENT) continue;
            model.Tint = lerp(emitter->m_startColor, emitter->m_endColor, lifetime.m_timeAsPercentage);
        }
    }
}

void bee::ParticleSystem::UpdatePhysics(float dt)
{
    auto& ecs = Engine.ECS();

    for (auto [e, particle, transform, body] : ecs.Registry.view<Particle, Transform, Body>().each())
    {
        if (auto* emitter = ecs.Registry.try_get<ParticleEmitter>(particle.emitterEntity))
        {
            body.m_velocity += DOWN * emitter->m_gravity * dt * body.m_mass;
        }
        transform.SetTranslation(transform.GetTranslation() + body.m_velocity);
    }
}

void bee::ParticleSystem::SpawnParticle(entt::entity emitterEntity)
{
    auto& ecs = Engine.ECS();
    auto& emitter = ecs.Registry.get<ParticleEmitter>(emitterEntity);

    auto particleEntity = ecs.CreateEntity();

    auto& particle = ecs.CreateComponent<Particle>(particleEntity);
    {
        particle.emitterEntity = emitterEntity;
    }

    ecs.CreateComponent<HideTransform>(particleEntity);

    auto& transform = ecs.CreateComponent<Transform>(particleEntity);
    {
        auto& emitterTransform = ecs.Registry.get<Transform>(emitterEntity);
        auto& coneShape = ecs.Registry.get<ConeShape>(emitterEntity);

        transform.SetTranslation(RandomPointOnCircle(coneShape.m_radius) + emitterTransform.GetTranslation());

        transform.SetScale(glm::vec3(0.25f));
    }

    auto& lifetime = ecs.CreateComponent<LifeTime>(particleEntity);
    {
        lifetime.m_lifeSpan = GetRandomNumber(emitter.m_minLifeTime, emitter.m_maxLifeTime);
    }

    auto& meshRenderer =
        Engine.ECS().CreateComponent<MeshRendererXSR>(particleEntity, internal::GetParticleShape(emitter.m_particleShape));
    {
        if (emitter.m_colorMode == ParticleEmitter::COLOR_MODE::SINGLE)
        {
            meshRenderer.Tint = emitter.m_startColor;
        }

        if (emitter.m_colorMode == ParticleEmitter::COLOR_MODE::RANDOM)
        {
            meshRenderer.Tint = GetRandomColor();
        }
    }

    auto& body = Engine.ECS().CreateComponent<Body>(particleEntity);
    {
        auto& emitterTransform = ecs.Registry.get<Transform>(emitterEntity);
        auto& coneShape = ecs.Registry.get<ConeShape>(emitterEntity);

        body.m_mass = GetRandomNumber(emitter.m_minParticleMass, emitter.m_maxParticleMass);

        body.m_velocity = RandomDirectionInCone(UP * emitterTransform.GetRotation(), coneShape.m_angle) *
                          GetRandomNumber(emitter.m_minInitialMagnitude, emitter.m_maxInitialMagnitude);
    }
}

void bee::ParticleSystem::CreateEmitter()
{
    auto& ecs = Engine.ECS();

    auto entity = ecs.CreateEntity();
    auto& transform = ecs.CreateComponent<Transform>(entity);
    {
        transform.SetTranslation(glm::vec3(0.0f, 1.0f, 0.0f));
        transform.Name = "ParticleEmitter " + std::to_string(static_cast<uint32_t>(entity));
    }

    auto& particleEmitter = ecs.CreateComponent<ParticleEmitter>(entity);
    auto& coneShape = ecs.CreateComponent<ConeShape>(entity);
    particleEmitter;
    coneShape;
}

void bee::ParticleSystem::Render()
{
    if (!m_debug_render) return;
    auto& ecs = Engine.ECS();

    if (m_debug_render_cones)
    {
        for (auto [e, transform, emitter, coneShape] : ecs.Registry.view<Transform, ParticleEmitter, ConeShape>().each())
        {
            debug_draw_tools::draw_cone(transform.GetTranslation(),
                                        UP * transform.GetRotation(),
                                        coneShape.m_radius,
                                        coneShape.m_angle);
        }
    }

    if (m_debug_render_arrows)
    {
        for (auto [e, transform, particle, body] : ecs.Registry.view<Transform, Particle, Body>().each())
        {
            if (auto* emitter = ecs.Registry.try_get<ParticleEmitter>(particle.emitterEntity))
            {
                float t = (glm::length(body.m_velocity)) / (emitter->m_maxInitialMagnitude);
                debug_draw_tools::draw_arrow(transform.GetTranslation(),
                                             body.m_velocity,
                                             1.0,
                                             lerp(debug_draw_tools::Colors::Green, debug_draw_tools::Colors::Red, t));
            }
            else
            {
                debug_draw_tools::draw_arrow(transform.GetTranslation(), body.m_velocity, 1.0, debug_draw_tools::Colors::Green);
            }
        }
    }
}

#ifdef BEE_INSPECTOR
void bee::ParticleSystem::Inspect()
{
    ImGui::Begin(Title.c_str());
    ImGui::Text("this is a particle system");

    if (ImGui::Button("Create Default Emitter"))
    {
        CreateEmitter();
    }

    if (ImGui::CollapsingHeader("Particle Step Settings"))
    {
        if (ImGui::RadioButton("Fixed Time Step", m_isFixedTimeStep == true))
        {
            m_isFixedTimeStep = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Dynamic Time Step", m_isFixedTimeStep == false))
        {
            m_isFixedTimeStep = false;
        }

        if (m_isFixedTimeStep)
        {
            if (ImGui::InputFloat("Fixed Framerate", &m_frameRate))
            {
                m_fixedDt = 1.0f / m_frameRate;
            }
        }
    }

    if (ImGui::CollapsingHeader("Particle Debug Render"))
    {
        ImGui::Checkbox("Debug Render", &m_debug_render);
        if (m_debug_render)
        {
            ImGui::Checkbox("Draw Cone Shapes", &m_debug_render_cones);
            ImGui::Checkbox("Draw Arrows", &m_debug_render_arrows);
        }
    }
    ImGui::End();
}

void bee::ParticleSystem::Inspect(Entity entity)
{
    if (Engine.ECS().Registry.any_of<ParticleEmitter>(entity))
    {
        InspectParticleEmitter(entity);
    }
    if (Engine.ECS().Registry.any_of<ConeShape>(entity))
    {
        InspectConeShape(entity);
    }
}

void bee::ParticleSystem::InspectParticleEmitter(Entity e)
{
    if (ImGui::CollapsingHeader("Particle Emitter"))
    {
        auto& emitter = Engine.ECS().Registry.get<ParticleEmitter>(e);

        ImGui::Text("Particle Type");
        ImGui::Separator();
        ImGui::Indent();
        {
            ImGui::Text("Please load the particle type from disk.");
        }
        ImGui::Unindent();

        ImGui::Spacing();

        ImGui::Text("Emitter Settings");
        ImGui::Separator();
        ImGui::Indent();
        {
            ImGui::InputInt("Emission Rate", &emitter.m_emissionRate);
            ImGui::InputInt("Pool Size", &emitter.m_poolSize);
            ImGui::InputFloat("Gravity", &emitter.m_gravity);
        }
        ImGui::Unindent();

        ImGui::Spacing();

        ImGui::Text("Particle Settings");
        ImGui::Separator();
        ImGui::Indent();
        {
            ImGui::Text("Mesh");
            {
                if (ImGui::BeginCombo("Select Particle Shape", magic_enum::enum_name(emitter.m_particleShape).data()))
                {
                    for (int n = 0; n < (int)magic_enum::enum_count<bee::ParticleEmitter::PARTICLE_SHAPE>(); n++)
                    {
                        bee::ParticleEmitter::PARTICLE_SHAPE shape = static_cast<bee::ParticleEmitter::PARTICLE_SHAPE>(n);
                        bool is_selected = (emitter.m_particleShape == shape);
                        if (ImGui::Selectable(magic_enum::enum_name(shape).data(), is_selected))
                        {
                            emitter.m_particleShape = shape;
                        }

                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }
            }

            ImGui::Spacing();

            ImGui::Text("Colors");
            {
                if (ImGui::RadioButton("Gradient", emitter.m_colorMode == ParticleEmitter::COLOR_MODE::GRADIENT))
                {
                    emitter.m_colorMode = ParticleEmitter::COLOR_MODE::GRADIENT;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Single", emitter.m_colorMode == ParticleEmitter::COLOR_MODE::SINGLE))
                {
                    emitter.m_colorMode = ParticleEmitter::COLOR_MODE::SINGLE;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Random", emitter.m_colorMode == ParticleEmitter::COLOR_MODE::RANDOM))
                {
                    emitter.m_colorMode = ParticleEmitter::COLOR_MODE::RANDOM;
                }

                ImGui::Spacing();

                switch (emitter.m_colorMode)
                {
                    case ParticleEmitter::COLOR_MODE::GRADIENT:
                    {
                        ImGui::ColorEdit4("Start Color", glm::value_ptr(emitter.m_startColor));
                        ImGui::ColorEdit4("End Color", glm::value_ptr(emitter.m_endColor));
                        break;
                    }
                    case ParticleEmitter::COLOR_MODE::SINGLE:
                    {
                        ImGui::ColorEdit4("Color", glm::value_ptr(emitter.m_startColor));
                        break;
                    }
                    case ParticleEmitter::COLOR_MODE::RANDOM:
                    {
                        ImGui::Text("Colors are randomly generated");
                        break;
                    }
                }
            }

            ImGui::Spacing();

            ImGui::Text("Initial Magnitude");
            {
                ImGui::InputFloat("Min##Magnitude", &emitter.m_minInitialMagnitude);
                ImGui::InputFloat("Max##Magnitude", &emitter.m_maxInitialMagnitude);
            }

            ImGui::Spacing();

            ImGui::Text("Mass");
            {
                ImGui::InputFloat("Min##Mass", &emitter.m_minParticleMass);
                ImGui::InputFloat("Max##Mass", &emitter.m_maxParticleMass);
            }

            ImGui::Spacing();

            ImGui::Text("Life Time");
            {
                ImGui::InputFloat("Min##Lifetime", &emitter.m_minLifeTime);
                ImGui::InputFloat("Max##Lifetime", &emitter.m_maxLifeTime);
            }
        }
        ImGui::Unindent();
    }
}

void bee::ParticleSystem::InspectConeShape(Entity e)
{
    if (ImGui::CollapsingHeader("Cone Shape"))
    {
        auto& coneShape = Engine.ECS().Registry.get<ConeShape>(e);

        ImGui::InputFloat("Angle", &coneShape.m_angle);
        ImGui::InputFloat("Radius", &coneShape.m_radius);
    }
}

#endif
