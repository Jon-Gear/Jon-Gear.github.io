---
layout: project
title: "Tools Programmer @ Zentera: A Voxel-Based Puzzle Platformer Made With A Custom C++ Engine"
role: "Tools Programmer"


version: long
short_url: /projects/year-2-block-d-short
long_url: /projects/year-2-block-d-long
date: 2025-06-27
categories: [featured_projects]
image: assets/images/Y2D/main.gif

description: "Embark on an adventure with Gori, a clever lizard-like creature, in this voxel-based puzzle platformer. Use his versatile tongue to push, pull, swing, and solve intricate dioramic puzzles in charming miniature voxel worlds. Gori wants to impress his beloved partner by bringing beautiful little trinkets from each level."

contributions: "Lead Programmer | Particle System | Release Management | Sound Implementation | QA Process Improvements | Bug Reporting & Fixing"
tools: "C++ | Custom Engine | Angelscript | FMOD"
team_size: "9 Programmers | 3 Designers | 4 Artists"
platforms: "Windows | PlayStation 5"
duration: "8 Weeks (May 2025 - June 2025)"
---

<a class="cta-link" href="https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/zentera" target="_blank" rel="noopener">⚙️ See Code Examples</a>

## 💥 Impact on the Project

The particle system and sounds are one of the most significant parts of the game, they are an ever-present contribution, appearing in every single level of the game, even inside the player itself. They add an amazing final touch of the game, creating detail and immersion for the player to marvel at.

### Player Particles & Sounds

Here is a video of the particles and sounds present on the player in the final project deliverable:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2D/player-particles-and-sounds.mp4" type="video/mp4">
</video>

The music has been turned off to hear the player sounds more easily.

In the video above, you will see that:
- The player leaves behind a trail of particles which change colors depending on the current color of the voxel they stood on.
- The player has footstep sound effects which leave a very squishy and sloppy sound effect, like Squidward's footsteps.
- The player has licking sound effects, specifically when they stick out and pull back the tongue.

### Main Menu Level Particles & Sounds

Here is a video of the particles and sounds present in the main menu level in the final project deliverable:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2D/main-menu-particles-and-sounds.mp4" type="video/mp4">
</video>

In the video above, you will see that:
- Each portal uses a particle system VFX, each portal with different colors
- Hearts emitting from Sora uses a collection of particle systems to form this specific heart shape.
- The main menu level has BGM and ambience noise, allowing for more immersion.

### Onboarding Level Particles & Sounds

Here is a video of the particles and sounds present in the onboarding level in the final project deliverable:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2D/onboarding-level-particles-and-sounds.mp4" type="video/mp4">
</video>

In the video above, you will see that:
- Each of the campfires and torches use a fire particle system VFX. 
- The crate emits sounds upon moving.
- The lever emits sounds upon activation.
- The door emits a sound upon opening and play a large dust particle system VFX.
- The onboarding level has BGM and ambience noise, allowing for more immersion.

### Level 1 Particles & Sounds

Here is a video of the particles and sounds present in the level 1 in the final project deliverable:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2D/level-1-particles-and-sounds.mp4" type="video/mp4">
</video>

In the video above, you will see that:
- A tripwire emits a sound upon snapping.
- Logs emits a sound upon crashing down and breaking the planks.
- Each of the campfires and torches use a fire particle system VFX. 
- Each of the waterfalls use a water foam particle system VFX.
- The crate emits sounds upon moving.
- The lever emits sounds upon activation.
- The button emits sounds upon activation.
- The pressure plates emit sounds upon activation and deactivation.
- The door emits a sound upon opening and play a large dust particle system VFX.
- The metal gates emit a sound upon opening.
- The level 1 has BGM and ambience noise, allowing for more immersion.


### Level 2 Particles & Sounds

Here is a video of the particles and sounds present in the level 2 in the final project deliverable:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2D/level-2-particles-and-sounds.mp4" type="video/mp4">
</video>

