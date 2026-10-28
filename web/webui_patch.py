#!/usr/bin/env python3
"""webui_patch.py — збирає веб-інтерфейс Informer Clock UA з оригінального інтерфейсу Clock Informer 2.7.1 (React,
web/original/index.html.gz, автор Semen_p, CC BY-NC 4.0) точними правками й пише його в прошивку
(firmware/src/web_assets.h: масив INDEX_HTML_GZ і мітка INDEX_HTML_ETAG) та в web/build/index.html(.gz).
Нових сторінок немає — лише поля на існуючих сторінках, наші адреси (оновлення, мовні пакети, файли, інструкція)
замість сервера автора, назва проєкту й посилання на автора оригіналу.

EN: builds the Informer Clock UA web interface from the original Clock Informer 2.7.1 UI (by Semen_p, CC BY-NC 4.0)
by exact string patches and writes it into the firmware (firmware/src/web_assets.h) and web/build/. Every patch must
match exactly once. Run: python3 web/webui_patch.py"""
import gzip, hashlib, json, os, re, sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
ORIG = os.path.join(ROOT, 'web', 'original', 'index.html.gz')
OUTD = os.path.join(ROOT, 'web', 'build')
ASSETS = os.path.join(ROOT, 'firmware', 'src', 'web_assets.h')
CHANGELOG = os.path.join(ROOT, 'update', 'changelog.json')

REPO = 'https://github.com/roman885-85/informer-clock-ua'
RAW = 'https://raw.githubusercontent.com/roman885-85/informer-clock-ua/main'
PAGES = 'https://roman885-85.github.io/informer-clock-ua'
FILES = REPO + '/releases/download/files'
NAME = 'Informer Clock UA'

# (опис, було, стало[, скільки входжень]) — у зібраному коді: t(...) — елемент React; ye — перемикач, gr — кнопки-
# сегменти, ne — список; c("ключ") — переклад з мовного пакета (беремо лише наявні ключі — написи мовою інтерфейсу)
PATCHES = [
    # --- функції на існуючих сторінках
    ('Сторінка «Час»: «Літній час» — Вимк | Увімк | Авто (sumT 0/1/2; авто — правила ЄС, firmware/src/timefix.cpp)',
     't(ye,{id:"sumT",label:c("tSummerTimeNTP")})',
     't(gr,{id:"sumT",label:c("tSummerTimeNTP"),options:[{value:0,title:c("tNotshow")},{value:1,title:c("tShow")},'
     '{value:2,title:c("tWifiAuto")}]})'),
    ('Сторінка «System»: перемикач «Налагодження по Wi-Fi» (ключ wdbg, firmware/src/wdbg.cpp) — компонент FxWd',
     'Kf=()=>{var R,U,ie,me;',
     'FxWd=({ip:e})=>{const n=v(a=>a.settings.lang),r=v(a=>a.settings.wdbg)==="1",'
     'i=n==="3"?["Налагодження по Wi-Fi","журнал","прошивка по Wi-Fi (OTA, порт 3232), пароль — як у веб-інтерфейсу"]:'
     'n==="0"||n==="2"?["Отладка по Wi-Fi","журнал","прошивка по Wi-Fi (OTA, порт 3232), пароль — как у веб-интерфейса"]:'
     '["Wi-Fi debug","log","firmware over Wi-Fi (OTA, port 3232), password — as for the web interface"];'
     'return t(de,{properties:["wdbg"],noContainer:!0,children:t("div",{className:"flex flex-col gap-1",children:['
     't(ye,{id:"wdbg",label:i[0]}),r&&t("span",{className:"px-3.5 pb-1 font-mono text-[11px] leading-relaxed '
     'text-black/55 dark:text-white/55",children:`${i[1]}: nc ${e||"<IP>"} 23 · ${i[2]}`})]})})},'
     'Kf=()=>{var R,U,ie,me;'),
    ('Сторінка «System»: рядок FxWd у картці System — після смуги пам\'яті',
     'C,"%"]})]})]})]})}),t("div",{className:"rounded-[14px]',
     'C,"%"]})]})]}),t(FxWd,{ip:c.local_ip})]})}),t("div",{className:"rounded-[14px]'),
    # --- усе, що бралось із сервера автора, — з цього проєкту
    ('Мовні пакети (index.json, <code>.json, <id>.led) — з репозиторію проєкту (lang/)',
     'xa="https://www.goldart.ee/api/lang"', f'xa="{RAW}/lang"'),
    ('Історія версій («What\'s New») — з репозиторію проєкту (update/changelog.json)',
     'vf="https://www.goldart.ee/api/changelog.json"', f'vf="{RAW}/update/changelog.json"'),
    ('Файли для SD-картки (звуки голосу) — з випуску «files» проєкту',
     'https://www.goldart.ee/api/files/sounds_sd_card.zip', f'{FILES}/sounds_sd_card.zip'),
    ('Корпуси для 3D-друку — з випуску «files» проєкту',
     'https://www.goldart.ee/api/files/clock_cases.zip', f'{FILES}/clock_cases.zip'),
    ('Картинка до звуків — з репозиторію проєкту', 'https://www.goldart.ee/web_images/sounds_sd.jpg', f'{RAW}/docs/images/sounds_sd.jpg'),
    ('Картинка до корпусів — з репозиторію проєкту', 'https://www.goldart.ee/web_images/3d_printing.jpg', f'{RAW}/docs/images/3d_printing.jpg'),
    ('Інструкція (вбудоване вікно й посилання) — сторінки проєкту', 'https://www.goldart.ee/manual/', f'{PAGES}/manual/', 2),
    # --- назва проєкту; авторство оригіналу лишається (посилання на автора в підвалі)
    ('Заголовок вкладки браузера', '<title>Clock Informer</title>', f'<title>{NAME}</title>'),
    ('Назва в шапці', 'dark:to-emerald-500 ${r?"!text-3xl":"!text-2xl sm:!text-3xl"}`,children:"Clock Informer"}',
     'dark:to-emerald-500 ${r?"!text-3xl":"!text-2xl sm:!text-3xl"}`,children:"' + NAME + '"}'),
    ('Позначка в шапці: замість «GoldArtOS»',
     'text-[11px] font-bold text-amber-600 dark:text-amber-400",children:"GoldArtOS"}',
     'text-[11px] font-bold text-amber-600 dark:text-amber-400",children:"fork"}'),
    ('Назва в бічному меню', 'bg-clip-text text-base font-black text-transparent",children:"Clock Informer"}',
     'bg-clip-text text-base font-black text-transparent",children:"' + NAME + '"}'),
    ('Назва в мобільному меню', 'bg-clip-text text-sm font-black text-transparent",children:"Clock Informer"}',
     'bg-clip-text text-sm font-black text-transparent",children:"' + NAME + '"}'),
    ('Підвал: «Powered by GoldArtOS» → посилання на цей проєкт (посилання «Clock Informer by Sem» на автора лишається)',
     'children:"Powered by GoldArtOS"',
     f'children:t("a",{{href:"{REPO}",target:"_blank",rel:"noreferrer",className:"hover:underline",'
     f'children:"{NAME} — незалежний форк · independent fork"}})', 2),
    ('Майстер першого запуску: «Powered by GoldArtOS» → назва проєкту',
     'e("tWizPoweredBy")," ",t("span",{className:"font-bold text-black/50 dark:text-white/50",children:"GoldArtOS"})',
     'e("tWizPoweredBy")," ",t("span",{className:"font-bold text-black/50 dark:text-white/50",children:"' + NAME + '"})'),
    ('Підпис кнопки меню (доступність)', '"aria-label":"Clock Informer"', '"aria-label":"' + NAME + '"'),
    ('Підказка імені пристрою в Home Assistant', 'placeholder:"Clock Informer"', 'placeholder:"' + NAME + '"'),
]


