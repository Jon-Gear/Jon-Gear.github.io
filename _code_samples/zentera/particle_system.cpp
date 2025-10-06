#include "kudzu/systems/particle_system.hpp"
#include "components/particle_system_component.hpp"
#include "components/layer_component.hpp"
#include "components/voxel_renderer.hpp"
#include "kudzu/resources.hpp"

#include "kudzu/transform.hpp"
#include <algorithm>
#include <tools/ease.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>  // for glm::lerp

#include "graphics/debug_render.hpp"

kudzu::ParticleSystem::ParticleSystem() : System(Engine.ecs()) {
    default_1x1x1_voxel = Engine.resources().load<VoxParser>(FileIO::Directory::SharedAssets, "voxel/1x1x1.vox");
}

kudzu::ParticleSystem::~ParticleSystem() {}

void kudzu::ParticleSystem::update(float dt) {
    if (!is_pool_created()) {
        create_particle_pool(PARTICLE_AMOUNT_MAX);
    }

    for (auto [e, emitter, transform] : current_ecs.registry.view<ParticleSystemComponent, Transform>().each()) {
#if defined(KUDZU_INSPECTOR)
        if (!Engine.is_playing() && !emitter.is_playing) {
            continue;
        }
#endif
        if (!emitter.enable_emission || !transform.is_enabled()) {
            continue;
        }

        spawn_particles_over_time(e, emitter, dt);
        spawn_particles_over_distance(e, emitter, transform);
        spawn_particles_in_bursts(e, emitter);
        update_emitters_duration(emitter, dt);
        despawn_emitters(e, emitter);
    }

    for (auto [e, particle, transform, voxel_renderer] :
         current_ecs.registry.view<Particle, Transform, VoxelRenderer>().each()) {
        auto* emitter = current_ecs.registry.try_get<ParticleSystemComponent>(particle.emitter_entity);

        if (!transform.is_enabled()) {
            continue;
        }

        if (!emitter) {
            transform.SetTranslation(transform.GetTranslation() + particle.velocity);
            update_particle_lifetime(particle, dt);
            despawn_particles(e, particle);
            continue;
        }

        if (!emitter->enable_emission) {
            transform.SetTranslation(transform.GetTranslation() + particle.velocity);
            update_particle_lifetime(particle, dt);
            despawn_particles(e, particle);
            continue;
        }

        update_particle_physics(particle, transform, *emitter, dt);

        if (emitter->enable_rotation_over_lifetime) update_particle_rotation(particle, transform, *emitter);
        if (emitter->enable_size_over_lifetime) update_particle_size(particle, transform, *emitter);
        if (emitter->enable_color_over_lifetime) update_particle_colors(particle, voxel_renderer, *emitter);

        update_particle_lifetime(particle, dt);
        despawn_particles(e, particle);
    }
}
#if defined(KUDZU_INSPECTOR)
void kudzu::ParticleSystem::Inspect(float dt) {
    if (Engine.is_playing()) {
        return;
    }

    render();
    update(dt);
}
#endif

void kudzu::ParticleSystem::render() {
    for (auto [e, emitter, transform] : current_ecs.registry.view<ParticleSystemComponent, Transform>().each()) {
        if (emitter.enable_shape) {
            switch (emitter.shape) {
                case ParticleSystemComponent::EmitterShape::Sphere:
                    Engine.debug_renderer().add_sphere(
                        DebugCategory::EDITOR, emitter.shape_translation + transform.GetWorldPosition(), emitter.radius,
                        Colors::White
                    );
                    break;
                case ParticleSystemComponent::EmitterShape::Cone:

                    glm::vec3 coneDirection =
                        glm::normalize((emitter.shape_rotation * transform.GetWorldRotation()) * Directions::Up);

                    Engine.debug_renderer().add_cone(
                        DebugCategory::EDITOR, emitter.shape_translation + transform.GetWorldPosition(), emitter.radius,
                        emitter.angle, coneDirection, Colors::White
                    );
                    break;
            }
        }
    }
}

void kudzu::ParticleSystem::start() {
    for (auto [e, emitter, transform] : current_ecs.registry.view<ParticleSystemComponent, Transform>().each()) {
        emitter.is_playing = false;
        emitter.current_particle_count = 0;
    }
}

/// Emitters