In the video above, you will see that:
- Each of the campfires and torches use a fire particle system VFX. 
- Each of the waterfalls use a water foam particle system VFX.
- The crate emits sounds upon moving.
- The pressure plates emit sounds upon activation and deactivation.
- The door emits a sound upon opening and play a large dust particle system VFX.
- The level 2 has BGM and ambience noise, allowing for more immersion.

All of this goes to show that my contributions have an incredibly large presence in the entire game, being present in all levels and even in the player itself.


## 1️⃣ `particle_system_component.hpp` — The Data Model of the Particle System

This file defines the **core ECS component** that represents a particle emitter, as well as the structure of an individual particle.

This header sets up:

* a lightweight particle data struct
* an ECS emitter component containing:
  * emission rules
  * shape rules
  * lifetime effects (color/size/rotation)
  * gravity
  * rendering toggles
  * runtime bookkeeping
* reflection hooks for editor tooling

In this system, particles are not “entities” in the ECS — instead they are lightweight structs stored in a pool, while the emitter itself *is* an entity containing a `ParticleSystemComponent`.

This is a very common design choice because:

* ECS entities are relatively heavy
* particles can be hundreds/thousands
* particles are short-lived and frequently created/destroyed
* you want tight memory layout + fast iteration

---

### 1) The `Particle` struct

```cpp
struct Particle {
    Entity emitter_entity;
    float lifetime = {};
    float lifetime_left_as_percentage {};

    glm::vec3 velocity {};
    float mass {};
};
```

A `Particle` stores only the bare minimum needed for simulation:

#### **Emitter reference**

```cpp
Entity emitter_entity;
```

Each particle knows which emitter created it. This is important because the simulation/rendering may need to read emitter settings (color over lifetime, size over lifetime, etc.).

#### **Lifetime**

```cpp
float lifetime = {};
float lifetime_left_as_percentage {};
```

Instead of only storing remaining time, the system stores a percentage value too. This is extremely useful because almost all “over lifetime” effects work like:

* `t = 0.0` → particle just spawned
* `t = 1.0` → particle is about to die

This makes gradients and interpolation trivial.

#### **Physics state**

```cpp
glm::vec3 velocity {};
float mass {};
```

Velocity drives movement, and mass is likely used for forces like gravity or any external forces.

---

### 2) Tag: `ParticlePoolCreatedTag`

```cpp
struct ParticlePoolCreatedTag {};
```

This is a pure ECS “tag component”.

It exists for a practical reason: the system likely creates a particle pool (vector, GPU buffer, or instance buffer) once per emitter entity, and this tag prevents doing it multiple times.

---

### 3) The emitter component: `ParticleSystemComponent`

```cpp
struct ParticleSystemComponent {
    BEFRIEND_VISITABLE();

    bool is_playing = false;
    ...
};
```

This component is the emitter itself. It holds every setting needed to:

* spawn particles
* simulate them
* control lifetime behavior
* control shape and emission rules
* drive rendering decisions

---

#### 3.1) Playback control

```cpp
bool is_playing = false;
float duration = 1.0f;
bool looping = true;
```

This makes the particle system behave like a “timeline”:

* `duration` is how long the emitter emits particles
* `looping` restarts emission when it reaches the end
* `is_playing` determines whether emission is currently active

This mimics how Unity-style particle systems work.

---

#### 3.2) Spawn defaults

```cpp
float start_lifetime = 1.0f;
float start_speed = 0.01f;
glm::vec3 start_color = glm::vec3(0.0f);
```

These are the base values assigned to new particles.

* `start_lifetime` determines how long each particle lives
* `start_speed` is the initial velocity magnitude
* `start_color` is the initial color (unless overridden by lifetime gradients)

---

#### 3.3) Gravity settings

```cpp
bool apply_gravity = false;
float gravity = 9.8f;
float gravity_modifier = 1.0f;
```

This is a simple gravity model.

Most likely, the acceleration applied per particle is:

```
gravity_accel = gravity * gravity_modifier
```

and then velocity is updated with:

```
v += gravity_accel * dt
```

---

#### 3.4) Rendering-related toggles

```cpp
bool cast_shadows = true;
bool emissive = false;
```

