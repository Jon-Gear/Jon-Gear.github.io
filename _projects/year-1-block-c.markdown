---
layout: project
title: "Light Pong: Custom C++ CPU-based Raytracing Engine"
date: 2024-04-12
categories: [projects]
image: /assets/images/Y1c/main.gif
description: "A custom C++ CPU-based raytracing engine which was used to create a simple game of Pong. Use a spotlight as a paddle to hit a mirror ball and score goals!"
contributions: "The Raytracing Algorithm | Shadows | Reflections | Point Lights, Spot Lights, and Directional Lights | Recursive (Whitted) raytracing for Mirrors and Dielectrics| Voxel Sub-Objects | Basic Denoising | Texturing | Panini Projection | Post-Processing | Accumulator | Stochastic Lighting | Sky Dome Illumination | Beer's Law"
tools: "C++"
team_size: "Solo"
platforms: "Windows"
duration: "8 Weeks (Mar 2024 - Apr 2024)"
---

### 💡 Light Pong - CPU-Based Raytracing Engine

**Light Pong** is a custom-built CPU-based raytracing engine written entirely in C++. Designed from scratch without any third-party rendering libraries, it simulates realistic lighting and material interactions using Whitted-style recursive raytracing. The engine supports advanced features such as reflections, refractions, volumetric smoke, physically-based lighting, and post-processing effects. It culminates in an interactive two-player "pong-like" game, where lights act as paddles and the environment responds dynamically to realistic lighting physics. This project served as both a technical deep dive into real-time graphics and a practical application of rendering theory in game development.

---

## 🔨 My Contributions

---

### **The Raytracing Algorithm**

I implemented a custom CPU-based Whitted-style recursive raytracer. It supports primary rays, shadow rays, reflection/refraction rays, and stochastic light sampling. Rays recursively interact with the scene to calculate lighting, shadows, and materials.

---

### **Shadows**

Shadows were implemented using shadow rays to determine visibility of light sources from hit points. Key challenges included fixing "shadow acne" due to floating-point errors, resolved with an epsilon offset.

---

### **Reflections**

Reflection rays are cast recursively. This feature was fully integrated to simulate perfect mirrors and glossy surfaces. Reflections also consider all light types and environment contribution, including the sky dome.

---

### **Point Lights, Spot Lights, and Directional Lights**

All common light types were implemented from scratch. Each light computes falloff, direction, and angle of incidence. A light manager aggregates light contributions for any given point. Spotlights include cone falloff logic, and directional lights mimic sunlight.

---

### **Recursive (Whitted) Raytracing for Mirrors and Dielectrics**

I added recursive raytracing to support dielectrics and mirror materials. Implementing dielectrics required Fresnel equations and accounting for internal reflection and refraction, which was one of the hardest parts of the engine. Fresnel blending and total internal reflection were handled accurately using Snell’s law and dot product logic.

---

### **Voxel Sub-Objects**

To support a dynamic scene, I implemented support for many voxel-based sub-objects. These were organized using a Bounding Volume Hierarchy (BVH), dramatically improving performance for ray-scene traversal. Creating a system to handle dozens of independent voxel objects was essential for the game's architecture.

---

### **Smoke**

Volumetric rendering was added to simulate smoke. This was combined with Beer’s Law to create realistic absorption as rays pass through the medium. It added depth and visual variety to the scene.

---

### **Basic Denoising**

I implemented bilateral filtering as a post-process to reduce noise in stochastic renders. While it was functional, it had mixed results and was one of the more frustrating features to perfect.

---

### **Texturing**

UV texturing was implemented with support for MIP-mapping to handle under/oversampling artifacts. Normal mapping was also added to simulate fine surface detail without additional geometry.

---

### **Panini Projection**

Panini projection was used as a camera model to reduce perspective distortion in wide-angle views. Implementing it required studying Shadertoy demos and adapting the math to my own renderer, which was both time-consuming and technically rewarding.

---

### **Post-Processing**

A general post-processing pipeline was established, supporting chromatic aberration, vignetting, depth-of-field, and filtering. These features were key to creating a polished look.

---

### **Accumulator**

A frame accumulator collects multiple samples over time to support anti-aliasing, depth-of-field, and other stochastic effects. This greatly improved image stability and realism.

---

### **Stochastic Lighting**

Multiple lights are sampled probabilistically to reduce rendering cost while preserving realism. This included random light sampling for soft shadows and indirect effects.

---

### **Sky Dome Illumination**

HDR environment lighting was added through a sky dome. The dome contributes to both diffuse and specular lighting and can be seen in reflections and refractions. Challenges included implementing UV-based lookup and maintaining lighting consistency.

---

### **Beer’s Law**

I implemented Beer’s Law to simulate light attenuation through transparent materials. This was used for both dielectrics and volumetric effects like smoke. It required calculating absorption based on path length inside the medium.

---

### 🧠 Key Engineering Concepts

* **Whitted-Style Recursive Raytracing** – foundational for handling reflections, refractions, and global illumination logic.
* **Lighting Models** – implemented point, spot, directional, and area lights with accurate attenuation and incidence math.
* **Fresnel & Snell's Law** – used for realistic light behavior in dielectrics, enabling refraction and total internal reflection.
* **Beer’s Law** – simulated absorption through transparent media like glass and smoke.
* **BVH (Bounding Volume Hierarchy)** – spatial partitioning for performance, enabling efficient traversal of complex scenes.
* **Data-Oriented Design (DOD)** – optimized memory access patterns to improve performance over OOP approaches.
* **Stochastic Sampling & Anti-Aliasing** – improved image quality via sub-pixel and light-based randomness.
* **Panini Projection & DOF** – advanced camera effects for natural image presentation.
* **Post-Processing & Denoising** – added final image polish with filters like bilateral and chromatic aberration.
* **Voxel Rendering & Normal Mapping** – achieved visual complexity with minimal geometric cost.

---

### 🎯 Overall Impact

Building this raytracing engine taught me how graphics rendering works from the ground up. I moved beyond abstract theory into solving real problems—debugging light transport, optimizing for performance, and balancing visual quality with computational cost. More than just coding, I learned how to read papers, apply mathematical models, and prioritize features under time constraints. Most importantly, I realized that technical achievement only matters if it supports a playable, engaging experience. The project shifted my focus from just building features to building purpose.

---
