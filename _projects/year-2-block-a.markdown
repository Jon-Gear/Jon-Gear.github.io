---
layout: post
title: "Custom C++ Game Engine"
image: /assets/images/sample.jpg
date: 2024-11-01
categories: projects
author: Zhangir Nurmukhambetov
---

## Summary Block

Team - Solo Developer

Roles - Engine Programmer / Tools Programmer

Tasks - ECS Design (EnTT), Particle System, ImGui Editor & Gizmo Tooling, GLTF Runtime Loader, Resource Manager, Hierarchy System, Cross-Platform Engine Abstractions, JSON Serialization, Tile-Based Level Editor

Tech - C++, OpenGL, GLSL, EnTT, ImGui, Cereal, STB, tinygltf, ImGuizmo

Dates & Times: 8 Weeks (Sep 2024 - Nov 2024)

Features:
- ECS Architecture with Hierarchy Support
- Real-Time Particle System with Physics, Gradients, and Pooling
- GLTF Importer (Mesh/Texture/Hierarchy)
- Resource Manager with Runtime Reference Tracking
- ImGui + ImGuizmo Editor with Inspector & Hierarchy Panel
- Fully Serializable Scene Graph via Cereal + EnTT Meta
- Grid-Based Tile Level Editor with Click-to-Select & Importable Assets
- Cross-Platform Input/Audio/File Abstraction Layer

## Detail

This **C++ Game Engine** was my first full-scale engine project, designed from the ground up to support component-based architecture, real-time editing tools, particle systems, asset loading, and engine abstractions. It laid the groundwork for later projects like the **Terrain Editor** and **Fire Ant Engine**, both of which were built upon its architecture.

### ⚙️ Entity Component System (ECS) with Hierarchy Support

Using **EnTT**, I implemented a full ECS model including:

* `Transform` components with parent-child relationships.
* A linked list–style hierarchy with recursive scene traversal.
* Scene graph management for hierarchy-aware rendering and transformation.

This provided the backbone for advanced systems like serialization, inspector traversal, and prefab instancing.

### 💨 Particle System with Visual Tools

A modular **Particle System** was built with:

* `ParticleEmitter` and `Particle` components.
* Configurable emission rates, gradients, masses, shapes (Cube/Sphere/Cylinder), and lifetimes.
* Velocity vectors computed within a configurable cone.
* Physics simulation with gravity and lifetime-based interpolation.

Particles could be inspected and debugged in real time, with tools like:

* Velocity arrows colored by magnitude.
* Cone visualizations for spawn shapes.
* Gradient UI and emitter property editing.

All particles were rendered using a custom `MeshRendererXSR`, integrated with GLTF-loaded assets.

### 🧩 GLTF Runtime Loader & Resource Manager

The engine featured a **GLTF loader** using `tinygltf`, allowing:

* Runtime import of 3D assets.
* Hierarchical scene instancing with transform preservation.
* Material extraction, including textures and mesh data.

A **runtime resource manager** handled:

* Reference counting.
* Load-time deduplication.
* Handle-based access for all `Mesh`, `Texture`, and `Model` resources.

This allowed lightweight memory usage, hot asset reloading, and robust resource tracking.

### 🛠️ ImGui Editor Tools

A fully integrated **ImGui editor** allowed:

* In-engine hierarchy exploration and property editing.
* Component-level editing through ImGui panels.
* `ImGuizmo`-based gizmo manipulation for translation, rotation, and scale.
* Play/Pause/Step simulation control panel for runtime debugging.
* Entity creation and deletion from the UI.

Systems were modular and injected into the Inspector via the ECS registry.

### 📦 Scene Serialization (Cereal + EnTT Meta)

To persist and reload entire scenes, I implemented a **JSON-based serialization system** using `Cereal`. Key features:

* Scene nodes (entities) with hierarchical parent-child saving.
* Component registration via `EnTT Meta`.
* Selective component loading with fail-safe fallbacks.
* Runtime load/save for individual prefabs or entire scenes.

Each component implemented a `Save()` and `Load()` interface that integrated with EnTT’s meta reflection system.

### 🧱 Tile-Based Level Editor

The final module was a **grid-based tile editor**:

* Custom `Grid` and `Tile` components allowed for 2D map generation.
* Tiles could be clicked in the viewport to select and assign imported GLTF assets.
* A debug overlay showed tile placement and selections.
* Fully serializable levels supported drag-and-drop content reuse.

This editor evolved into a foundation for the terrain tools in the Terrain Editor and Fire Ant Engine.

### 🔄 Platform Abstractions (PC & Console-Ready)

Cross-platform abstraction layers were developed for:

* **Input** (mouse, keyboard, gamepad).
* **File I/O** (platform-specific file path mapping).
* **Audio** (FMOD setup with platform PRX loading).
* **Rendering/Device Layer** (GLFW/OpenGL on PC; alternate device file for console).

This made the engine build-ready for multiple target platforms using conditional compilation and macro-based selection.

---

## Reflections

This engine taught me the full spectrum of engine architecture, from ECS design to asset pipelines, editor tooling, and serialization. Some highlights:

* Learned to structure large C++ projects for flexibility and growth.
* Built real-time editing tools that influenced all future projects.
* Explored runtime resource management, GLTF parsing, and scene hierarchy systems.
* Created a reusable foundation for both creative experimentation and serious technical expansion.

This engine wasn’t just a playground—it became a platform to build upon.