These are not simulation settings — they exist for rendering.

* `cast_shadows` controls whether particles write into shadow maps
* `emissive` probably makes them ignore lighting and “glow”

---

#### 3.5) Pool size + active count

```cpp
int max_particle_count = 100;
int current_particle_count = 0;
```

This strongly suggests your system uses a fixed-size pool:

* `max_particle_count` = capacity
* `current_particle_count` = how many are currently alive

This avoids constant allocations and is much faster than spawning/destroying dynamically.

---

### 4) Shape module (where particles spawn)

```cpp
bool enable_shape = true;

enum class EmitterShape { Sphere, Cone };
EmitterShape shape;

float radius = 0.2f;
float angle = 30.0f;
```

This module controls the spawn distribution.

* Sphere: spawn in or on a sphere of radius `radius`
* Cone: spawn direction is limited by `angle`

The shape also supports full transform controls:

```cpp
glm::vec3 shape_translation = glm::vec3(0.0f);
glm::vec3 shape_scale = glm::vec3(1.0f);
glm::quat shape_rotation = glm::identity<glm::quat>();
```

This is a big deal: it means your shape can be positioned and rotated independently from the emitter entity’s transform.

---

### 5) Emission module (how particles are spawned over time)

```cpp
bool enable_emission = true;
int rate_over_time = 50;
int rate_over_distance = 0;
```

There are two common emission types:

#### Rate over time

Spawn N particles per second.

#### Rate over distance

Spawn based on how far the emitter moved (useful for trails).

This file also defines a burst system:

```cpp
struct Burst {
    float emission_time;
    int count;
    bool triggered;
};
std::vector<Burst> bursts;
```

A burst is:

* emitted at a specific time in the emitter timeline
* emits `count` particles instantly
* uses `triggered` so it only happens once per cycle

---

### 6) Color over lifetime

```cpp
bool enable_color_over_lifetime = false;
Gradient color_gradient;
```

This enables gradient-based color.

Instead of linearly interpolating between 2 colors, a gradient lets you do:

* fade in
* shift hue
* fade out
* multi-stage color transitions

The gradient is likely sampled using the particle’s lifetime percentage:

```
color = gradient.Sample(t);
```

---

### 7) Size over lifetime

```cpp
bool enable_size_over_lifetime = false;
glm::vec3 start_size = glm::vec3(1.0f);
glm::vec3 end_size = glm::vec3(1.0f);
```

This is a simple interpolation model:

```
size = mix(start_size, end_size, t);
```

You store sizes as `vec3`, meaning particles can stretch non-uniformly (good for sparks, smoke streaks, etc.)

---

### 8) Rotation over lifetime

```cpp
bool enable_rotation_over_lifetime = false;
glm::quat start_rotation = glm::identity<glm::quat>();
glm::quat end_rotation = glm::identity<glm::quat>();
```

Rotation uses quaternions, which avoids gimbal lock and gives smooth interpolation.

This likely uses slerp:

```
rot = slerp(start_rotation, end_rotation, t);
```

---

### 9) Runtime state (internal bookkeeping)

These values are not “settings”. They are runtime values used by the system update loop:

```cpp
float duration_left_as_percentage = 0.0f;
float accumulator = 0.0f;
glm::vec3 previous_position = glm::vec3(0.0f);
```

#### `duration_left_as_percentage`

Tracks emitter progress through its timeline (similar to particles).

#### `accumulator`

Classic emission trick.

If you want `rate_over_time = 50`, you can’t always spawn exactly 50 every frame — so you accumulate time until enough has passed to spawn the next particle.

#### `previous_position`

This is almost certainly used for `rate_over_distance`.

To emit based on movement, you need:

* current position
* last frame position
* distance traveled

---

### 10) Private internal cached values

```cpp
private:
    float spawn_interval = 1.0f;
    int spawn_amount = 1;

    float particle_mass = 0.01f;
    glm::vec3 force = glm::vec3(0.0f);
```

These are implementation helpers:

