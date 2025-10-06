#pragma once
#include "core/fileio.hpp"
#include "core/ecs.hpp"

#include <cereal/cereal.hpp>
#include "tools/glm_cereal.hpp"
#include <cereal/archives/json.hpp>

namespace bee
{

struct Particle
{
    entt::entity emitterEntity;
};

struct ParticleEmitter
{
    enum COLOR_MODE
    {
        GRADIENT,
        SINGLE,
        RANDOM
    };

    enum PARTICLE_SHAPE
    {
        Cube,
        Cylinder,
        Sphere
    };

    int m_emissionRate = 1;
    int m_poolSize = 100;

    int m_currentPoolCount = 0;

    PARTICLE_SHAPE m_particleShape = PARTICLE_SHAPE::Cube;

    COLOR_MODE m_colorMode = COLOR_MODE::GRADIENT;
    glm::vec4 m_startColor{glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)};
    glm::vec4 m_endColor{glm::vec4(1.0f)};

    float m_minInitialMagnitude = 0.0f;
    float m_maxInitialMagnitude = 1.0f;

    float m_minParticleMass = 0.1f;
    float m_maxParticleMass = 1.0f;

    float m_gravity = 9.8f;

    float m_minLifeTime = 0.1f;
    float m_maxLifeTime = 1.0f;

    template <class Archive>
    void serialize(Archive& ar);

    void Save(cereal::JSONOutputArchive&, const entt::entity, const std::string&);
    void Load(cereal::JSONInputArchive&, const entt::entity, const std::string&);
};

struct ConeShape
{
    float m_angle = 30.0f;
    float m_radius = 1.0f;

    template <class Archive>
    void serialize(Archive& ar);

    void Save(cereal::JSONOutputArchive&, const entt::entity, const std::string&);
    void Load(cereal::JSONInputArchive&, const entt::entity, const std::string&);
};

struct LifeTime
{
    float m_timeAsPercentage{};
    float m_lifeSpan{};
};

struct Body
{
    float m_mass{};
    glm::vec3 m_velocity{glm::vec3(0.0f)};
};

}  // namespace bee