void kudzu::ParticleSystem::spawn_particles_over_time(Entity& e, ParticleSystemComponent& emitter, float dt) {
    KUDZU_PROFILE_FUNCTION();

    float particles_to_emit = emitter.rate_over_time * dt;
    emitter.accumulator += particles_to_emit;

    int whole_particles = static_cast<int>(emitter.accumulator);
    emitter.accumulator -= whole_particles;

    int available_slots = emitter.max_particle_count - emitter.current_particle_count;
    int spawn_count = std::min(whole_particles, available_slots);

    for (int j = 0; j < spawn_count; j++) {
        spawn_one_particle(e);
        emitter.current_particle_count = std::clamp(emitter.current_particle_count + 1, 0, emitter.max_particle_count);
    }
}

void kudzu::ParticleSystem::spawn_particles_over_distance(Entity& e, ParticleSystemComponent& emitter, Transform& transform) {
    glm::vec3 current_position = transform.GetWorldPosition();
    glm::vec3 previous_position = emitter.previous_position;

    float distance_moved = glm::distance(current_position, previous_position);
    emitter.previous_position = current_position;

    float particles_to_emit = emitter.rate_over_distance * distance_moved;
    emitter.accumulator += particles_to_emit;

    int whole_particles = static_cast<int>(emitter.accumulator);
    emitter.accumulator -= whole_particles;

    int available_slots = emitter.max_particle_count - emitter.current_particle_count;
    int spawn_count = std::min(whole_particles, available_slots);

    for (int j = 0; j < spawn_count; j++) {
        spawn_one_particle(e);
        emitter.current_particle_count = std::clamp(emitter.current_particle_count + 1, 0, emitter.max_particle_count);
    }
}

void kudzu::ParticleSystem::spawn_particles_in_bursts(Entity& e, ParticleSystemComponent& emitter) {
    const float total_duration = emitter.duration;

    for (auto& burst : emitter.bursts) {
        const float burst_time_as_percentage_of_duration = burst.emission_time / total_duration;

        if (!burst.triggered && emitter.duration_left_as_percentage >= burst_time_as_percentage_of_duration) {
            int available_slots = emitter.max_particle_count - emitter.current_particle_count;
            int spawn_count = std::min(burst.count, available_slots);

            for (int i = 0; i < spawn_count; ++i) {
                spawn_one_particle(e);
                emitter.current_particle_count = std::clamp(emitter.current_particle_count + 1, 0, emitter.max_particle_count);
            }
            burst.triggered = true;
        }
    }
}

void kudzu::ParticleSystem::update_emitters_duration(ParticleSystemComponent& emitter, float dt) {
    const float total_duration = emitter.duration;
    const float dt_as_percentage_of_duration = dt / total_duration;

    emitter.duration_left_as_percentage += dt_as_percentage_of_duration;
}

void kudzu::ParticleSystem::despawn_emitters(Entity& e, ParticleSystemComponent& emitter) {
    if (emitter.duration_left_as_percentage >= 1.0f && !emitter.looping) {
#if defined(KUDZU_INSPECTOR)
        if (Engine.is_playing()) {
            current_ecs.destroy_entity(e);
        } else {
            emitter.is_playing = false;
            emitter.duration_left_as_percentage = 0.0f;
            for (auto& burst : emitter.bursts) {
                burst.triggered = false;
            }
        }
#else
        current_ecs.destroy_entity(e);
#endif

    } else if (emitter.duration_left_as_percentage >= 1.0f && emitter.looping) {
        emitter.duration_left_as_percentage -= 1.0f;
        for (auto& burst : emitter.bursts) {
            burst.triggered = false;
        }
    }
}

/// Particles

void kudzu::ParticleSystem::update_particle_physics(
    Particle& particle, Transform& transform, ParticleSystemComponent& emitter, float dt
) {
    particle.velocity +=
        (emitter.apply_gravity ? glm::vec3(0.0f, -emitter.gravity, 0.0f) * emitter.gravity_modifier : glm::vec3(0.0f)) * dt;

    transform.SetTranslation(transform.GetTranslation() + particle.velocity);
}

void kudzu::ParticleSystem::update_particle_rotation(
    Particle& particle, Transform& transform, ParticleSystemComponent& emitter
) {
    transform.SetRotation(glm::lerp(emitter.start_rotation, emitter.end_rotation, particle.lifetime_left_as_percentage));
}

void kudzu::ParticleSystem::update_particle_size(Particle& particle, Transform& transform, ParticleSystemComponent& emitter) {
    transform.SetScale(glm::lerp(emitter.start_size, emitter.end_size, particle.lifetime_left_as_percentage));
}