* `spawn_interval` is probably computed from `rate_over_time`
* `spawn_amount` is probably used for bursts or distance-based spawning
* `particle_mass` is the default particle mass
* `force` is a cached force vector applied during simulation

These are private because they’re not meant to be edited in the inspector — they’re derived.

---

### 11) Reflection / Editor support (VISITABLE_STRUCT)

At the bottom:

```cpp
VISITABLE_STRUCT(kudzu::ParticleSystemComponent::Burst, emission_time, count, triggered);
```

and

```cpp
VISITABLE_STRUCT(
    kudzu::ParticleSystemComponent,
    duration, looping, start_lifetime, start_speed, start_color, ...
);
```

This is a reflection system.

The goal is: **the engine can automatically display this component in an editor UI** and serialize/deserialize it.

So instead of manually writing:

* inspector code
* save/load code
* debug UI code

…the macro lists the fields and makes them “visitable”.

This is a clean way to make the particle system editable like a real engine tool.

---

## 2️⃣ `particle_system.hpp/.cpp` — The Particle System Runtime (Spawn, Simulate, Pool)

The previous file (`particle_system_component.hpp`) defined all particle/emitter settings as pure data.

This file is where the behavior lives. It implements the complete particle system:
* ECS-based emitters
* pooled particles (performance-friendly)
* multiple emission types
* timeline duration + looping
* shape spawning with full transform support
* color/size/rotation over lifetime
* gravity
* debug editor visualization
* particles survive emitter deletion


The system is implemented as an ECS `System`:

```cpp
class ParticleSystem : public System, public IInspect {
```

Meaning:

* it runs every frame (`update(dt)`)
* it can also draw editor debug visuals (`render()`)
* it supports an inspector mode (`Inspect(dt)`)

---

### 1) Two loops: Emitters first, then Particles

The update function is structured in two passes:

#### Pass 1 — update all emitters

```cpp
for (auto [e, emitter, transform] : view<ParticleSystemComponent, Transform>().each()) {
    ...
    spawn_particles_over_time(e, emitter, dt);
    spawn_particles_over_distance(e, emitter, transform);
    spawn_particles_in_bursts(e, emitter);
    update_emitters_duration(emitter, dt);
    despawn_emitters(e, emitter);
}
```

#### Pass 2 — update all active particles

```cpp
for (auto [e, particle, transform, voxel_renderer] :
     view<Particle, Transform, VoxelRenderer>().each()) {
    ...
    update_particle_physics(...);
    update_particle_rotation(...);
    update_particle_size(...);
    update_particle_colors(...);
    update_particle_lifetime(...);
    despawn_particles(...);
}
```

This is a good architecture because:

* emitters decide **how many particles should exist**
* particles handle **their own simulation**
* each loop stays clean and purpose-specific

---

### 2) The system uses a particle pool (object pooling)

The first thing `update()` does:

```cpp
if (!is_pool_created()) {
    create_particle_pool(PARTICLE_AMOUNT_MAX);
}
```

This is the performance foundation.

Instead of constantly creating/destroying particle entities, the system creates a fixed pool once, and then reuses particles by enabling/disabling them.

#### Creating the pool

```cpp
for (int i = 0; i < n; ++i) {
    auto entity = default_1x1x1_voxel->instantiate(0);
    add_component<Particle>(entity);
    get_component<Transform>(entity).set_enabled(false);
}
```

Key idea:

* **particles always exist**
* when “dead”, they’re disabled
* when “spawned”, they’re enabled again

This avoids:

* memory allocations
* ECS entity churn
* cache misses
* fragmentation

---

#### Pool creation is tracked using a tag entity

```cpp
bool is_pool_created() {
    return !registry.view<ParticlePoolCreatedTag>().empty();
}
```

So the system knows whether it already initialized.

---

### 3) Emission logic (three emission modes)

Your emitter can spawn particles using **three separate mechanisms**, which can all stack together:

```
1) Rate over time

2) Rate over distance

3) Bursts
```

This is exactly how “real” engine particle systems work.

---

#### 3.1 Rate over time (the accumulator trick)

