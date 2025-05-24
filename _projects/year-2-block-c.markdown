---
layout: project
title: "Fire Ant Engine: Custom C++ Strategy Game Engine"
date: 2025-04-11
categories: [projects]
image: /assets/images/sample.jpg
description: "Fire Ant Engine is a continuation of my custom Terrain Editor. The team was joined by seven talented programmers. The project features a prefab system, a terrain editor, flow-fields for AI agent navigation, an in-engine UI editor, optimizations which allowed a support for a large terrain map and many units, and support on Windows and Nintendo Switch."
contributions: "Terrain Editor | Terrain Rendering | Height Editing | Texture Editing | Prop Placement | Scene Serialization | Prefabs"
tools: "C++, OpenGL, GLSL (Compute & Tesselation Shaders), EnTT, Cereal, ImGui"
team_size: "7 Programmers"
platforms: "Windows | Nintendo Switch"
duration: "8 Weeks (Feb 2025 - Apr 2025)"
---

## My Contributions

### Terrain Editor

[Image]

#### Key Features & Challenges

[Image]

### Terrain Rendering

[Image]

#### Key Features & Challenges


### Height Brush

![image1](../../assets/images/Y2C/height_brush_raise.gif)

![image1](../../assets/images/Y2C/height_brush_lower.gif)



#### Key Features & Challenges

### Texture Brush

![image1](../../assets/images/Y2C/texture_brush.gif)



#### Key Features & Challenges


### Prop Brush

[Image]

#### Key Features & Challenges


### Plateu Brush

![image1](../../assets/images/Y2C/plateau_brush.gif)


#### Key Features & Challenges

### Prefab System

[Image]

#### Key Features & Challenges





## Summary Block


Tasks - Terrain Editor, Terrain Rendering Pass, GPU-accelerated Terrain Editing (Height/Texture/Prop/Plateau Brushes), Scene Serialization & Loading System

Features:
- Terrain Editor with Modular Brush Architecture
- GPU-based Height/Texture/Plateau Brushes using Compute Shaders
- Procedural Prop Placement with Normals Support
- Multi-pass OpenGL Terrain Rendering Pipeline (Vertex + Tessellation + Fragment)
- Scene Hierarchy Serialization via Cereal & EnTT Meta System
- Runtime Reflection and Dynamic Component Loading
- In-Editor Asset Drag & Drop, Scene Saving/Loading

## Detail

My role focused on the core engine & tools development and rendering features, with major contributions to terrain editing systems and a runtime scene serialization framework.

### 🔧 Terrain Editor
During pre-production, I architected and implemented a modular terrain editor, allowing users to dynamically modify terrain using a set of tools built on a BaseTerrainBrush interface. This design enabled easy scaling and extension through polymorphism, promoting reuse and separation of concerns across different brush types.

I developed and integrated the following terrain tools:
- Height Brush: Displaces terrain based on radius/intensity; supports both mathematically generated circular weights and alpha image-based displacement. Runs entirely on GPU via compute shaders.
- Texture Brush: Paints base terrain texture; supports custom alphas and tiled pattern application.
- Prop Brush: Procedurally places props along terrain, aligned to terrain normals and loaded from the in-engine asset browser.
- Plateau Brush: Smoothly levels terrain to a selected height using a falloff curve; height sampling enabled via scene click.

The move from CPU to GPU via GLSL compute shaders enabled efficient real-time terrain deformation, solving performance bottlenecks from earlier prototypes. Each brush is shown working in-engine through sprint demo videos.

### 🌄 Terrain Rendering Pass
The engine supports a custom rendering pass for terrain using OpenGL tessellation shaders, enabling dynamic terrain displacement via height maps. I implemented this pipeline and wrote custom vertex, tessellation, and fragment shaders that compute normals and handle encoded height data.

This rendering pass is isolated from the rest of the engine's model pipeline and uses custom materials (TerrainMaterial) and mesh renderers (TerrainMeshRenderer) to allow flexibility in terrain-specific rendering logic.

### 💾 Scene Saving and Loading
I designed and implemented a scene serialization and loading system that allows saving the entire scene's entity/component hierarchy into JSON using the Cereal library. 

Key features:
- Recursive traversal of entity hierarchies, preserving parent-child relationships.
- Serialization of component data (e.g., transforms, models).
- Runtime reflection system using EnTT’s Meta API for generic load/save logic.

This system became a foundational feature for level editing, versioning, and rapid iteration—enabling scene persistence and recoverability for the entire team.

### 🧩 Engineering Maturity & Team Collaboration
Working within a team required strong modular design, clean API structures, and consistent Git-based workflows. I restructured the terrain editor inherited from a solo prototype to be scalable, maintainable, and team-friendly. Clear naming, encapsulation, and extensibility were prioritized throughout.

## Reflections
This project significantly deepened my understanding of modern rendering pipelines, GPU compute workloads, scene architecture, and real-world software engineering practices. 

Key personal milestones:
- Learned and applied compute shaders for the first time.
- Built extensible brush systems and GPU-driven editing workflows.
- Developed a full runtime reflection system using EnTT Meta.
- Learned to work collaboratively within a version-controlled, cross-feature team.

