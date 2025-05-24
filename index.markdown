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

<ul class="project-list">
  {% for post in site.posts %}
    {% if post.categories contains "projects" %}
      <li class="project-item">
        <a href="{{ post.url }}" class="project-box" style="background-image: url('{{ post.image }}');">
          <h2 class="project-title">{{ post.title }}</h2>
        </a>
      </li>
    {% endif %}
  {% endfor %}
</ul>