```cpp
float particles_to_emit = emitter.rate_over_time * dt;
emitter.accumulator += particles_to_emit;

int whole_particles = static_cast<int>(emitter.accumulator);
emitter.accumulator -= whole_particles;
```

This is a *classic trick* used in games.

Why? Because if the emitter is set to 50 particles/sec, and you’re running at 60 FPS:

* 50/60 = 0.833 particles per frame
* you can’t spawn 0.833 particles
* so you accumulate the fractional part

This makes emission frame-rate independent.

Then you clamp to available slots:

```cpp
int available_slots = emitter.max_particle_count - emitter.current_particle_count;
int spawn_count = std::min(whole_particles, available_slots);
```

So each emitter respects its own max particle count.

---

#### 3.2 Rate over distance (movement-based emission)

This is for trails, dust, skid marks, etc.

```cpp
glm::vec3 current_position = transform.GetWorldPosition();
float distance_moved = glm::distance(current_position, emitter.previous_position);
emitter.previous_position = current_position;

float particles_to_emit = emitter.rate_over_distance * distance_moved;
```

This means:

* if the emitter is standing still → spawns nothing
* if it moves fast → spawns more particles

And it reuses the same accumulator logic.

---

#### 3.3 Bursts (timed events)

Bursts are stored as:

```cpp
struct Burst {
    float emission_time;
    int count;
    bool triggered;
};
```

Then evaluated using the emitter timeline:

```cpp
const float burst_time_as_percentage = burst.emission_time / total_duration;

if (!burst.triggered &&
    emitter.duration_left_as_percentage >= burst_time_as_percentage) {
    ...
    burst.triggered = true;
}
```

This means bursts fire **once per cycle**, and then reset if looping.

---

### 4) The emitter timeline (duration + looping)

Emitters track progress as a percentage:

```cpp
emitter.duration_left_as_percentage += (dt / emitter.duration);
```

Then:

#### Non-looping emitter:

* gets destroyed in play mode
* or stops playing in editor mode

```cpp
if (>= 1.0f && !looping) {
    destroy_entity(e);
}
```

#### Looping emitter:

* wraps around
* resets burst triggers

```cpp
else if (>= 1.0f && looping) {
    emitter.duration_left_as_percentage -= 1.0f;
    for (auto& burst : bursts) burst.triggered = false;
}
```

---

### 5) Spawning a particle

The actual spawn happens in one function:

```cpp
void spawn_one_particle(Entity& emitter_entity)
```

This function:

1. finds a free particle in the pool
2. resets its lifetime + links it to the emitter
3. assigns material settings (emissive/shadows)
4. spawns it in a shape (sphere or cone)
5. initializes color/rotation/size

---

#### 5.1 Pulling from the pool (and material hinting)

This part is clever.

Particles are 1x1x1 voxel entities. Their “material” is stored in the voxel.

You attempt to reuse a particle that already matches the needed emissive setting:

```cpp
const uint32 emission_hint = emitter.emissive ? 2 : 0;
auto particle_entity = create_particle(emission_hint);
```

And in `create_particle()`:

```cpp
if (!transform.is_enabled() && emission_value == emission_hint) {
    transform.set_enabled(true);
    return e;
}
```

So if possible:

* you reuse a particle that already has the correct emission value
* you avoid rewriting voxel material data

Only if needed, you update the voxel:

```cpp
if (voxel_renderer.get_voxel(0,0,0).emission != emission_hint) {
    PrincipledVoxel voxel(...);
    voxel.emission = emission_hint;
    voxel_renderer.set_voxel(glm::vec3(0,0,0), voxel);
}
```

This is a micro-optimization, but a good one.

---

#### 5.2 Linking particle to emitter

```cpp
particle.emitter_entity = emitter_entity;
particle.lifetime = emitter.start_lifetime;
particle.lifetime_left_as_percentage = 0.0f;
```

This is the heart of the system.

The particle stores which emitter it belongs to, so during updates it can access settings like:

* gravity
* size/rotation curves
* color gradients

---

#### 5.3 Layer copying

