---
layout: project
title: "Tools Programmer @ Zentera: A Voxel-Based Puzzle Platformer Made With A Custom C++ Engine"
role: "Tools Programmer"


version: long
short_url: /projects/year-2-block-d-short
long_url: /projects/year-2-block-d-long
date: 2025-06-27
categories: [featured_projects]
image: /assets/images/Y2D/main.gif

description: "Embark on an adventure with Gori, a clever lizard-like creature, in this voxel-based puzzle platformer. Use his versatile tongue to push, pull, swing, and solve intricate dioramic puzzles in charming miniature voxel worlds. Gori wants to impress his beloved partner by bringing beautiful little trinkets from each level."

contributions: "Lead Programmer | Particle System | Release Management | Sound Implementation | QA Process Improvements | Bug Reporting & Fixing"
tools: "C++ | Custom Engine | Angelscript | FMOD"
team_size: "9 Programmers | 3 Designers | 4 Artists"
platforms: "Windows | PlayStation 5"
duration: "8 Weeks (May 2025 - June 2025)"
---

## 🎉 My Contribution: Particle System & Sound Effects

My individual development work was in line with the allocated project hours.

### 🟠🟡 Relevance towards Requirements

For the developed features to be directly relevant to the project, I have asked the artists what they want to have in the particle system. We have used Unity as a point of reference. 

![image1](../../assets/images/Y2D/asking-what-artists-want-1.png)

![image1](../../assets/images/Y2D/asking-what-artists-want-2.png)

![image1](../../assets/images/Y2D/asking-what-artists-want-3.png)

All of this has allowed us to save time and effort on things that truly matter.

In regards to the sounds, I have approached Simon with a list of sounds which I have collected myself after playing a build of our game.

![image1](../../assets/images/Y2D/consulting-designers-on-sounds-1.png)

![image1](../../assets/images/Y2D/consulting-designers-on-sounds-2.png)

![image1](../../assets/images/Y2D/consulting-designers-on-sounds-3.png)

This way, we knew exactly what sounds were needed in our game, allowing for efficiency.

### 🟢 Direct/Indirect Contribution Towards Other Disciplines (Multi-Disciplinary Development)

The particle system has been a great direct contribution to the artists. 

First, as I have mentioned before, I have asked the artists what they want to have in the particle system. We have used Unity as a point of reference. 

![image1](../../assets/images/Y2D/asking-what-artists-want-1.png)

![image1](../../assets/images/Y2D/asking-what-artists-want-2.png)

![image1](../../assets/images/Y2D/asking-what-artists-want-3.png)

This allowed me to develop features which would be directly relevant to the project, allowing us to save time and effort on things that truly matter.

The particle system has been a great indirect contribution to the programmers by being optimized.

![image1](../../assets/images/Y2D/before-optimization.png)

![image1](../../assets/images/Y2D/correctly-identifying-the-lag-cause.png)

![image1](../../assets/images/Y2D/after-optimization.png)

I have collaborated and agreed with graphics to have 512 particles in the game.

The particle system has been a great indirect contribution to the gameplay programmmers by being integrated in Angelscript. Here is the integration below. I have talked and consulted with gameplay programmers on how to successfully integrate it into the scripting engine.

```cpp
#pragma once

#include <angelscript/angelscript.h>
#include "components/particle_system_component.hpp"
#include "engine.hpp"
#include "ecs.hpp"
#include "tools/log.hpp"
#include "scripting/scripting_engine.hpp"
#include <tools/gradient.hpp>

namespace kudzu {

static Gradient& ParticleSystem_GetGradient(ParticleSystemComponent* self) {
    assert(self);
    return self->color_gradient;
}

static void register_particle_system(asIScriptEngine* engine) {
    int r = engine->RegisterObjectType("ParticleSystemComponent", sizeof(ParticleSystemComponent), asOBJ_REF | asOBJ_NOCOUNT);
    assert(r >= 0);

    r = engine->RegisterObjectMethod(
        "ParticleSystemComponent", "gradient& get_gradient()", asFUNCTION(ParticleSystem_GetGradient), asCALL_CDECL_OBJLAST
    );
    assert(r >= 0);
}

}  // namespace kudzu
```

This addition has allowed the gameplay programmers to edit the color of the particle system for the footsteps so that the dust would be the color of the ground.

In regards to sounds, have been closely working together on FMOD with designers. Simon has been collecting the sounds on the internet and compiling them into the bank where I have been implementing the sound events to each relevant gameplay script. I have consulted frequently other gameplay programmers the best way to implement sounds in their scripts.

Here is an example of a `pressure_plate.as` script below, an example of combination of collaboration between me, designers, and other gameplay programmers:

```
#include "scripts/bakje/interactable.as"

shared class pressure_plate : IKudzu {
    //... other code
    
    [editable] [serialize] 
    Audio::SoundEvent press_sound_event;
    
    [editable] [serialize] 
    Audio::SoundEvent unpress_sound_event;
    
    //... other code

    void enter(GameEntity other, vec3) {
        if (other.get_layer() != Layers::PLAYER) return;
        player_standing = true;
        interact_all();
    }

    void exit(GameEntity other, vec3) {
        if (other.get_layer() != Layers::PLAYER) return;
        player_standing = false;  
        interact_all();
    }
    
    //... other code

    void interact_all(){
        for (int i = 0; i < interact_entities.length(); i++) {
            GameEntity entity = interact_entities[i];

            IKudzu@ script = entity.get_script("interactable");
            interactable@ interactable = cast<interactable>(script);

            interactable.interact(self);
        }

        interact_active = !interact_active;

        if(interact_active) {
            press_sound_event.start();
        } else {
            unpress_sound_event.start();
        }
    }
    //... other code
}
```

In summary, my work have been a great contribution to artists, graphics programmers, gameplay programmers, and designers by helping them with creating particles, being an optimized particle system, allowing the particle system to be accessed in the scripting engine, adding sounds to the relevant gameplay scripts, and working together with designers to get all of the sounds to FMOD and into the engine.

### 🔵 Presence and Significance in The Final Project Deliverable

The particle system and sounds are one of the most significant parts of the game, they are an ever-present contribution, appearing in every single level of the game, even inside the player itself. They add an amazing final touch of the game, creating detail and immersion for the player to marvel at.

#### Player Particles & Sounds

Here is a video of the particles and sounds present on the player in the final project deliverable:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2D/player-particles-and-sounds.mp4" type="video/mp4">
</video>

The music has been turned off to hear the player sounds more easily.

In the video above, you will see that:
- The player leaves behind a trail of particles which change colors depending on the current color of the voxel they stood on.
- The player has footstep sound effects which leave a very squishy and sloppy sound effect, like Squidward's footsteps.
- The player has licking sound effects, specifically when they stick out and pull back the tongue.

#### Main Menu Level Particles & Sounds

Here is a video of the particles and sounds present in the main menu level in the final project deliverable:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2D/main-menu-particles-and-sounds.mp4" type="video/mp4">
</video>

In the video above, you will see that:
- Each portal uses a particle system VFX, each portal with different colors
- Hearts emitting from Sora uses a collection of particle systems to form this specific heart shape.
- The main menu level has BGM and ambience noise, allowing for more immersion.

#### Onboarding Level Particles & Sounds

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

#### Level 1 Particles & Sounds

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


#### Level 2 Particles & Sounds

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