void kudzu::ParticleSystem::update_particle_colors(
    Particle& particle, VoxelRenderer& voxel_renderer, ParticleSystemComponent& emitter
) {
    voxel_renderer.set_tint(emitter.color_gradient.sample(particle.lifetime_left_as_percentage));
}

void kudzu::ParticleSystem::update_particle_lifetime(Particle& particle, float dt) {
    const float total_lifetime = particle.lifetime;
    const float dt_as_percentage_of_lifetime = dt / total_lifetime;

    particle.lifetime_left_as_percentage += dt_as_percentage_of_lifetime;
}

void kudzu::ParticleSystem::despawn_particles(Entity& e, Particle& particle) {
    if (particle.lifetime_left_as_percentage > 1.0f) {
        if (auto* emitter = current_ecs.registry.try_get<ParticleSystemComponent>(particle.emitter_entity)) {
            emitter->current_particle_count = std::clamp(emitter->current_particle_count - 1, 0, emitter->max_particle_count);
        }

        destroy_particle(e);
        // current_ecs.destroy_entity(e);
    }
}

/// Base Functions

void kudzu::ParticleSystem::create_particle_pool(int n) {
    auto tag = current_ecs.create_entity();
    current_ecs.add_component<ParticlePoolCreatedTag>(tag);
    current_ecs.add_component<ECS::Hidden>(tag);

    for (int i = 0; i < n; ++i) {
        auto entity = default_1x1x1_voxel->instantiate(0);
        current_ecs.add_component<ECS::Hidden>(entity);
        current_ecs.add_component<Particle>(entity);
        current_ecs.get_component<Transform>(entity).set_enabled(false);
    }
    Log::Info("Particle System: Created particle pool of size {}", n);
}

void kudzu::ParticleSystem::destroy_particle_pool() {
    for (auto [e, particle, transform] : current_ecs.registry.view<Particle, Transform>().each()) {
        if (auto* emitter = current_ecs.registry.try_get<ParticleSystemComponent>(particle.emitter_entity)) {
            emitter->current_particle_count = std::clamp(emitter->current_particle_count - 1, 0, emitter->max_particle_count);
        }
        current_ecs.destroy_entity(e);
    }
}

kudzu::Entity kudzu::ParticleSystem::create_particle(uint32 emission_hint) {
    /* Grab a view of all particles in the scene pool */
    entt::basic_view view = current_ecs.registry.view<Particle, VoxelRenderer, Transform>();

    /* Try to find a particle that has the emission material set the same way we need it */
    for (auto [e, particle, renderer, transform] : view.each()) {
        const uint32 emission_value = renderer.get_voxel(0u, 0u, 0u).emission;
        if (!transform.is_enabled() && (emission_value == emission_hint)) {
            transform.set_enabled(true);
            return e;
        }
    }

    /* Otherwise, just grab any available particle and change its material */
    for (auto [e, particle, _, transform] : view.each()) {
        if (!transform.is_enabled()) {
            transform.set_enabled(true);
            return e;
        }
    }

    return entt::null; /* Return null if no particles were available */
}

void kudzu::ParticleSystem::destroy_particle(Entity& particle_entity) {
    auto& transform = current_ecs.registry.get<Transform>(particle_entity);
    transform.set_enabled(false);
}

bool kudzu::ParticleSystem::is_pool_created() { return !current_ecs.registry.view<ParticlePoolCreatedTag>().empty(); }

