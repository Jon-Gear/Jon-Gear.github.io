---
layout: default
---

# 👋 Hi, I'm Zhangir 

🎓 I'm a 3rd year student at Breda University of Applied Sciences in Creative Media and Game Technologies.

💼 I'm a Tools Developer intern at SportsImproVR.

I am skilled in **Engine and Tools** programming with strong experience in making custom engines. I enjoy **creating tools and systems** which help other disciplines streamline game development. I’m passionate about **solving technical challenges** and continuously improving my skills.

Proficient in C++. Additional experience with Unreal, Unity, and Godot.

Hobbies: writing stories, calisthenics, designing in Photoshop.

# ⚙️ Skills

<div class="skills-bar">
  <div class="skill-icon">
    <img src="assets/images/icons/cpp.svg" alt="C++" />
    <div class="tooltip">
      C++<br>
      8 years of experience in C++
    </div>
  </div>

  <div class="skill-icon">
    <img src="assets/images/icons/vs.png" alt="Visual Studio" />
    <div class="tooltip">
      Visual Studio<br>
      4 years of experience. Familiar with debugging, profiling, and project configuration tools
    </div>
  </div>

  <div class="skill-icon">
    <img src="assets/images/icons/vscode.png" alt="Visual Studio Code" />
    <div class="tooltip">
      Visual Studio Code<br>
    </div>
  </div>

  <div class="skill-icon">
    <img src="assets/images/icons/github-light.svg" alt="Github" />
    <div class="tooltip">
      Github <br> 
      3 years of experience. Experienced with its pipelines and used them in a team environment, including reviews, automated checks, and managing branches.
    </div>
  </div>

  <div class="skill-icon">
    <img src="assets/images/icons/perforce.png" alt="Perforce" />
    <div class="tooltip">
      Perforce <br> 
      2 years of experience with Perforce and P4V in a collaborative team environment, including active participation in code reviews.
    </div>
  </div>

  <div class="skill-icon">
    <img src="assets/images/icons/unreal.svg" alt="Unreal Engine" />
    <div class="tooltip">
      Unreal Engine <br>
      Familiar working with Unreal Engine's Blueprints and in C++.
    </div>
  </div>

  <div class="skill-icon">
    <img src="assets/images/icons/unity.svg" alt="Unreal Engine" />
    <div class="tooltip">
      Unity C# <br>
      Familiar working with Unity and in C#
    </div>
  </div>
</div>



# 💼 My Professional Experience

<div class="project-list">
  {% assign sorted_work = site.work | sort: "date" | reverse %}
  {% for job in sorted_work %}
    {% if job.categories contains "work" %}
      <div class="project-card">
        {% if job.image %}
          <a href="{{ job.url }}" class="project-image-link">
            <img src="{{ job.image }}" alt="Preview of {{ job.title }}">
          </a>
        {% endif %}

        <h2>
          <a href="{{ job.url }}" class="project-title-link">{{ job.title }}</a>
        </h2>

        <p><strong>Date:</strong> {{ job.date | date: "%B %-d, %Y" }}</p>

        {% if job.company %}
          <p><strong>Company:</strong> {{ job.company }}</p>
        {% endif %}

        {% if job.work_type %}
          <p><strong>Work Type:</strong> {{ job.work_type }}</p>
        {% endif %}


        {% if job.description %}
          <p>{{ job.description }}</p>
        {% endif %}
        
        <p><a href="{{ job.url }}" class="read-more-link">Read more →</a></p>

        {% if job.contributions %}
          <p><strong>My Contributions:</strong> {{ job.contributions }}</p>
        {% endif %}

        {% if job.tools %}
          <p><strong>Tools:</strong> {{ job.tools }}</p>
        {% endif %}

        {% if job.team_size %}
          <p><strong>Team Size:</strong> {{ job.team_size }}</p>
        {% endif %}

        {% if job.duration %}
          <p><strong>Duration:</strong> {{ job.duration }}</p>
        {% endif %}
      </div>
    {% endif %}
  {% endfor %}
</div>

<br>

# 🚀 My Projects

<div class="project-list">
  {% assign sorted_projects = site.projects | sort: "date" | reverse %}
  {% for project in sorted_projects %}
    {% if project.categories contains "projects" %}
      <div class="project-card">
        {% if project.image %}
          <a href="{{ project.url }}" class="project-image-link">
            <img src="{{ project.image }}" alt="Preview of {{ project.title }}">
          </a>
        {% endif %}

        <h2>
          <a href="{{ project.url }}" class="project-title-link">{{ project.title }}</a>
        </h2>

        <p><strong>Date:</strong> {{ project.date | date: "%B %-d, %Y" }}</p>

        {% if project.description %}
          <p>{{ project.description }}</p>
        {% endif %}
        
        <p><a href="{{ project.url }}" class="read-more-link">Read more →</a></p>

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
    {% endif %}
  {% endfor %}
</div>