```cpp
layer_particle.set_layer(layer_emitter);
```

This ensures particles render in the same layer as the emitter (very important in engines with render filtering / collision layers).

---

#### 5.4 Spawn position + initial velocity

If shape is enabled, you delegate:

```cpp
switch (emitter.shape) {
    case Sphere: spawn_in_sphere(...); break;
    case Cone: spawn_in_cone(...); break;
}
```

If shape is disabled, you spawn directly at emitter position:

```cpp
particle_transform.SetTranslation(emitter_transform.GetWorldPosition());
particle.velocity = GetRandomDirectionInSphere() * emitter.start_speed;
```

---

### 6) Shape spawning (Sphere and Cone)

Both shapes apply the same idea:

* build a local shape transform
* multiply it by the emitter’s world transform
* place the particle in world space

#### Shared transform logic

```cpp
glm::mat4 shapeTransform =
    translate(shape_translation) *
    toMat4(shape_rotation) *
    scale(shape_scale);
```

Then you do:

```cpp
particleTransform.SetTranslation(
    emitterTransform.World() * shapeTransform * vec4(random_point, 1.0f)
);
```

This is a clean and correct way to support:

* rotated emitters
* offset shapes
* scaled shapes

---

#### Sphere spawning

```cpp
GetRandomPointInSphere(emitter.radius)
particle.velocity = GetRandomDirectionInSphere() * emitter.start_speed;
```

Particles spawn at random positions inside the sphere and shoot in random directions.

---

#### Cone spawning

Position spawns on the base:

```cpp
GetRandomPointInConeBase(emitter.radius)
```

Direction is restricted:

```cpp
GetRandomDirectionInCone(emitter.angle)
```

And rotated into world space:

```cpp
glm::normalize(mat3(emitterTransform.World() * rotationMatrix) * randomConeDir)
```

So:

* the cone points in emitter rotation direction
* particles shoot outward within that cone angle

---

### 7) Particle update logic

Particles are updated every frame in a simple pipeline:

#### Step 1 — Physics

```cpp
particle.velocity += gravity * dt;
transform.SetTranslation(transform.GetTranslation() + particle.velocity);
```

This is basic Euler integration.

#### Step 2 — Over lifetime modules (optional)

```cpp
if (enable_rotation_over_lifetime) ...
if (enable_size_over_lifetime) ...
if (enable_color_over_lifetime) ...
```

#### Step 3 — Lifetime progression

```cpp
particle.lifetime_left_as_percentage += dt / particle.lifetime;
```

#### Step 4 — Despawn if dead

```cpp
if (particle.lifetime_left_as_percentage > 1.0f) {
    destroy_particle(e);
}
```

---

#### Important: Particles still work if emitter disappears

This block is very important:

```cpp
auto* emitter = try_get<ParticleSystemComponent>(particle.emitter_entity);

if (!emitter) {
    transform.SetTranslation(transform.GetTranslation() + particle.velocity);
    update_particle_lifetime(particle, dt);
    despawn_particles(e, particle);
    continue;
}
```

If the emitter entity gets destroyed, particles:

* keep moving
* keep aging
* despawn normally

So particles don’t instantly vanish if the emitter is removed.

That’s a very good quality-of-life detail.

---

### 8) Destroying a particle = disabling it

This is the pool reuse mechanism:

```cpp
void destroy_particle(Entity& particle_entity) {
    registry.get<Transform>(particle_entity).set_enabled(false);
}
```

No entity destruction, no memory churn. It simply becomes “inactive”.

---

### 9) Debug rendering in the editor

The `render()` function draws the emitter shape in the editor:

```cpp
add_sphere(...);
add_cone(...);
```

So you can visually see:

* sphere radius
* cone direction + angle

This makes the particle system *tool-friendly* rather than purely code-driven.

---

### 10) Inspector mode: simulate while paused

This is a neat feature:

```cpp
void Inspect(float dt) {
    if (Engine.is_playing()) return;

    render();
    update(dt);
}
```

Meaning:

* in editor mode (not playing), you can still preview particles
* it calls both debug render + update

