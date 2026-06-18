---
layout: project
title: "Internship at Flashgroup XR"
company: "Flashgroup XR"
role: "Engine & Tools Programmer"
start_date: 2026-04-10
end_date: 2026-10-10
version: short
short_url: /work/internship-2-short
long_url: /work/internship-2-long
categories: [work]
image: /assets/images/internship-2/main_loop.gif
contributions: "Collider Editor | Primitive Collider Generator"
tools: "Unity, C#, Python, C++"
platforms: "Virtual Reality (Meta Quest) | Windows"
duration: "9 Months (Apr 2026 - Nov 2026)"
---

# 💼 Internship at Flashgroup XR

[Flashgroup XR](https://flashgroupxr.com/) develops immersive XR training systems for emergency services, including firefighters, police, security personnel and first-aid responders. Its portable, plug-and-play solutions combine multiplayer VR simulations, realistic training scenarios and replicas of professional equipment, allowing instructors to conduct and review practical emergency-response training almost anywhere.

I worked at Flashgroup XR as a Programming Intern / Unity Tools Developer. 

My main responsibility was **researching and developing internal tools that improved the team’s production workflow.**

My primary project was a custom Unity collider editor that helped making colliders in Unity for a given game object.

# 👤 Zhangir Before the Project

Describe your **starting point before this project**:

- What skills did you already have?
- What were you unfamiliar or weak in?
- What assumptions or mindset did you bring into the project?

This section should establish a **baseline** so the reader can understand your growth.

---

# 🔨 My Contributions

---

## 🛠️📦 The Collider Editor Tool

![image1](../../assets/images/internship-2/main_loop.gif)


**The Collider Editor** is a custom Unity editor tool that lets artists and developers **create accurate physics colliders directly from a mesh’s vertices**. Users can select parts of a model in the Scene View, preview the collider shape, and generate colliders such as spheres, capsules, boxes, cylinders, convex hulls, or convex-decomposition collider sets.

My goal was to **help create colliders as easy as possible**.

This tool was made for **Flashgroup’s 3D artists and programmers**. 

Before, an artist/programmer had to either **leave Unity to edit meshes in Blender** or rely on **Unity’s limited native collider tools**, which is **slow, repetitive, and error-prone**.

Now, they can **create, preview, and generate colliders directly inside Unity** using the mesh’s vertices, making the process **faster, more accurate, and easier to manage**.





```

Features related to UI:

= Simple and Clear User Interface - Adds a Unity editor window for creating and editing colliders from selected mesh vertices.

* **Generated Colliders Panel** — Lists colliders found on the selected object and its children.


Features related to preview:
= * **Editable Mesh Overlay** — Draws mesh wireframes, selected faces, selected edges, and vertex points in the Scene view.
= **Live Collider Preview** — Shows a Scene view preview of the selected collider type before creation.
= **Wireframe Preview Mode** — Allows primitive previews and existing collider previews to be drawn as outlines only.
= **Performance-Colored Previews** — Colors collider previews based on expected physics cost, such as sphere being fastest and mesh collider being slowest.
= **Existing Collider Performance View** — Draws existing colliders in the Scene view with performance labels and colors.




Features related to selection:
= Select Vertices From Scene - select mesh vertices directly in the Scene view by clicking or dragging a selection rectangle. You can add/remove vertices from selection, select all vertices 
= Connected Hull Selection — Double-clicking a vertex selects the connected mesh island or hull around that vertex.
= Through Select / Visible-Only Selection — Allows selection either through the mesh to back-side vertices or only to vertices visible from the Scene camera.

= Vertex Masking — Lets the user hide selected vertices so they are not drawn or selectable.
= Vertex Isolation — Hides every vertex except the current selection.





Colliders
= Boxes, Spheres, Capsules, Cylinders
= Mesh from selecte vertices - Creates a convex MeshCollider using a quick-hull mesh built from the selected vertices.
= Convex decomposition - Uses CoACD to generate multiple convex `MeshCollider` hulls from a selected region, useful for preserving concave shapes better than a single convex hull.



Other features:
* **Undo Support** — Integrates with Unity Undo for selection changes, masking, collider creation, and collider deletion.

* **Generated Mesh Asset Cleanup** — Deletes generated mesh assets when they are no longer used by any generated collider.

* **Persistent Generated Mesh Assets** — Saves generated MeshCollider meshes as Unity assets in organized generated-mesh folders.

* **Keyboard Shortcuts** — Supports shortcuts for undo, select all, hide, isolate, create collider, stop editing, and number-key collider selection/creation.


* **Background Collider Generation** — Runs heavy convex decomposition work on a background task and reports progress through Unity’s progress UI.





```






### Challenge 1: Creating a "Automatic Collider Generator"

#### ❓ What was the Problem?

My given tool objective is formulated as follows:

> **"A tool that can a create a compound collider structure, multiple primitive colliders (box, sphere,...) for a specific mesh (collider) to improve performance."**

My goal is to create an automatic collider generator. 

#### 🧩 Why Was it Hard? What Struggles Did I Face?

Explain why this problem was **non-trivial**:

- Technical unknowns?
- Poor documentation?
- Complex interactions between systems?
- Bugs that were hard to reproduce?
- Design ambiguity?

Show that the difficulty was **real and meaningful**, not artificial.


#### 💡 Solution. How Did I Approach the Problem?

This is the **main section**.

Explain your full thinking and execution process:

- How you broke down the problem
- What ideas you tried (including failed ones if useful)
- Key insights that led to the solution
- Step-by-step approach to implementation
- Why your solution works

Treat this as:
> “This is where I show how I think and build as an engineer.”

You should:
- Include design decisions
- Include architecture explanation
- Include code where relevant
- Explain *why*, not just *what*

Show your engineering judgment (Engineering Decisions & Trade-offs):

- Why did you choose one approach over another?
- What alternatives did you consider?
- What were the trade-offs?

Be explicit:

- Performance vs simplicity
- Flexibility vs complexity
- Speed of development vs scalability


Include **only meaningful code**, not random snippets.

```cpp
// Example: Why this structure?
void Update(float deltaTime)
{
    // Using a fixed timestep accumulator to ensure deterministic simulation
    accumulator += deltaTime;

    while (accumulator >= fixedStep)
    {
        Simulate(fixedStep); // Keeps physics stable
        accumulator -= fixedStep;
    }
}
```

**Explanation:**

* Why did you structure it this way?
* What problem does this solve?
* What would happen with a naive approach?
* What alternatives exist?

Code should support your thinking — not replace it.

#### ✅ Problem Solved

![image1](../../assets/images/problem-solved-demo.gif)

Show the **result visually or descriptively**:

* What changed after your solution?
* How do we know it works?
* What improved?

Make the success tangible.

#### 📉 Cost / Limitations / Tradeoffs

Be honest and critical:

* What did this solution sacrifice?
* What are its limitations?
* What wouldn’t scale?
* What technical debt was introduced?

Examples:

* Lacks async support
* Tight coupling between systems
* Performance not optimal in edge cases

This builds **credibility**.

### Challenge 2: Creating a "Collider Editor"

#### ❓ What was the Problem?

When I started the project, my initial tool objective was formulated as follows:

> **"A tool that can a create a compound collider structure, multiple primitive colliders (box, sphere,...) for a specific mesh (collider) to improve performance."**

At the time, I was told to create an automatic collider generator. But it turns out that the problem that I was trying to solve is much different from what I started.

Here were my observations:
- When I tested out the tool with artists, I made an important discovery: in order to make a decent collision for a given object, they take up 1-2 minutes.
- When I asked the artist what was the longest part of making a collider, they replied "opening up Blender". 
- When I asked an artist to show me how they would make a collider for a given model, I noticed how tedious to use the native Unity collision editor is. 

Based on all of this, in order for CPDCD to effectively "outcompete" the human, the algorithm must nail the generation for EVERY OBJECT pretty much instantly and on the first try. 

In this case, manual human work is actually better in terms of time to make and performance. If you can't beat manual human work, then make manual work as easy as possible.

This all culminated when I asked the programmer directly and found out that they need a proper collider editor.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

Explain why this problem was **non-trivial**:

- Technical unknowns?
- Poor documentation?
- Complex interactions between systems?
- Bugs that were hard to reproduce?
- Design ambiguity?

Show that the difficulty was **real and meaningful**, not artificial.


#### 💡 Solution. How Did I Approach the Problem?

This is the **main section**.

Explain your full thinking and execution process:

- How you broke down the problem
- What ideas you tried (including failed ones if useful)
- Key insights that led to the solution
- Step-by-step approach to implementation
- Why your solution works

Treat this as:
> “This is where I show how I think and build as an engineer.”

You should:
- Include design decisions
- Include architecture explanation
- Include code where relevant
- Explain *why*, not just *what*

Show your engineering judgment (Engineering Decisions & Trade-offs):

- Why did you choose one approach over another?
- What alternatives did you consider?
- What were the trade-offs?

Be explicit:

- Performance vs simplicity
- Flexibility vs complexity
- Speed of development vs scalability


Include **only meaningful code**, not random snippets.

```cpp
// Example: Why this structure?
void Update(float deltaTime)
{
    // Using a fixed timestep accumulator to ensure deterministic simulation
    accumulator += deltaTime;

    while (accumulator >= fixedStep)
    {
        Simulate(fixedStep); // Keeps physics stable
        accumulator -= fixedStep;
    }
}
```

**Explanation:**

* Why did you structure it this way?
* What problem does this solve?
* What would happen with a naive approach?
* What alternatives exist?

Code should support your thinking — not replace it.

#### ✅ Problem Solved

![image1](../../assets/images/problem-solved-demo.gif)

Show the **result visually or descriptively**:

* What changed after your solution?
* How do we know it works?
* What improved?

Make the success tangible.

#### 📉 Cost / Limitations / Tradeoffs

Be honest and critical:

* What did this solution sacrifice?
* What are its limitations?
* What wouldn’t scale?
* What technical debt was introduced?

Examples:

* Lacks async support
* Tight coupling between systems
* Performance not optimal in edge cases

This builds **credibility**.




### Challenge 3: Refining the "Collider Editor"

#### ❓ What was the Problem?

The artists and programmers in their feedback have noted a couple of things to add and refine:
- Mask/Hide vertices
- Confirm collider button
- Logical progression of things (select an object first, enter vertex selection mode, select vertices, select solution, create collider)
- Vertex selection mode: this is to signify to the user that Scene View tools (e.g. transform, box bounds) are unavailable to use because they are currently selecting vertices (so that we don't interfere with one another)
- CTRL + Z for vertices and other things in the editor
- Meshes are saved as an asset, not into a scene or anything.
- Shortcuts for selecting and creating colliders
- The mesh is drawn as a wireframe now, similar to Blender
- Hover tooltips
- Collider preview must have an option to be filled or be an outline
- Collider previews must have an option to be color-coded for performance (green = fast, red = slow)
- You can see a performance view of already generated colliders
- You can see a list of colliders we already generated
- You can deselect vertices on click
- Confirmation steps. Ask an "are you sure?" when you create a collider to make the user double-check their work.
- Added better descriptors for min-max, distance, best fit;
- Update the UI.


#### 🧩 Why Was it Hard? What Struggles Did I Face?

Explain why this problem was **non-trivial**:

- Technical unknowns?
- Poor documentation?
- Complex interactions between systems?
- Bugs that were hard to reproduce?
- Design ambiguity?

Show that the difficulty was **real and meaningful**, not artificial.


#### 💡 Solution. How Did I Approach the Problem?

This is the **main section**.

Explain your full thinking and execution process:

- How you broke down the problem
- What ideas you tried (including failed ones if useful)
- Key insights that led to the solution
- Step-by-step approach to implementation
- Why your solution works

Treat this as:
> “This is where I show how I think and build as an engineer.”

You should:
- Include design decisions
- Include architecture explanation
- Include code where relevant
- Explain *why*, not just *what*

Show your engineering judgment (Engineering Decisions & Trade-offs):

- Why did you choose one approach over another?
- What alternatives did you consider?
- What were the trade-offs?

Be explicit:

- Performance vs simplicity
- Flexibility vs complexity
- Speed of development vs scalability


Include **only meaningful code**, not random snippets.

```cpp
// Example: Why this structure?
void Update(float deltaTime)
{
    // Using a fixed timestep accumulator to ensure deterministic simulation
    accumulator += deltaTime;

    while (accumulator >= fixedStep)
    {
        Simulate(fixedStep); // Keeps physics stable
        accumulator -= fixedStep;
    }
}
```

**Explanation:**

* Why did you structure it this way?
* What problem does this solve?
* What would happen with a naive approach?
* What alternatives exist?

Code should support your thinking — not replace it.

#### ✅ Problem Solved

![image1](../../assets/images/problem-solved-demo.gif)

Show the **result visually or descriptively**:

* What changed after your solution?
* How do we know it works?
* What improved?

Make the success tangible.

#### 📉 Cost / Limitations / Tradeoffs

Be honest and critical:

* What did this solution sacrifice?
* What are its limitations?
* What wouldn’t scale?
* What technical debt was introduced?

Examples:

* Lacks async support
* Tight coupling between systems
* Performance not optimal in edge cases

This builds **credibility**.




### Challenge 4: Simplifying the "Collider Editor"

#### ❓ What was the Problem?

My product manager noted a couple points to look improve for the collider editor.

The collider creation workflow is messy and difficult to use.

When selecting a lot of vertices, the framerate fell down.


#### 🧩 Why Was it Hard? What Struggles Did I Face?

Explain why this problem was **non-trivial**:

- Technical unknowns?
- Poor documentation?
- Complex interactions between systems?
- Bugs that were hard to reproduce?
- Design ambiguity?

Show that the difficulty was **real and meaningful**, not artificial.


#### 💡 Solution. How Did I Approach the Problem?

This is the **main section**.

Explain your full thinking and execution process:

- How you broke down the problem
- What ideas you tried (including failed ones if useful)
- Key insights that led to the solution
- Step-by-step approach to implementation
- Why your solution works

Treat this as:
> “This is where I show how I think and build as an engineer.”

You should:
- Include design decisions
- Include architecture explanation
- Include code where relevant
- Explain *why*, not just *what*

Show your engineering judgment (Engineering Decisions & Trade-offs):

- Why did you choose one approach over another?
- What alternatives did you consider?
- What were the trade-offs?

Be explicit:

- Performance vs simplicity
- Flexibility vs complexity
- Speed of development vs scalability


Include **only meaningful code**, not random snippets.

```cpp
// Example: Why this structure?
void Update(float deltaTime)
{
    // Using a fixed timestep accumulator to ensure deterministic simulation
    accumulator += deltaTime;

    while (accumulator >= fixedStep)
    {
        Simulate(fixedStep); // Keeps physics stable
        accumulator -= fixedStep;
    }
}
```

**Explanation:**

* Why did you structure it this way?
* What problem does this solve?
* What would happen with a naive approach?
* What alternatives exist?

Code should support your thinking — not replace it.

#### ✅ Problem Solved

![image1](../../assets/images/problem-solved-demo.gif)

Show the **result visually or descriptively**:

* What changed after your solution?
* How do we know it works?
* What improved?

Make the success tangible.

#### 📉 Cost / Limitations / Tradeoffs

Be honest and critical:

* What did this solution sacrifice?
* What are its limitations?
* What wouldn’t scale?
* What technical debt was introduced?

Examples:

* Lacks async support
* Tight coupling between systems
* Performance not optimal in edge cases

This builds **credibility**.





### 💥 Impact of Contribution

Focus specifically on the before vs after transformation:

- What was the situation before your contribution?
- What is the situation now?

Highlight meaningful change:

- What is now easier for users or developers?
- What is now possible that wasn’t before?
- What friction or limitations were removed?

Make the impact clear, measurable, or strongly felt (even if qualitative).

### 📚 What I Learned

Focus on **thinking, not just doing**:

* What did you learn about engineering or system design?
* What mental models improved?
* What would you do differently next time?

Avoid generic statements — be specific.

### 🔄 Future Improvements

List realistic next steps:

* Features you would add
* Optimizations you would implement
* Architectural changes you would make

This shows **forward thinking and ownership**.

---

# 👤 Zhangir After the Project

Wrap up the story:

* Did you achieve the original goal?
* What are you most proud of?
* How did you grow as an engineer?
* What does this project say about how you think and work?

This should feel like a **before → after transformation**.

