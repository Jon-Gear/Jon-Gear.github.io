---
layout: default
---

# About Me

My name is Zhangir Nurmukhambetov. 

I am an Engine & Tools Programmer.

[Read more...](/about/)

# My Projects

<ul>
  {% for post in site.posts %}
    {% if post.categories contains "projects" %}
      <li>
        <a href="{{ post.url }}">{{ post.title }}</a> - {{ post.date | date: "%B %-d, %Y" }}
      </li>
    {% endif %}
  {% endfor %}
</ul>