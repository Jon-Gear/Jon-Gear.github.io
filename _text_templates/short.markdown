---
layout: project
title: "<PROJECT TITLE>"
role: "<YOUR ROLE>"

version: <short | long>
short_url: <link>
long_url: <link>

date: <YYYY-MM-DD>
categories: [projects]
image: <thumbnail path>
description: "<Write a concise 1–2 sentence summary explaining what the project is, what problem it solves, and why it matters.>"
contributions: "<List your key contributions separated by | (focus on impact, not tasks)>"
tools: "<List core technologies, frameworks, and tools used>"
team_size: "<Solo / X people>"
platforms: "<Target platforms (PC, Web, Mobile, etc.)>"
duration: "<Timeframe (e.g., 3 months)>"
---

# 💼 Work/Project Title

Write a clear, human-readable project title. Avoid internal codenames unless explained.


# 👤 Zhangir Before the Project

Describe your **starting point before this project**:

- What skills did you already have?
- What were you unfamiliar or weak in?
- What assumptions or mindset did you bring into the project?

This section should establish a **baseline** so the reader can understand your growth.

---

# 🔨 My Contributions

---

## 📋 EXAMPLE CONTRIBUTION 1

![image1](../../assets/images/cool-demo.gif)

Clearly highlight ownership:

- **System / Feature:** What you built and why it mattered  
- **Technical Decision:** What you chose and why  
- **Impact:** What changed because of your work (performance, usability, scalability, etc.)  


### 🎯 What was the Goal?

Describe the **initial objective clearly**:

- What problem were you trying to solve?
- Who was this for (user, team, system)?
- What constraints existed (time, tech, team, performance, etc.)?

Make it clear **what success looked like**.


### Challenge 1: [PROBLEM NAME]

#### ❓ What was the Problem?

Explain the issue in detail:

- What exactly was broken, missing, or inefficient?
- When/where did it occur?
- How did it affect the system or user?

Avoid being vague — this should feel concrete and real.


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