void kudzu::ParticleSystem::spawn_one_particle(Entity& emitter_entity) {
    KUDZU_PROFILE_FUNCTION();

    auto& emitter = current_ecs.registry.get<ParticleSystemComponent>(emitter_entity);
    auto& emitter_transform = current_ecs.registry.get<Transform>(emitter_entity);

    // auto particle_entity = default_1x1x1_voxel->instantiate(0);

    const uint32 emission_hint = emitter.emissive ? 2 : 0;
    auto particle_entity = create_particle(emission_hint);

    if (!current_ecs.registry.valid(particle_entity)) return;

    // current_ecs.add_component<ECS::Hidden>(particle_entity);

    // auto& particle = current_ecs.add_component<Particle>(particle_entity);

    auto& particle = current_ecs.get_component<Particle>(particle_entity);
    auto& voxel_renderer = current_ecs.get_component<VoxelRenderer>(particle_entity);
    auto& particle_transform = current_ecs.get_component<Transform>(particle_entity);

    auto& layer_particle = current_ecs.get_component<LayerComponent>(particle_entity);
    auto& layer_emitter = current_ecs.get_component<LayerComponent>(emitter_entity);
    layer_particle.set_layer(layer_emitter);

    particle.emitter_entity = emitter_entity;
    particle.lifetime = emitter.start_lifetime;
    particle.lifetime_left_as_percentage = 0.0f;

    voxel_renderer.set_shadow_casting(emitter.cast_shadows);

    /* Update the particle material, only if it should be different */
    if (voxel_renderer.get_voxel(0u, 0u, 0u).emission != emission_hint) {
        PrincipledVoxel voxel(0b111111u, 0b111111u, 0b111111u); /* White albedo */
        voxel.roughness = voxel.reflectance = voxel.metallic = 0u;
        voxel.emission = emission_hint;
        voxel_renderer.set_voxel(glm::vec3(0, 0, 0), voxel);
    }
    
    // Set spawn position

    if (emitter.enable_shape) {
        switch (emitter.shape) {
            case ParticleSystemComponent::EmitterShape::Sphere:
                spawn_in_sphere(emitter_entity, particle_entity);
                break;
            case ParticleSystemComponent::EmitterShape::Cone:
                spawn_in_cone(emitter_entity, particle_entity);
                break;
        }
    } else {
        particle_transform.SetTranslation(emitter_transform.GetWorldPosition());
        particle.velocity = GetRandomDirectionInSphere() * emitter.start_speed;
    }

    // Color
    if (emitter.enable_color_over_lifetime) {
        voxel_renderer.set_tint(emitter.color_gradient.sample(particle.lifetime_left_as_percentage));
    } else {
        voxel_renderer.set_tint(emitter.start_color);
    }

    // Rotation
    if (emitter.enable_rotation_over_lifetime) {
        particle_transform.SetRotation(
            glm::lerp(emitter.start_rotation, emitter.end_rotation, particle.lifetime_left_as_percentage)
        );

    } else {
        particle_transform.SetRotation(emitter.start_rotation);
    }

    // Size
    if (emitter.enable_size_over_lifetime) {
        particle_transform.SetScale(glm::lerp(emitter.start_size, emitter.end_size, particle.lifetime_left_as_percentage));
    } else {
        particle_transform.SetScale(emitter.start_size);
    }
}

void kudzu::ParticleSystem::spawn_in_sphere(Entity& emitter_entity, Entity& particle_entity) {
    auto& emitter = current_ecs.registry.get<ParticleSystemComponent>(emitter_entity);
    auto& emitterTransform = current_ecs.registry.get<Transform>(emitter_entity);

    auto& particle = current_ecs.registry.get<Particle>(particle_entity);
    auto& particleTransform = current_ecs.registry.get<Transform>(particle_entity);

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), emitter.shape_translation);
    glm::mat4 rotationMatrix = glm::toMat4(emitter.shape_rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), emitter.shape_scale);
    glm::mat4 shapeTransform = translationMatrix * rotationMatrix * scaleMatrix;

    particleTransform.SetTranslation(
        emitterTransform.World() * shapeTransform * glm::vec4(GetRandomPointInSphere(emitter.radius), 1.0f)
    );
    particle.velocity = GetRandomDirectionInSphere() * emitter.start_speed;
}

void kudzu::ParticleSystem::spawn_in_cone(Entity& emitter_entity, Entity& particle_entity) {
    auto& emitter = current_ecs.registry.get<ParticleSystemComponent>(emitter_entity);
    auto& emitterTransform = current_ecs.registry.get<Transform>(emitter_entity);

    auto& particle = current_ecs.registry.get<Particle>(particle_entity);
    auto& particleTransform = current_ecs.registry.get<Transform>(particle_entity);

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), emitter.shape_translation);
    glm::mat4 rotationMatrix = glm::toMat4(emitter.shape_rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), emitter.shape_scale);
    glm::mat4 shapeTransform = translationMatrix * rotationMatrix * scaleMatrix;

    particleTransform.SetTranslation(
        emitterTransform.World() * shapeTransform * glm::vec4(GetRandomPointInConeBase(emitter.radius), 1.0f)
    );

    particle.velocity =
        glm::normalize(glm::mat3(emitterTransform.World() * rotationMatrix) * GetRandomDirectionInCone(emitter.angle)) *
        emitter.start_speed;
}
