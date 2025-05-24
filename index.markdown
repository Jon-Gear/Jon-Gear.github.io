---
layout: default
---

# Zhangir Nurmukhambetov
## Engine & Tools Programmer

[Photo]

2nd year student at Breda University of Applied Sciences in Creative Media and Game Technologies. 

I am skilled in **Engine and Tools programming** with strong experience in making custom engines. I enjoy **creating tools and systems** which help other disciplines streamline game development. I'm passionate about **solving technical challenges** and continuosly improving my skills. 

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
  {% for post in site.posts %}
    {% if post.categories contains "projects" %}
      <a href="{{ post.url }}" class="project-card-link">
        <div class="project-card">
          {% if post.preview_image %}
            <img src="{{ post.preview_image }}" alt="Preview of {{ post.title }}">
          {% endif %}

          <h2>{{ post.title }}</h2>
          <p><strong>Date:</strong> {{ post.date | date: "%B %-d, %Y" }}</p>

          {% if post.description %}
            <p>{{ post.description }}</p>
          {% endif %}

          {% if post.contributions %}
            <p><strong>My Contributions:</strong> {{ post.contributions }}</p>
          {% endif %}

          {% if post.tools %}
            <p><strong>Engine/Tools:</strong> {{ post.tools }}</p>
          {% endif %}

          {% if post.team_size %}
            <p><strong>Team Size:</strong> {{ post.team_size }}</p>
          {% endif %}

          {% if post.platforms %}
            <p><strong>Platforms:</strong> {{ post.platforms }}</p>
          {% endif %}

          {% if post.duration %}
            <p><strong>Duration:</strong> {{ post.duration }}</p>
          {% endif %}
        </div>
      </a>
    {% endif %}
  {% endfor %}
</div>
