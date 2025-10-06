#include "particles/particle_components.hpp"

void bee::ParticleEmitter::Save(cereal::JSONOutputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("ParticleEmitter_" + number, *this));
}

void bee::ParticleEmitter::Load(cereal::JSONInputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("ParticleEmitter_" + number, *this));
}

template <class Archive>
void bee::ParticleEmitter::serialize(Archive& ar)
{
    ar(cereal::make_nvp("Emission Rate", m_emissionRate));
    ar(cereal::make_nvp("Pool Size", m_poolSize));
    ar(cereal::make_nvp("Particle Shape", m_particleShape));
    ar(cereal::make_nvp("Color Mode", m_colorMode));
    ar(cereal::make_nvp("Start Color", m_startColor), cereal::make_nvp("End Color", m_endColor));
    ar(cereal::make_nvp("Min Initial Magnitude", m_minInitialMagnitude),
       cereal::make_nvp("Max Initial Magnitude", m_maxInitialMagnitude));
    ar(cereal::make_nvp("Min Particle Mass", m_minParticleMass), cereal::make_nvp("Max Particle Mass", m_maxParticleMass));
    ar(cereal::make_nvp("Gravity", m_gravity));
    ar(cereal::make_nvp("Min Life Time", m_minLifeTime), cereal::make_nvp("Max Life Time", m_maxLifeTime));
}

// Explicit template instantiations for the types you intend to use
template void bee::ParticleEmitter::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive&);
template void bee::ParticleEmitter::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive&);

void bee::ConeShape::Save(cereal::JSONOutputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("ConeShape_" + number, *this));
}

void bee::ConeShape::Load(cereal::JSONInputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("ConeShape_" + number, *this));
}

template <class Archive>
void bee::ConeShape::serialize(Archive& ar)
{
    ar(cereal::make_nvp("Angle", m_angle));
    ar(cereal::make_nvp("Radius", m_radius));
}

// Explicit template instantiations for the types you intend to use
template void bee::ConeShape::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive&);
template void bee::ConeShape::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive&);
