#pragma once
#include "kudzu/ecs.hpp"
#include "kudzu/engine.hpp"
#include "kudzu/editor/inspectable.hpp"
#include "kudzu/voxel_resource.hpp"
#include "components/particle_system_component.hpp"

namespace kudzu {
class ParticleSystem : public System, public IInspect {
   public:
    ParticleSystem();

    ~ParticleSystem();
#if defined(KUDZU_INSPECTOR)
    void Inspect(float dt) override;
#endif

    void create_particle_pool(int n);
    void destroy_particle_pool();

   protected:
    void update(float dt) override;
    void render() override;
    void start() override;

   private:
    // Emitters
    void update_particle_physics(Particle&, Transform&, ParticleSystemComponent&, float dt);
    void update_particle_rotation(Particle&, Transform&, ParticleSystemComponent&);
    void update_particle_size(Particle&, Transform&, ParticleSystemComponent&);
    void update_particle_colors(Particle&, VoxelRenderer&, ParticleSystemComponent&);
    void update_particle_lifetime(Particle&, float dt);
    void despawn_particles(Entity&, Particle&);

    // Particles
    void spawn_particles_over_time(Entity&, ParticleSystemComponent&, float dt);
    void spawn_particles_over_distance(Entity&, ParticleSystemComponent&, Transform&);
    void spawn_particles_in_bursts(Entity&, ParticleSystemComponent&);
    void update_emitters_duration(ParticleSystemComponent&, float dt);
    void despawn_emitters(Entity&, ParticleSystemComponent&);

    // Base Functions

    Entity create_particle(uint32 emission_hint);
    void destroy_particle(Entity&);

    bool is_pool_created();

    void spawn_one_particle(Entity& emitter_entity);
    void spawn_in_sphere(Entity& emitter_entity, Entity& particle_entity);
    void spawn_in_cone(Entity& emitter_entity, Entity& particle_entity);

    std::shared_ptr<VoxParser> default_1x1x1_voxel;

    // const int PARTICLE_AMOUNT_MAX = 1;

    const int PARTICLE_AMOUNT_MAX = 512;
};
}  // namespace kudzu
