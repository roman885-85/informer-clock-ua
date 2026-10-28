// Документація Informer Clock UA у PDF: одна книга на мову (обкладинка, зміст, усі частини поспіль) і кожна частина
// окремим файлом. Тексти — ті самі, що на GitHub (docs/uk, docs/en, README) і на сайті (docs/manual).
// EN: builds the documentation PDFs — one book per language (cover, contents, all parts) and every part separately.
//
//   npm install --prefix tools/docs markdown-it@14      (один раз / once)
//   node tools/docs/build-pdf.mjs [тека-виходу]          (типово / default: release/docs-pdf)
//
// Потрібен Google Chrome, Chromium або Edge: сторінки друкуються в PDF без вікна (шлях можна задати змінною CHROME).
// markdown-it шукається в tools/docs/node_modules або за шляхом у змінній MARKDOWN_IT (тека пакета).
import { createRequire } from 'module';
import { readFileSync, writeFileSync, mkdirSync, rmSync, existsSync, statSync, readSync, openSync, closeSync } from 'fs';
import { execFileSync } from 'child_process';
import { tmpdir } from 'os';
import path from 'path';
import { fileURLToPath, pathToFileURL } from 'url';

const ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..', '..');
const OUT = path.resolve(process.argv[2] || path.join(ROOT, 'release', 'docs-pdf'));
const REPO = 'https://github.com/roman885-85/informer-clock-ua';
const PAGES = 'https://roman885-85.github.io/informer-clock-ua';
const VERSION = readFileSync(path.join(ROOT, 'firmware/src/version.h'), 'utf8').match(/FX_VERSION "([^"]+)"/)[1];

function loadMarkdownIt() {
  try {
    const dir = process.env.MARKDOWN_IT;
    return dir ? createRequire(path.join(dir, 'package.json'))(dir) : createRequire(import.meta.url)('markdown-it');
  } catch {
    console.error('немає markdown-it: npm install --prefix tools/docs markdown-it@14 (або MARKDOWN_IT=<тека пакета>)');
    process.exit(1);
  }
}
const MarkdownIt = loadMarkdownIt();

function findChrome() {
  const list = [process.env.CHROME, '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome',
    '/Applications/Chromium.app/Contents/MacOS/Chromium', '/Applications/Microsoft Edge.app/Contents/MacOS/Microsoft Edge',
    '/usr/bin/google-chrome', '/usr/bin/chromium', '/usr/bin/chromium-browser',
    'C:/Program Files/Google/Chrome/Application/chrome.exe', 'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'];
  const c = list.find((p) => p && existsSync(p));
  if (!c) { console.error('немає браузера Chrome/Chromium/Edge (змінна CHROME=<шлях>)'); process.exit(1); }
  return c;
}

// Частини: id (латиницею — для імен файлів і якорів), назва, джерело. Манал — блок data-l з docs/manual/index.html.
const LANGS = {
  uk: {
    book: 'Informer Clock UA — документація', sub: 'Незалежна прошивка годинника-інформера на ESP32 і матрицях MAX7219',
    toc: 'Зміст', pre: 'Попередній випуск: зібрано й перевірено на імітаторах і в браузері, але ще не на годиннику.',
    credit: 'На основі проєкту Clock Informer автора Semen_p (thingiverse.com/thing:3924947). Ліцензія CC BY-NC 4.0.',
    version: 'Версія прошивки', date: 'Дата',
    parts: [['about', 'Про проєкт', 'README.md'], ['manual', 'Інструкція користувача', 'docs/manual/index.html'],
      ['flashing', 'Прошивка', 'docs/uk/flashing.md'], ['wiring', 'Підключення', 'docs/uk/wiring.md'],
      ['building', 'Збирання з вихідного коду', 'docs/uk/building.md'], ['architecture', 'Як влаштована прошивка', 'docs/uk/architecture.md'],
      ['wifi-debug', 'Налагодження по Wi-Fi', 'docs/uk/wifi-debug.md'], ['troubleshooting', 'Якщо щось не так', 'docs/uk/troubleshooting.md']],
  },
  en: {
    book: 'Informer Clock UA — documentation', sub: 'Independent firmware for the ESP32 + MAX7219 LED-matrix information clock',
    toc: 'Contents', pre: 'Pre-release: built and tested in simulators and in the browser, but not yet on a clock.',
    credit: 'Based on the Clock Informer project by Semen_p (thingiverse.com/thing:3924947). License CC BY-NC 4.0.',
    version: 'Firmware version', date: 'Date',
    parts: [['about', 'About the project', 'README.en.md'], ['manual', 'User manual', 'docs/manual/index.html'],
      ['flashing', 'Flashing', 'docs/en/flashing.md'], ['wiring', 'Wiring', 'docs/en/wiring.md'],
      ['building', 'Building from source', 'docs/en/building.md'], ['architecture', 'How the firmware works', 'docs/en/architecture.md'],
      ['wifi-debug', 'Wi-Fi debugging', 'docs/en/wifi-debug.md'], ['troubleshooting', 'Troubleshooting', 'docs/en/troubleshooting.md']],
  },
};

