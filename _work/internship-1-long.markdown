---
layout: work
title: "Internship at SportsImproVR"
role: "Tools & VR Programmer"

version: long
short_url: /work/internship-1-short
long_url: /work/internship-1-long

start_date: 2025-11-10
end_date: 2026-04-10

categories: [work]
image: /assets/images/internship-1/fbt.gif
description: "A VR tools and systems internship focused on automating motion capture workflows and building a full-body tracking solution for sports training in Unity."

contributions: "Video-to-FBX automation tool | Full-body tracking system research & implementation"

tools: "Unity, C#, Python, Android, Bluetooth LE, OSC, VR (Meta Quest), Xsens, HaritoraX"
platforms: "VR (Meta Quest) | Windows | Android"
duration: "10+ weeks"
---


# 💼 Internship at SportsImproVR

This internship was not just about building features — it was about solving messy, real-world problems where the solution was often unclear.

I worked on two major areas:

- 🎥 Automating motion capture workflows  
- 🕺 Building a full-body tracking system for VR training  

Both required deep technical exploration, experimentation, and decision-making under uncertainty.

---

# 👤 Zhangir Before the Project

Before starting:

- Strong **systems & low-level programming mindset**
- Comfortable building tools and engine-level features
- Limited experience with:
  - VR development
  - Android builds
  - Bluetooth communication
  - Motion capture pipelines

Most importantly:
> I was used to problems where the solution *exists*.

This internship forced me into problems where:
> The solution might not even be possible.

---

# 🔨 My Contributions

---

# 🎥 Contribution 1: Video-to-FBX Converter

## 🎯 The Goal

The company had a painful workflow:

1. Upload video to MoveAI  
2. Wait  
3. Download files  
4. Import into Unity  
5. Clean up unnecessary data  

This process was:
- Slow ⏳
- Manual 🖱️
- Error-prone ⚠️

### The Vision

> “What if this entire pipeline was just one button inside Unity?”

---

## ⚔️ Challenge 1: Bridging Unity and Python

### ❓ Problem

MoveAI only provided a **Python API**.

Unity uses C#.

There was no direct way to connect them.

---

### 🧩 Why it was Hard

- Cross-language execution (C# → Python)
- File handling across environments
- Debugging becomes harder across boundaries

---

### 💡 Solution

I started small:

- Created a Python script that writes a file
- Triggered it from Unity

Then scaled up:

- Integrated MoveAI API
- Passed video paths as input
- Returned FBX data into Unity

---

### ✅ Result

A working pipeline:
> Unity → Python → MoveAI → Unity

---

## ⚔️ Challenge 2: Editor Freezing

### ❓ Problem

Each conversion took ~15 minutes.

Unity completely froze during this time.

---

### 🧩 Why it was Hard

- Unity runs on a **main thread**
- Blocking operations freeze the editor

---

### 💡 Solution

- Moved execution into **background tasks**
- Kept Unity responsive during processing

---

### ✅ Result

- Editor stays usable
- Workflow no longer blocked

---

## ⚔️ Challenge 3: Sequential Processing Bottleneck

### ❓ Problem

Videos processed one-by-one.

10 videos = hours of waiting.

---

### 🧩 Why it was Hard

- Python parallelism unreliable
- Needed per-task tracking

---

### 💡 Solution

Instead of forcing Python:

> I moved parallelism into Unity.

- Each video = separate task
- Independent progress tracking

---

### ✅ Result

- Parallel processing ⚡
- Massive time savings

---

## ⚔️ Challenge 4: UX Was Terrible

### ❓ Problem

- Users had to manually copy files
- No feedback
- No validation

---

### 💡 Solution

I redesigned the tool:

- 📂 Native file browser
- ⚠️ Error/warning system
- 📊 Progress bars per video
- 🧭 Clear UI layout

---

### ✅ Result

From:
> “Hacky internal tool”

To:
> “Production-ready pipeline”

---

## 💥 Final Impact

Before:
- Manual, slow, frustrating workflow

After:
- One-click automation
- Scalable pipeline
- Developer-friendly tool

---

# 🕺 Contribution 2: Full-Body Tracking System

## 🎯 The Goal

Build a system that tracks the **entire human body in VR**.

This is critical for:
- Sports training 🏋️
- Motion analysis 📈

---

## ⚔️ Challenge 1: The Tracking Landscape is a Mess

### ❓ Problem

There is no single standard solution.

Options included:
- IMU trackers
- Lighthouse systems
- Camera tracking
- EMF systems

---

### 💡 Solution

I performed a full breakdown of:

- Accuracy
- Latency
- Cost
- Reliability

---

### ✅ Result

Identified **IMU-based tracking** as best balance.

---

## ⚔️ Challenge 2: Xsens Was a Dead End

### ❓ Problem

Trackers drifted over time.

Even when stationary:
> They still moved.

---

### 🧩 Why it was Hard

- Not a coding bug
- Fundamental hardware limitation

---

### 💡 Solution

- Contacted Xsens support
- Validated behavior experimentally

---

### 🚨 Realization

> This solution will NEVER work for our use case.

---

### ✅ Result

Pivoted away early → saved huge time.

---

## ⚔️ Challenge 3: Bluetooth is a Nightmare

### ❓ Problem

Needed to communicate with trackers.

---

### 🧩 Why it was Hard

Bluetooth LE structure:

Device → Service → Characteristic

- Poor documentation
- Hard to debug
- Platform-specific behavior

---

### 💡 Solution

- Learned BLE architecture deeply
- Built Android test apps
- Scanned and connected to devices

---

### ✅ Result

Successfully detected and connected trackers.

---

## ⚔️ Challenge 4: “Connected” ≠ “Working”

### ❓ Problem

Could connect to trackers…

…but received **no usable data**.

---

### 💡 Solution

- Studied BLE specs
- Identified correct characteristics
- Learned how data is structured

---

### ✅ Result

Understood full communication pipeline.

---

## ⚔️ Challenge 5: Choosing the Right Hardware

### ❓ Problem

Xsens unusable → needed alternative

---

### 💡 Solution

Researched and proposed:

**HaritoraX**

Why:
- 💸 Cheaper
- 🧠 Provides full skeleton
- 🔌 Outputs via OSC

---

### ✅ Result

Company adopted new solution.

---

## ⚔️ Challenge 6: OSC Pipeline Not Working

### ❓ Problem

No data received in Unity.

Silent failure.

---

### 🧩 Why it was Hard

- No error messages
- Platform restrictions (Android)

---

### 💡 Solution

After deep debugging:

- Found missing Android permissions
- Modified `AndroidManifest.xml`

---

### ✅ Result

OSC communication works 🎉

---

## 💥 Final Impact

Before:
- No working tracking solution

After:
- Clear pipeline:
  - Trackers → VR Manager → OSC → Unity

---

# 👤 Zhangir After the Project

## 🚀 Growth

Before:
- Strong programmer

After:
- Real-world problem solver

---

## 🧠 What Changed

- I now think in **systems, not features**
- I question whether a problem is worth solving
- I make decisions based on **constraints, not assumptions**

---

## 🔑 Key Takeaways

- The hardest problems are **uncertain ones**
- Engineering = **decision-making under ambiguity**
- The best solutions often come from **changing direction**

---

## 🏁 Final Reflection

This internship proved that I can:

- Build real production tools 🛠️  
- Navigate unknown technical domains 🌍  
- Make impactful engineering decisions 🎯  

Most importantly:

> I can turn chaos into working systems.
