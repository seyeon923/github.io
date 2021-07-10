---
title: "Python"
layout: category
permalink: categories/python
author_profile: true
---

{% assign posts = site.categories['C/C++'] %}
{% for post in posts %} {% include archive-single.html type=page.entires_layout %} {% endfor %}