const slug = (t) => t.trim().toLowerCase().replace(/[^\p{L}\p{N}\s_-]/gu, '').replace(/\s/g, '-');
const esc = (s) => s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');

// Посилання з тексту частини: на іншу частину — всередину книги (у книзі) або на GitHub (окремий PDF);
// на файл репозиторію — на GitHub; картинки — локальні файли. env: { from, lang, prefix, book }
function linkTarget(href, env) {
  if (/^(https?:|mailto:)/.test(href)) return { href };
  if (href.startsWith('#')) return { href: `#${env.prefix}${href.slice(1)}` };
  const [file, anchor] = href.split('#');
  const rel = path.relative(ROOT, path.resolve(path.dirname(path.join(ROOT, env.from)), file.split('?')[0]));
  const part = LANGS[env.lang].parts.find((p) => p[2] === rel);
  if (part && env.book) return { href: `#${part[0]}--${anchor || 'top'}` };
  if (rel.startsWith('docs/') && !rel.endsWith('.md')) {           // сторінки сайту (прошивальник, інструкція)
    const web = rel.slice(5).replace(/index\.html$/, '');
    return { href: `${PAGES}/${web}${href.includes('?') ? '?' + href.split('?')[1].split('#')[0] : ''}` };
  }
  const isDir = existsSync(path.join(ROOT, rel)) && statSync(path.join(ROOT, rel)).isDirectory();
  return { href: `${REPO}/${isDir ? 'tree' : 'blob'}/main/${rel}${anchor ? '#' + anchor : ''}` };
}

const md = new MarkdownIt({ html: true, linkify: false });
md.core.ruler.push('anchors', (state) => {
  const seen = new Map();
  state.env.toc = [];
  state.tokens.forEach((tok, i) => {
    if (tok.type !== 'heading_open') return;
    const text = state.tokens[i + 1].children.filter((t) => t.type === 'text' || t.type === 'code_inline').map((t) => t.content).join('');
    let id = slug(text);
    const n = seen.get(id) || 0; seen.set(id, n + 1);
    if (n) id += `-${n}`;
    tok.attrSet('id', state.env.prefix + id);
    if (tok.tag === 'h2') state.env.toc.push([state.env.prefix + id, text]);
  });
});
const baseLink = md.renderer.rules.link_open || ((t, i, o, e, s) => s.renderToken(t, i, o));
md.renderer.rules.link_open = (tokens, idx, options, env, self) => {
  tokens[idx].attrSet('href', linkTarget(tokens[idx].attrGet('href') || '', env).href);
  return baseLink(tokens, idx, options, env, self);
};
md.renderer.rules.image = (tokens, idx, options, env) => {
  const t = tokens[idx];
  const src = path.resolve(path.dirname(path.join(ROOT, env.from)), t.attrGet('src'));
  return `<img src="${pathToFileURL(src).href}" alt="${esc(t.content)}">`;
};

