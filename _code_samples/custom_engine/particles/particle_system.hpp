#include "core/ecs.hpp"
#include "core/transform.hpp"

namespace bee
{

class ParticleSystem : public bee::System
{
public:
    ParticleSystem();
    ~ParticleSystem();
    void Update(float) override;
    void Render() override;

#ifdef BEE_INSPECTOR
    void Inspect() override;
    void Inspect(Entity) override;
#endif

private:
    void UpdateLifeTime(float);
    void UpdateSpawnParticles(float);
    void UpdateColors(float);
    void UpdatePhysics(float);

    void SpawnParticle(entt::entity);

    void CreateEmitter();

    bool m_isFixedTimeStep = false;
    float m_accumulator = 0.0f;
    float m_fixedDt = 0.0f;
    float m_frameRate = 60.0f;

    bool m_debug_render = false;
    bool m_debug_render_cones = false;
    bool m_debug_render_arrows = false;

#ifdef BEE_INSPECTOR
    void InspectParticleEmitter(Entity);
    void InspectConeShape(Entity);
#endif
};

}  // namespace bee
