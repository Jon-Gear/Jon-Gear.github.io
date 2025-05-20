---
layout: default
---

# About Me

My name is Zhangir Nurmukhambetov. 

I am an Engine & Tools Programmer.

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