This is how you get “live particle preview” in an editor.

---

## 3️⃣ `component_inspector.cpp` — ImGui Editor UI for the Particle System

This file implements the editor inspector UI for `ParticleSystemComponent`.

The particle system itself is driven entirely by the ECS runtime (`ParticleSystem::update()`), but in an engine you need a way to:

* edit emitter settings interactively
* preview the effect without pressing Play
* manage complex nested settings (bursts, gradients, shapes)

That’s exactly what this inspector does.

---

### 1) Editor preview Play/Stop

At the top:

```cpp
if (!Engine.is_playing()) {
    if (comp.is_playing) {
        if (ImGui::Button("Stop")) {
            comp.is_playing = false;
            comp.current_particle_count = 0;
        }
    } else {
        if (ImGui::Button("Play")) {
            comp.is_playing = true;
            comp.current_particle_count = 0;
        }
    }
}
```

This connects directly to the behavior we saw earlier in `ParticleSystem::update()`:

```cpp
#if defined(KUDZU_INSPECTOR)
if (!Engine.is_playing() && !emitter.is_playing) {
    continue;
}
#endif
```

So when the engine is **not running**, emitters normally don’t simulate.

But by toggling `comp.is_playing`, you can preview the particle system in-editor.

#### Why reset `current_particle_count`?

Because you’re using a pool, and the emitter uses `current_particle_count` to enforce `max_particle_count`.

If you stop and restart without resetting, you can get:

* emission stuck at “max particles”
* incorrect clamping behavior

So resetting makes preview reliable.

---

### 2) Organized like a real engine particle inspector

The UI is broken into familiar modules:

* Main Settings
* Voxel Renderer
* Emission
* Shape
* Color over Lifetime
* Size over Lifetime
* Rotation over Lifetime

Each is a collapsible header:

```cpp
if (ImGui::CollapsingHeader("Main Settings")) { ... }
```

This matters because particle systems can have *a lot* of parameters, and a flat list becomes unreadable fast.

---

### 3) Main Settings UI

```cpp
ImGui::InputFloat("Duration", &comp.duration);
ImGui::Checkbox("Looping", &comp.looping);

ImGui::InputFloat("Start Lifetime", &comp.start_lifetime);
ImGui::InputFloat("Start Speed", &comp.start_speed);
```

This edits the emitter timeline and spawn defaults.

Then:

```cpp
ImGui::ColorEdit3("Start Color", glm::value_ptr(comp.start_color));
```

This is the default color used when color-over-lifetime is disabled.

---

#### Gravity is conditional UI

```cpp
ImGui::Checkbox("Apply Gravity", &comp.apply_gravity);
if (comp.apply_gravity) {
    ImGui::InputFloat("Gravity (m/s)", &comp.gravity);
    ImGui::InputFloat("Gravity Modifier", &comp.gravity_modifier);
}
```

This is good UI design: it prevents clutter.

---

##### Max particles is locked while previewing

```cpp
if (!comp.is_playing) {
    ImGui::InputInt("Max Particles", &comp.max_particle_count);
} else {
    ImGui::Text("Max Particles: %d", comp.max_particle_count);
}
```

This is a subtle but important safety decision.

Changing pool constraints while the system is actively emitting can cause:

* incorrect clamping
* mismatch between pool capacity and emitter limits
* “half dead” particles

So you only allow editing it when the preview is stopped.

---

### 4) Voxel renderer section

```cpp
ImGui::Checkbox("Cast Shadows", &comp.cast_shadows);
ImGui::Checkbox("Emissive", &comp.emissive);
```

This ties into the spawn logic:

```cpp
voxel_renderer.set_shadow_casting(emitter.cast_shadows);
const uint32 emission_hint = emitter.emissive ? 2 : 0;
```

Meaning: the inspector directly edits values that affect both visuals and pool selection.

---

### 5) Emission section (with enable checkbox)

This header uses a pattern like Unity’s particle system:

```cpp
if (ImGui::CollapsingHeaderWithCheckbox("Emission", &comp.enable_emission)) { ... }
```

