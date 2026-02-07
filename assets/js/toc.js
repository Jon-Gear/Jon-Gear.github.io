(function () {
  const toc = document.getElementById("toc");
  const content = document.getElementById("content");
  if (!toc || !content) return;

  const headings = content.querySelectorAll("h2, h3");
  if (!headings.length) return;

  const ul = document.createElement("ul");

  headings.forEach((h) => {
    if (!h.id) {
      h.id = h.textContent
        .toLowerCase()
        .trim()
        .replace(/[^\w\s]/g, "")
        .replace(/\s+/g, "-");
    }

    const li = document.createElement("li");
    li.className = "toc-" + h.tagName.toLowerCase();

    const a = document.createElement("a");
    a.href = "#" + h.id;
    a.textContent = h.textContent;

    li.appendChild(a);
    ul.appendChild(li);
  });

  toc.appendChild(ul);
})();
