#pragma once
#include "kudzu/ecs.hpp"
#include "tools/macros.hpp"
#include "tools/gradient.hpp"

namespace kudzu {

struct Particle {
    Entity emitter_entity;
    float lifetime = {};
    float lifetime_left_as_percentage {};

    glm::vec3 velocity {};
    float mass {};
};

struct ParticlePoolCreatedTag {};

struct ParticleSystemComponent {
    BEFRIEND_VISITABLE();

    bool is_playing = false;

    // Main settings
    float duration = 1.0f;
    bool looping = true;

    float start_lifetime = 1.0f;
    float start_speed = 0.01f;

    glm::vec3 start_color = glm::vec3(0.0f);

    bool apply_gravity = false;
    float gravity = 9.8f;
    float gravity_modifier = 1.0f;

    // bool play_on_awake = true;
    // bool auto_random_seed = true;

    bool cast_shadows = true;
    bool emissive = false;

    int max_particle_count = 100;
    int current_particle_count = 0;

    // Shape Settings
    bool enable_shape = true;

    enum class EmitterShape { Sphere, Cone };

    EmitterShape shape;

    float radius = 0.2f;
    float angle = 30.0f;

    glm::vec3 shape_translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 shape_scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::quat shape_rotation = glm::identity<glm::quat>();

    // Emission Settings
    bool enable_emission = true;
    int rate_over_time = 50;
    int rate_over_distance = 0;

    struct Burst {
        float emission_time;  // When to emit
        int count;            // How many particles
        bool triggered;
    };
    std::vector<Burst> bursts;

    // Color over Lifetime
    bool enable_color_over_lifetime = false;
    Gradient color_gradient;

    // Size over Lifetime
    bool enable_size_over_lifetime = false;
    glm::vec3 start_size = glm::vec3(1.0f);
    glm::vec3 end_size = glm::vec3(1.0f);

    // Rotation over Lifetime
    bool enable_rotation_over_lifetime = false;
    glm::quat start_rotation = glm::identity<glm::quat>();
    glm::quat end_rotation = glm::identity<glm::quat>();

    //------------------
    float duration_left_as_percentage = 0.0f;
    float accumulator = 0.0f;
    glm::vec3 previous_position = glm::vec3(0.0f);

   private:
    float spawn_interval = 1.0f;

    int spawn_amount = 1;

    float particle_mass = 0.01f;

    glm::vec3 force = glm::vec3(0.0f);
};
}  // namespace kudzu

VISITABLE_STRUCT(kudzu::ParticleSystemComponent::Burst, emission_time, count, triggered);

VISITABLE_STRUCT(
    kudzu::ParticleSystemComponent, duration, looping, start_lifetime, start_speed, start_color, apply_gravity, gravity,
    gravity_modifier, max_particle_count, current_particle_count, enable_shape, shape, radius, angle, shape_translation,
    shape_scale, shape_rotation, enable_emission, rate_over_time, rate_over_distance, bursts, enable_color_over_lifetime,
    color_gradient, enable_size_over_lifetime, start_size, end_size, enable_rotation_over_lifetime, start_rotation,
    end_rotation, cast_shadows, emissive
);