So the module can be toggled on/off without deleting settings.

Inside:

```cpp
ImGui::InputInt("Rate over Time", &comp.rate_over_time);
ImGui::InputInt("Rate over Distance", &comp.rate_over_distance);
```

---

#### Burst editing UI (nested + dynamic)

This part handles the vector of bursts:

```cpp
for (size_t i = 0; i < comp.bursts.size(); ++i) {
    Burst& burst = comp.bursts[i];
    ...
    ImGui::SliderFloat("Time", &burst.emission_time, 0.0f, comp.duration);
    ImGui::InputInt("Count", &burst.count);
}
```

#### The important part: safe removal

```cpp
if (ImGui::Button("Remove Burst")) {
    comp.bursts.erase(comp.bursts.begin() + i);
    break;
}
```

The `break` is necessary because erasing invalidates indices/iterators.

And the `PushID(i)` is necessary because ImGui needs unique IDs for repeated UI elements:

```cpp
ImGui::PushID(static_cast<int>(i));
...
ImGui::PopID();
```

---

#### Adding bursts

```cpp
if (ImGui::Button("Add Burst")) {
    comp.bursts.push_back(Burst {0.0f, 10});
}
```

So the inspector fully supports dynamic burst editing.

---

### 6) Shape section

Again: collapsible header with enable checkbox:

```cpp
if (ImGui::CollapsingHeaderWithCheckbox("Shape", &comp.enable_shape))
```

Then shape selection uses `magic_enum`:

```cpp
if (ImGui::BeginCombo("Shape", magic_enum::enum_name(comp.shape).data())) {
    ...
}
```

This is a nice tool choice because it avoids hardcoding string names for enum values.

---

#### Shape-specific controls

Sphere:

```cpp
ImGui::InputFloat("Radius", &comp.radius);
```

Cone:

```cpp
ImGui::InputFloat("Radius", &comp.radius);
ImGui::InputFloat("Angle", &comp.angle);
```

This mirrors the actual spawning functions:

* `spawn_in_sphere()`
* `spawn_in_cone()`

---

#### Shape transform editing

The shape has its own local transform independent of the emitter transform.

Position:

```cpp
ImGui::DragFloat3("Position", value_ptr(translation), 0.01f);
```

Scale:

```cpp
ImGui::DragFloat3Colored("Scale", scale, 0.01f);
```

Rotation:

```cpp
comp.shape_rotation = glm::quat(glm::radians(degrees));
```

#### Why degrees?

Humans think in degrees, but GLM uses radians. So the UI works in degrees and converts internally.

Also, quaternions aren’t human-editable, so you convert:

* quaternion → euler degrees for display
* degrees → quaternion for storage

---

### 7) Color over lifetime (gradient editor)

```cpp
ImGui::GradientInput("Color", comp.color_gradient);
```

This is the editor side of:

```cpp
voxel_renderer.set_tint(emitter.color_gradient.sample(t));
```

So you get a real “engine-like” gradient UI rather than just start/end colors.

---

### 8) Size over lifetime

```cpp
ImGui::DragFloat3Colored("Start Size", start_size, 0.01f);
ImGui::DragFloat3Colored("End Size", end_size, 0.01f);
```

This maps directly to:

```cpp
transform.SetScale(glm::lerp(start_size, end_size, t));
```

---

### 9) Rotation over lifetime

Same idea as shape rotation:

```cpp
comp.start_rotation = glm::quat(glm::radians(start_degrees));
comp.end_rotation = glm::quat(glm::radians(end_degrees));
```

Then in simulation:

```cpp
transform.SetRotation(glm::lerp(start_rotation, end_rotation, t));
```

---

### 10) The bigger picture: why this file matters

This inspector file doesn’t “make particles work”.

But it makes the system:

* testable
* tweakable
* designer-friendly
* comparable to real engine particle tools

Without this, every effect would require recompiling code or manually editing JSON.

With it, you can tune:

* emission rates
* burst timing
* cone angle
* gradients
* lifetime
* gravity

…in seconds.
