---
layout: default
---

# Zhangir Nurmukhambetov
## Engine & Tools Programmer

[Photo]

2nd year student at Breda University of Applied Sciences in Creative Media and Game Technologies.

I am skilled in **Engine and Tools** programming with strong experience in making custom engines. I enjoy **creating tools and systems** which help other disciplines streamline game development. I’m passionate about **solving technical challenges** and continuosly improving my skills.

Proficient in C++. Additional experience with Unreal, Unity, and Godot.

Hobbies: writing stories, calisthenics, designing in Photoshop.

Currently looking for a position in engine programming and/or content creation tool development starting in September 2025.

[Download Resume]

# Skills

- C++. 8 years of experience in C++
- Visual Studio. 4 years of experience. Familiar with debugging, profiling, and project configuration tools
- Visual Studio Code
- Github. 3 years of experience. Experienced with its pipelines and used them in a team environment, including reviews, automated checks, and managing branches.
- Perforce. 2 years of experience with Perforce and P4V in a collaborative team environment, including active participation in code reviews.
- Unreal Engine. 
- Unity C#. Spend my free time using Unity by making a side passion project. 
- Python. 
- Godot. 


# My Projects

<div class="project-list">
  {% assign sorted_projects = site.projects | sort: "date" | reverse %}
  {% for project in sorted_projects %}
    {% if project.categories contains "projects" %}
      <a href="{{ project.url }}" class="project-card-link">
        <div class="project-card">
          {% if project.image %}
            <img src="{{ project.image }}" alt="Preview of {{ project.title }}">
          {% endif %}

          <h2>{{ project.title }}</h2>
          <p><strong>Date:</strong> {{ project.date | date: "%B %-d, %Y" }}</p>

          {% if project.description %}
            <p>{{ project.description }}</p>
          {% endif %}

          {% if project.contributions %}
            <p><strong>My Contributions:</strong> {{ project.contributions }}</p>
          {% endif %}

          {% if project.tools %}
            <p><strong>Engine/Tools:</strong> {{ project.tools }}</p>
          {% endif %}

          {% if project.team_size %}
            <p><strong>Team Size:</strong> {{ project.team_size }}</p>
          {% endif %}

          {% if project.platforms %}
            <p><strong>Platforms:</strong> {{ project.platforms }}</p>
          {% endif %}

          {% if project.duration %}
            <p><strong>Duration:</strong> {{ project.duration }}</p>
          {% endif %}
        </div>
      </a>
    {% endif %}
  {% endfor %}
</div>
