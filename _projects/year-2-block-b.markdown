---
layout: post
title: "Custom C++ Engine Terrain Editor"
image: /assets/images/sample.jpg
date: 2025-01-24
categories: projects
author: Zhangir Nurmukhambetov
---

## Summary Block

Team - Solo Developer

Roles - Engine & Tools Programmer

Tasks - Real-time Terrain Rendering, Vertex Shader-based Displacement, Custom Brush System (Raise, Lower, Smooth, Plateau, Foliage), Height Map Editing, Object Placement Tools, Camera & Navigation System

Tech - C++, OpenGL, GLSL (Vertex & Fragment Shaders), ImGui, Custom Engine, GLM, Gear Engine (custom framework)

Dates & Times: 
8 weeks (Nov 2024 Jan 2025)


Features:
- Terrain Mesh Deformation via Height Maps
- Vertex Shader with Dynamic Normals & Displacement
- Fully Custom Brush Framework (Raise, Lower, Plateau, Smooth, Foliage)
- Realtime Editing with Visual Feedback
- Normals-based Object Placement (e.g., Trees on Slopes)
- Editable Brush Parameters (Size, Intensity, Falloff)
- Camera Fly Mode with Mouse-Look

## Detail

Terrain Editor Engine is a solo-developed custom terrain editing tool and rendering engine built in C++ using OpenGL. It served as a technical prototype and foundational architecture for the later Fire Ant Engine, with a focus on GPU-based terrain deformation and extensible editor tooling.

### 🛠️ Real-Time Terrain Rendering
At the heart of the engine is a dynamic terrain mesh system rendered using a custom vertex shader. The terrain’s shape is deformed in real time via a height map texture, which is uploaded and modified directly from the editor. Normals are computed on-the-fly in the shader by sampling the height map in adjacent texels, ensuring consistent lighting and shading without precomputed data.

To enable this, the rendering pipeline was extended with:

A Material system supporting height map textures.

Shader modifications for sampling and decoding 32-bit RGBA height data.

Per-pixel normal reconstruction for lighting fidelity.

This allowed precise deformation and editing with full visual feedback inside the engine.

### ✍️ Brush System & Terrain Editing
I designed and implemented a modular brush system to support multiple terrain manipulation modes. Each brush operates on the GPU texture directly, modifying terrain height based on the user-defined area and strength. The brush system includes:

Raise/Lower Brush – Gaussian-weighted deformation in a circle or custom alpha brush shape.

Plateau Brush – Flattens terrain to a sampled or fixed height with optional falloff.

Smooth Brush – Averages out nearby terrain points for natural transitions.

Foliage Brush – Spawns 3D objects (e.g., trees) along terrain, optionally aligning with terrain normals.

Brush size, intensity, and mode can be controlled in real-time via the GUI or keyboard shortcuts ([ and ]). A projected brush circle in the viewport provides precise user feedback.

This brush architecture later became the foundation of the more advanced terrain editor system in Fire Ant.

### 🔍 Height Map Modification & Texture Upload
Instead of rebuilding geometry, the editor modifies the height map texture directly, extracting and re-uploading only the edited region using OpenGL’s glTexSubImage2D. This minimized bandwidth and improved responsiveness during painting.

To optimize for interaction:

Brush regions are computed from ray-plane intersections and transformed into UV space.

Height decoding/encoding from 32-bit RGBA ensures precise elevation control.

Terrain normals for foliage placement are computed using the same sampling logic as in shaders.

### 🧭 Camera & Navigation System
For usability, I implemented a free-fly camera system inspired by 3D modeling tools. The system supports:

WASD movement with vertical controls (Q/E) and speed boost via Shift.

Mouse-look rotation with yaw/pitch and adjustable sensitivity.

Seamless integration into the editor for inspecting terrain edits.

This navigation system is not commonly included in terrain editors and helped establish a usable solo workflow.

## Reflections
This project was a major stepping stone in my technical development. It served as a self-contained prototype that taught me the principles of:

GPU-based editing pipelines using texture feedback.

Shader-driven mesh displacement.

Realtime editor feedback loops and user interaction design.

Object-space math for brushes and normals.

Efficient texture I/O with partial GPU uploads.

It also directly influenced the architecture and systems later adopted in Fire Ant Engine, particularly in the terrain editor, brush system, and foliage placement tools.