// Markdown без першого рядка-навігації ([English](…) · [Зміст]…) і без заголовка H1 (його дає назва частини).
function partMarkdown(src, env) {
  let text = readFileSync(path.join(ROOT, src), 'utf8');
  const lines = text.split('\n');
  const h1 = lines.findIndex((l) => l.startsWith('# '));
  if (h1 >= 0) lines.splice(h1, 1);
  let i = lines.findIndex((l) => l.trim());
  if (i >= 0 && /^\[(English|Українською)\]\(/.test(lines[i])) { while (i < lines.length && lines[i].trim()) lines.splice(i, 1); }
  return md.render(lines.join('\n'), env);
}

// Інструкція: вміст <div data-l="uk|en"> зі сторінки сайту; заголовкам — якоря, посиланням і картинкам — повні адреси.
function partManual(lang, env) {
  const html = readFileSync(path.join(ROOT, 'docs/manual/index.html'), 'utf8');
  const start = html.indexOf(`<div data-l="${lang}">`);
  const body = html.slice(start, html.indexOf('\n</div>\n', start));
  env.toc = [];
  const seen = new Map();
  return body.replace(/^<div data-l="\w+">\n/, '')
    .replace(/<h1>[\s\S]*?<\/h1>\n?/, '')
    .replace(/<h2>([\s\S]*?)<\/h2>/g, (m, t) => {
      const plain = t.replace(/<[^>]+>/g, '');
      let id = env.prefix + slug(plain); const n = seen.get(id) || 0; seen.set(id, n + 1); if (n) id += `-${n}`;
      env.toc.push([id, plain]);
      return `<h2 id="${id}">${t}</h2>`;
    })
    .replace(/src="\.\.\/([^"]+)"/g, (m, p) => `src="${pathToFileURL(path.join(ROOT, 'docs', p)).href}"`)
    .replace(/href="\.\.\/([^"]*)"/g, (m, p) => `href="${PAGES}/${p}"`);
}

const CSS = `
@page { size: A4; margin: 17mm 16mm 19mm;
  @bottom-center { content: counter(page); font: 9pt system-ui, -apple-system, "Segoe UI", Arial, sans-serif; color: #6a7280; }
  @bottom-left { content: "Informer Clock UA ${VERSION}"; font: 8pt system-ui, -apple-system, "Segoe UI", Arial, sans-serif; color: #9aa1ab; } }
@page cover { @bottom-center { content: none; } @bottom-left { content: none; } }
* { box-sizing: border-box; }
body { margin: 0; color: #1f2328; font: 10.5pt/1.5 system-ui, -apple-system, "Segoe UI", "Helvetica Neue", Arial, sans-serif; }
.cover { page: cover; height: 250mm; display: flex; flex-direction: column; justify-content: center; }
.cover h1 { font-size: 28pt; margin: 0 0 6pt; break-before: auto; }
.cover .sub { font-size: 14pt; color: #4b5563; margin: 0 0 22pt; }
.cover .meta { font-size: 11pt; line-height: 1.8; }
.cover .pre, blockquote { border-left: 3pt solid #b54708; background: #fff7ed; padding: 6pt 10pt; margin: 10pt 0; }
.cover .credit { margin-top: 26pt; color: #4b5563; font-size: 10pt; }
.toc { break-before: page; }
.toc ol { padding-left: 16pt; } .toc li { margin: 3pt 0; } .toc ul { margin: 2pt 0 6pt; padding-left: 14pt; color: #4b5563; font-size: 9.5pt; }
section.part { break-before: page; }
h1 { font-size: 21pt; line-height: 1.2; margin: 0 0 10pt; }
h2 { font-size: 14pt; margin: 16pt 0 6pt; break-after: avoid; }
h3 { font-size: 11.5pt; margin: 12pt 0 4pt; break-after: avoid; }
p, li { orphans: 2; widows: 2; }
a { color: #1d5fa8; text-decoration: none; }
code, kbd { font: 8.8pt Menlo, Consolas, monospace; background: #f1f2f4; border-radius: 3pt; padding: 0 2pt; }
pre { white-space: pre-wrap; word-break: break-word; background: #f4f5f7; border: 1px solid #e3e5e9; border-radius: 4pt; padding: 6pt 8pt; break-inside: avoid; }
pre code { background: none; padding: 0; font-size: 8.3pt; }
table { border-collapse: collapse; width: 100%; font-size: 9pt; margin: 6pt 0; }
th, td { border: 1px solid #d5d8de; padding: 3pt 5pt; vertical-align: top; text-align: left; }
th { background: #f1f2f4; } tr { break-inside: avoid; }
img { max-width: 100%; height: auto; }
.card { border: 1px solid #d5d8de; border-radius: 5pt; padding: 6pt 10pt; margin: 8pt 0; }
.note { border-left: 3pt solid #b54708; background: #fff7ed; }
.lead { color: #4b5563; font-size: 11.5pt; }
`;