def replace_changelog(html):
    """Вбудована історія версій (масив Nf) — автора 2.7.1 і раніше → історія цього проєкту (update/changelog.json):
    інакше інтерфейс показував би версії 2.x автора «новішими» за наші."""
    i = html.index('const Nf=[{v:')
    j = html.index('[', i)
    depth, k = 0, j
    in_str = None
    while True:
        ch = html[k]
        if in_str:
            if ch == '\\': k += 2; continue
            if ch == in_str: in_str = None
        elif ch in '"\'`': in_str = ch
        elif ch == '[': depth += 1
        elif ch == ']':
            depth -= 1
            if depth == 0: break
        k += 1
    ours = json.dumps(json.load(open(CHANGELOG, encoding='utf-8')), ensure_ascii=False, separators=(',', ':'))
    return html[:j] + ours + html[k + 1:]


def main():
    html = gzip.decompress(open(ORIG, 'rb').read()).decode('utf-8')
    for p in PATCHES:
        desc, old, new = p[0], p[1], p[2]
        want = p[3] if len(p) > 3 else 1
        n = html.count(old)
        if n != want:
            sys.exit(f'правка «{desc}»: входжень {n}, очікувалось {want}')
        html = html.replace(old, new)
        print('правка:', desc)
    html = replace_changelog(html)
    print('правка: вбудована історія версій — з update/changelog.json')
    left = sorted(set(re.findall(r'https?://(?:www\.)?goldart\.ee[^"\'` )]*', html)))
    if left:
        sys.exit('лишились адреси сервера автора: ' + ', '.join(left))
    data = html.encode('utf-8')
    gz = gzip.compress(data, compresslevel=9, mtime=0)
    os.makedirs(OUTD, exist_ok=True)
    open(os.path.join(OUTD, 'index.html'), 'wb').write(data)
    open(os.path.join(OUTD, 'index.html.gz'), 'wb').write(gz)
    etag = hashlib.sha256(gz).hexdigest()[:10]
    s = open(ASSETS, encoding='utf-8').read()
    s = re.sub(r'const char INDEX_HTML_ETAG\[\] = [^\n]*\n', '', s)
    arr = ',\n'.join('  ' + ','.join('0x%02x' % b for b in gz[i:i + 30]) for i in range(0, len(gz), 30))
    s, k = re.subn(r'// INDEX_HTML_GZ[^\n]*\nconst uint8_t INDEX_HTML_GZ\[\d+\] PROGMEM = \{.*?\};\n',
                   lambda m: f'// INDEX_HTML_GZ — оригінал автора (209660 Б) з правками web/webui_patch.py, {len(gz)} байт\n'
                             f'const uint8_t INDEX_HTML_GZ[{len(gz)}] PROGMEM = {{\n{arr}\n}};\n'
                             f'const char INDEX_HTML_ETAG[] = "\\"f{etag}\\"";\n', s, flags=re.S)
    if k != 1:
        sys.exit('web_assets.h: масив INDEX_HTML_GZ не знайдено')
    s = re.sub(r'^// \(оригінал[^\n]*\n', '', s, flags=re.M)
    s = re.sub(r'^// Статичні файли веб-інтерфейсу[^\n]*\n',
               '// Статичні файли веб-інтерфейсу з прошивки автора 2.7.1 (gzip). INDEX_HTML_GZ генерує web/webui_patch.py\n'
               '// (оригінал + правки) — не редагувати вручну; решта — байт у байт.\n', s, count=1, flags=re.M)
    open(ASSETS, 'w', encoding='utf-8').write(s)
    print(f'index.html.gz: {len(gz)} байт (оригінал 209660), ETag "f{etag}"')


main()
