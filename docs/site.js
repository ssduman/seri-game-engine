const luaKeywords = new Set(["and", "break", "do", "else", "elseif", "end", "for", "function", "goto", "if", "in", "local", "not", "or", "repeat", "return", "then", "until", "while"]);
const luaLiterals = new Set(["nil", "true", "false", "self"]);
const luaToken = /(--[^\n]*)|("(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*')|(\b\d+(?:\.\d+)?\b)|([A-Za-z_]\w*)/g;

const escapeHtml = (text) => text.replace(/[&<>]/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;" })[c]);

const highlightLua = (source) => {
  let html = "";
  let last = 0;

  for (const match of source.matchAll(luaToken)) {
    const [text, comment, string, number, word] = match;
    html += escapeHtml(source.slice(last, match.index));
    last = match.index + text.length;

    let kind = null;
    if (comment) {
      kind = "comment";
    } else if (string) {
      kind = "string";
    } else if (number || luaLiterals.has(word)) {
      kind = "number";
    } else if (luaKeywords.has(word)) {
      kind = "keyword";
    } else if (/^\s*\(/.test(source.slice(last))) {
      kind = "function";
    } else if (/^[A-Z]/.test(word) && !".:".includes(source[match.index - 1])) {
      kind = "global";
    }

    html += kind ? `<span class="tok-${kind}">${escapeHtml(text)}</span>` : escapeHtml(text);
  }

  return html + escapeHtml(source.slice(last));
};

for (const block of document.querySelectorAll("code.language-lua")) {
  block.innerHTML = highlightLua(block.textContent);
}

fetch("https://api.github.com/repos/ssduman/seri-game-engine")
  .then((response) => (response.ok ? response.json() : null))
  .then((repo) => {
    if (!repo || typeof repo.stargazers_count !== "number") {
      return;
    }

    const stars = repo.stargazers_count;
    const text = stars < 1000 ? `${stars}` : `${(stars / 1000).toFixed(stars < 10000 ? 1 : 0).replace(/\.0$/, "")}K`;

    for (const element of document.querySelectorAll(".github-count")) {
      element.textContent = text;
    }
  })
  .catch(() => {});
