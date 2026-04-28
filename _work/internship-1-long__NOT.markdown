---
layout: work
title: "Internship at SportsImproVR"
role: "Tools Programmer"
start_date: 2025-11-10
end_date: 2026-04-10
version: long
short_url: /work/internship-1-short
long_url: /work/internship-1-long
categories: [work]
image: /assets/images/internship-1/fbt.gif
contributions: "VR-Chat-Based Full-Body Tracking in Unity | Video-to-FBX Converter Tool in Unity"
tools: "Unity, Android, C#, Python, C++, Bluetooth LE, Inertial Measurement Units"
platforms: "Virtual Reality (Meta Quest) | Windows | Android"
---

## ⚾ About SportsImproVR

![image1](../../assets/images/internship-1/page.png)


SportsImproVR is a virtual reality training company specialized in baseball, softball, and tennis.

[Click to visit the company website!](https://sportsimprovr.com/en/) 

---

## 🔨 My Contributions

---

### 🏃‍♂️VR Full-Body Tracking with XSens DOT Trackers [WIP]

![image1](../../assets/images/internship-1/trackers-demo.gif)

**❗This project currently is in active development!** 

Core tracking and rotation mapping are implemented. I am currently validating position and scaling from test objects to a full body rig.

**What it Does:**

Full Body tracking for Meta Quest using Xsens DOT trackers inside Unity.

The project streams live motion data from multiple Xsens DOT trackers over Bluetooth LE into a standalone Quest build, then maps that data onto in game objects and a body rig to drive full body movement.


**Key Features & Challenges:**

- Feasibility first: proved that the entire idea is possible and that trackers can indeed connect to the Meta Quest through Bluetooth by getting a single tracker connected with the example XSens app.
- Bluetooth LE integration in Unity: used a Unity BLE package to scan, connect, subscribe to notifications, and handle reconnects on Quest.
- Services and characteristics: learned and implemented the BLE model to find the right services and characteristics, then subscribe to the correct data streams.
- Decoding live sensor output: extracted usable orientation and motion data from the incoming packets and built debug tooling to validate correctness.
- Rotation mapping and calibration: solved coordinate space and quaternion issues so the cube rotation matches the real tracker, including offsets and axis remapping.
- Position tracking work in progress: exploring approaches for positional movement, dealing with IMU drift, filtering, and reset strategies.

**Impact on the Project:**
- Established a working pipeline from Xsens DOT to Quest in Unity, de risking the technical unknowns early.
- Produced a solid rotation mapping foundation that can be reused for multiple trackers and avatar bones.
- Defined clear next steps for position by narrowing the problem to drift and reference constraints, guiding the roadmap towards IK or hybrid anchoring.

---

### 📽️ Unity Video-to-FBX Converter Tool

![image1](../../assets/images/internship-1/tool-ui.gif)

**What it Does:**

Video-to-FBX converter is a tool which can convert multiple videos (mp4, mov) to an FBX animation all within Unity using [MoveAI](https://www.move.ai/).

**Key Features & Challenges:**
* MoveAI API Intergration: the tool converts videos using MoveAI's API by launching a custom-made Python script in Unity. 
* Clear user interface: the tool provides an easy-to-use UI for a frictionless workflow.
* Parallel Conversion: the tool uses Unity's tasks system to run multiple conversions at the same time.
* User feedback per task: each task provides clear feedback on the job's status and progress.
* Runs in the background: the tool runs in the background and doesn't disturb regular editor workflows.

**Impact on the Company:**
* Simplified the workflow for the senior programmer from a three-step operation (uploading videos to MoveAI, downloading the FBX, importing to Unity) into a one-click operation all within Unity, saving a lot of time.
* Improved user feedback by providing clear status and progress per video within Unity, giving reassurance and understanding to the user.
* Allowed for multiple video conversions in parallel, turning from a 15-minute consequitive conversion for 3 videos into a 5-minute parallel conversion for 3 videos (and more).