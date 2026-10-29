// Ідентичність проєкту Informer Clock UA (нове, не з оригіналу).
// EN: Project identity: name, version, repository URLs for updates and language packs.
// Окремий незалежний проєкт на основі Clock Informer 2.7.1 автора Semen_p (https://www.thingiverse.com/thing:3924947,
// ліцензія CC BY-NC 4.0). Усе, що прошивка й веб-інтерфейс беруть із мережі (оновлення, мовні пакети), — з репозиторію
// цього проєкту, а не з сервера автора: версії двох проєктів не перетинаються.
// Project identity (new, not in the original): an independent fork of Clock Informer 2.7.1 by Semen_p (CC BY-NC 4.0);
// updates and language packs come from this project's repository, never from the original author's server.
#pragma once

#define FX_NAME "Informer Clock UA"
#define FX_VERSION "1.1.0-rc4"
#define FX_REPO_URL "https://github.com/roman885-85/informer-clock-ua"
#define FX_RAW_URL "https://raw.githubusercontent.com/roman885-85/informer-clock-ua/main"
#define FX_UPDATE_URL FX_RAW_URL "/update/esp32.json"   // опис останньої версії: {"ver","bin","size","md5",…}
#define FX_LANG_URL FX_RAW_URL "/lang"                  // мовні пакети табло: /lang/<id>.led

// чи версія a («1.2.3») новіша за b
bool fx_version_newer(const String &a, const String &b);