const page = (lang, title, body) => `<!doctype html><html lang="${lang}"><head><meta charset="utf-8"><title>${esc(title)}</title><style>${CSS}</style></head><body>${body}</body></html>`;

function renderPart(lang, [id, title, src], book) {
  const env = { from: src, lang, prefix: book ? `${id}--` : '', book };
  const body = src.endsWith('.html') ? partManual(lang, env) : partMarkdown(src, env);
  return { id, title, toc: env.toc, html: `<section class="part"><h1 id="${env.prefix}top">${esc(title)}</h1>${body}</section>` };
}

const chrome = findChrome();
const work = path.join(tmpdir(), `icua-pdf-${process.pid}`);
const profile = path.join(work, 'chrome-profile');             // свій профіль: налаштування браузера користувача не зачіпаються
mkdirSync(work, { recursive: true });
rmSync(OUT, { recursive: true, force: true });
mkdirSync(OUT, { recursive: true });
const today = new Date().toISOString().slice(0, 10);

function toPdf(html, name, lang, title) {
  const src = path.join(work, name.replace(/\.pdf$/, '.html'));
  writeFileSync(src, page(lang, title, html));
  const out = path.join(OUT, name);
  execFileSync(chrome, ['--headless=new', '--disable-gpu', '--no-pdf-header-footer', '--generate-pdf-document-outline',
    `--user-data-dir=${profile}`, `--print-to-pdf=${out}`, pathToFileURL(src).href], { stdio: 'ignore', timeout: 180000 });
  const fd = openSync(out, 'r'); const head = Buffer.alloc(4); readSync(fd, head, 0, 4, 0); closeSync(fd);
  if (head.toString() !== '%PDF') { console.error(`не PDF: ${name}`); process.exit(1); }
  console.log(`${name}  ${Math.round(statSync(out).size / 1024)} КБ`);
}

for (const [lang, L] of Object.entries(LANGS)) {
  // окремі частини
  L.parts.forEach((p, i) => {
    const part = renderPart(lang, p, false);
    toPdf(part.html.replace('<section class="part">', '<section>'), `informer-clock-ua-${VERSION}-${lang}-${String(i + 1).padStart(2, '0')}-${p[0]}.pdf`, lang, `${p[1]} — Informer Clock UA`);
  });
  // книга
  const parts = L.parts.map((p) => renderPart(lang, p, true));
  const cover = `<div class="cover"><h1>${esc(L.book)}</h1><p class="sub">${esc(L.sub)}</p>
<div class="meta">${L.version}: <b>${VERSION}</b><br>${L.date}: ${today}<br><a href="${REPO}">${REPO.replace('https://', '')}</a><br><a href="${PAGES}/">${PAGES.replace('https://', '')}</a></div>
<div class="pre">${esc(L.pre)}</div><p class="credit">${esc(L.credit)}</p></div>`;
  const toc = `<div class="toc"><h1>${L.toc}</h1><ol>${parts.map((p) => `<li><a href="#${p.id}--top">${esc(p.title)}</a><ul>${p.toc.map(([id, t]) => `<li><a href="#${id}">${esc(t)}</a></li>`).join('')}</ul></li>`).join('')}</ol></div>`;
  toPdf(cover + toc + parts.map((p) => p.html).join('\n'), `informer-clock-ua-${VERSION}-docs-${lang}.pdf`, lang, L.book);
}
rmSync(work, { recursive: true, force: true });
if (process.platform === 'darwin') { try { execFileSync('xattr', ['-cr', OUT]); } catch {} }   // Chrome позначає файли карантином
console.log(`готово / done: ${OUT}`);
