---
layout: default
---

# Zhangir Nurmukhambetov
## Engine & Tools Programmer

Student at Breda University of Applied Sciences in Creative Media and Game Technologies. 

Skilled in Engine and Tools programming with strong experience in custom engines.

Passionate about programming

Proficient in C++.

Additional experience with Unreal, Unity, and Godot.

Currently looking for a position starting in September 2025.


[Read more...](/about/)

# Skills

Languages:
- C++

Dev Tools:
- Microsoft Visual Studio
- Git


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
