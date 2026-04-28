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

contributions: "Built Video-to-FBX automation tool | Researched and prototyped full-body tracking | Integrated Bluetooth & OSC pipelines | Evaluated and selected tracking hardware"

tools: "Unity, C#, Python, Android, Bluetooth LE, OSC, VR (Meta Quest), Xsens, HaritoraX"
platforms: "VR (Meta Quest) | Windows | Android"
duration: "10+ weeks"
---

# 💼 Internship at SportsImproVR

A deep dive into VR tooling, automation, and full-body tracking systems for sports training applications.

---

# 👤 Zhangir Before the Project

Before starting this internship, I already had:

- Strong **C++ and low-level programming background**
- Experience with **custom engines and tools development**
- Some exposure to **Unity**, but not deeply in VR or mobile
- A mindset focused on **systems, performance, and engineering depth**

However, I lacked:

- Real-world experience with **VR development**
- Knowledge of **Bluetooth communication and Android builds**
- Understanding of **motion capture pipelines**
- Experience with **full-body tracking systems**

I also underestimated how messy real-world systems are compared to controlled academic projects.

---

# 🔨 My Contributions

---

## 📋 Contribution 1 — Video-to-FBX Automation Tool

![image1](../../assets/images/video-to-fbx-demo.gif)

- **System / Feature:** Unity tool that converts video into motion data using MoveAI API  
- **Technical Decision:** Embedded Python execution inside Unity to automate external pipeline  
- **Impact:** Reduced a multi-step manual workflow into a single-click solution  

---

### 🎯 What was the Goal?

The company workflow was:

1. Upload video to MoveAI  
2. Wait for processing  
3. Download files  
4. Import into Unity  

This was slow, manual, and error-prone.

**Goal:**  
Create a **one-click pipeline inside Unity**:
> Input video → Convert → Output FBX automatically

---

### Challenge 1: Automating External APIs Inside Unity

#### ❓ What was the Problem?

MoveAI only provided a **Python-based API**, not a Unity-native solution.

This meant:

- Unity could not directly communicate with the service
- Workflow required leaving the engine entirely
- No integration with existing pipeline

---

#### 🧩 Why Was it Hard?

- Unity does not natively support Python execution
- Needed to bridge **C# ↔ Python ↔ API**
- Required handling async workflows and file outputs
- Debugging cross-language issues is painful

---

#### 💡 Solution. How Did I Approach the Problem?

I broke the problem into layers:

1. **Test Python Execution in Unity**
2. Build a **custom Unity Editor Window**
3. Trigger Python scripts from C#
4. Capture outputs into the Unity project

I first validated the concept with a simple script:

```python
# test.py
with open("output.txt", "w") as f:
    f.write("Hello World")
````

Then triggered it from Unity.

Once validated, I:

* Integrated MoveAI API
* Built a UI for selecting videos
* Automated download + import pipeline

Key decision:

> Keep Python responsible for API logic, Unity for orchestration

---

#### ✅ Problem Solved

* Entire pipeline reduced to **one button**
* No need to leave Unity
* Fully automated workflow

---

#### 📉 Cost / Limitations / Tradeoffs

* Processing still takes ~15 minutes per video
* API costs money per conversion
* Dependency on external service (MoveAI)

---

### 💥 Impact of Contribution

**Before:**

* Manual, multi-step workflow
* Context switching between tools
* High friction

**After:**

* Single-click conversion
* Integrated into production pipeline
* Faster iteration for the team

---

### 📚 What I Learned

* Tooling is about **removing friction**, not adding features
* Cross-language integration is extremely powerful
* Validation via small prototypes is critical

---

### 🔄 Future Improvements

* Add caching for repeated conversions
* Improve error handling and logging
* Integrate progress tracking from API

---

## 📋 Contribution 2 — Parallel Processing & Tool UX

---

### 🎯 What was the Goal?

Improve the tool to:

* Support **multiple videos**
* Run in **background**
* Provide **progress feedback**

---

### Challenge 2: Blocking Editor & Sequential Processing

#### ❓ What was the Problem?

* Unity editor froze during conversion
* Videos processed one-by-one
* No visibility into progress

---

#### 🧩 Why Was it Hard?

* Unity is **single-threaded by default**
* Python parallelism failed
* Needed non-blocking architecture

---

#### 💡 Solution

Instead of forcing Python parallelism, I:

* Used **Unity background tasks**
* Created **independent jobs per video**
* Implemented progress tracking per task

Key insight:

> Parallelism belongs in Unity, not Python

---

#### ✅ Problem Solved

* Multiple videos convert simultaneously
* Editor remains responsive
* Progress visible per video

---

#### 📉 Tradeoffs

* More complex task management
* Requires careful state tracking

---

### 💥 Impact

* Massive improvement in usability
* Scales to real production workflows

---

### 📚 What I Learned

* Correct abstraction layer matters more than brute force
* UX is just as important as technical correctness

---

## 📋 Contribution 3 — Full-Body Tracking Research & Prototyping

---

### 🎯 What was the Goal?

Create a **full-body tracking solution** for VR sports training.

---

### Challenge 3: Xsens Trackers Limitations

#### ❓ What was the Problem?

Xsens trackers:

* Drift over time
* Do not provide reliable positional tracking
* Not designed for real-time VR gameplay

---

#### 🧩 Why Was it Hard?

* Poor documentation for VR use-case
* Bluetooth communication complexity
* Hardware limitations beyond software control

---

#### 💡 Solution

I:

1. Reverse-engineered Bluetooth communication concepts
2. Learned **BLE architecture (Device → Service → Characteristic)**
3. Built Android + Unity prototypes
4. Validated tracker communication

Eventually discovered:

> Xsens is fundamentally unsuitable for this use case 

---

#### ✅ Problem Solved

Identified core limitation early → avoided wasted development time

---

#### 📉 Tradeoffs

* Time spent exploring dead-end hardware
* Required pivot

---

### 💥 Impact

Saved the company from investing further into an unsuitable solution.

---

### 📚 What I Learned

* Sometimes the correct solution is to **stop building**
* Hardware constraints can invalidate software entirely

---

## 📋 Contribution 4 — Hardware Decision: HaritoraX Integration

---

### 🎯 What was the Goal?

Find a **viable full-body tracking solution**

---

### Challenge 4: Choosing the Right Technology

#### ❓ What was the Problem?

Multiple tracking solutions exist:

* IMU-based
* Lighthouse
* Camera-based
* EMF-based

Each has tradeoffs.

---

#### 💡 Solution

I performed a full comparison and recommended:

**HaritoraX 2**

Reasons:

* 3x cheaper than alternatives
* Provides full skeleton via OSC
* Already used in production apps like VRChat 

---

#### ✅ Problem Solved

* Clear direction for the company
* Ready-to-use pipeline

---

### 💥 Impact

Shifted project from **uncertain R&D → viable solution**

---

### 📚 What I Learned

* Engineering includes **decision-making**, not just coding
* Cost vs performance tradeoffs matter

---

## 📋 Contribution 5 — OSC & Networking Pipeline

---

### 🎯 What was the Goal?

Receive tracking data inside Unity.

---

### Challenge 5: OSC Communication Not Working

#### ❓ What was the Problem?

* No data received in Unity
* Silent failure
* No debugging visibility

---

#### 🧩 Why Was it Hard?

* Android networking restrictions
* Unity missing permissions
* Poor error messages

---

#### 💡 Solution

After debugging:

* Identified missing permissions in `AndroidManifest.xml`
* Added:

```xml
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"/>
```

---

#### ✅ Problem Solved

* OSC data successfully received
* Full pipeline unblocked

---

#### 📉 Tradeoffs

* Platform-specific complexity
* Fragile setup

---

### 💥 Impact

Enabled communication between:

* Trackers → VR Manager → Unity

---

### 📚 What I Learned

* Debugging infrastructure matters more than logic
* Platform constraints can silently break systems

---

# 👤 Zhangir After the Project

By the end of this internship:

### ✅ What I Achieved

* Built a **production-ready automation tool**
* Designed and validated a **full-body tracking pipeline**
* Worked across **Unity, Python, Android, Bluetooth, and networking**
* Made **technical decisions with real business impact**

---

### 🚀 Growth as an Engineer

Before:

* Focused on isolated systems

After:

* Think in **pipelines and ecosystems**
* Consider **UX, scalability, and production workflows**
* Comfortable with **unknown domains (VR, Bluetooth, OSC)**

---

### 🧠 Key Takeaways

* The best engineers **remove friction**
* Not all problems should be solved — some should be **reframed**
* Real-world engineering is about **decisions under uncertainty**

---

### 🏁 Final Reflection

This internship represents a shift from:

> “I can build systems”
> →
> “I can build the *right* systems”

It demonstrates:

* Ownership
* Technical depth
* Decision-making ability

And most importantly:

> The ability to turn uncertainty into working solutions.
