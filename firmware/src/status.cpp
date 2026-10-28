// Стан на табло, поки годинник чекає мережу під час запуску (нове, не з оригіналу).
// EN: Startup status on the display (new): animated icon + scrolling text while the clock waits for the network.
// Задача: після ввімкнення й під'єднання до точки доступу табло було чорним — показувати, що відбувається.
// У автора між під'єднанням до Wi-Fi і першим вмістом годинник синхронізує час, перевіряє
// інтернет і оновлення, вантажить мову й перші дані — при порожньому табло. Тепер на час таких очікувань (якщо вони
// довші за FX_STATUS_DELAY_MS): на двох поверхах зверху — значок із рухом, знизу — бігучий напис мовою табло; на
// одному поверсі — напис. Анімацію крутить окреме завдання FreeRTOS, поки головний цикл чекає мережу; табло воно
// чіпає лише між fx_status_begin() і fx_status_end(), коли головний цикл табло не торкається.
#include "fw.h"

// Значки 7×8, кадри анімації (tools/sim/status_icons.py): спінер, годинник, глобус, стрілка в лоток
static const uint8_t PROGMEM font_fx_status[] = {
  'F', 1, 0x01, 0x18, 8,
  7, 0x1c, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00,  // 0x01 спінер
  7, 0x04, 0x02, 0x01, 0x01, 0x01, 0x02, 0x00,  // 0x02 спінер
  7, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x0c,  // 0x03 спінер
  7, 0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x1c,  // 0x04 спінер
  7, 0x00, 0x00, 0x00, 0x40, 0x40, 0x20, 0x1c,  // 0x05 спінер
  7, 0x00, 0x20, 0x40, 0x40, 0x40, 0x20, 0x10,  // 0x06 спінер
  7, 0x18, 0x20, 0x40, 0x40, 0x40, 0x00, 0x00,  // 0x07 спінер
  7, 0x1c, 0x22, 0x40, 0x00, 0x00, 0x00, 0x00,  // 0x08 спінер
  7, 0x1c, 0x22, 0x41, 0x4f, 0x41, 0x22, 0x1c,  // 0x09 годинник
  7, 0x1c, 0x22, 0x41, 0x49, 0x45, 0x22, 0x1c,  // 0x0a годинник
  7, 0x1c, 0x22, 0x41, 0x49, 0x49, 0x2a, 0x1c,  // 0x0b годинник
  7, 0x1c, 0x22, 0x41, 0x49, 0x51, 0x22, 0x1c,  // 0x0c годинник
  7, 0x1c, 0x22, 0x41, 0x79, 0x41, 0x22, 0x1c,  // 0x0d годинник
  7, 0x1c, 0x22, 0x51, 0x49, 0x41, 0x22, 0x1c,  // 0x0e годинник
  7, 0x1c, 0x2a, 0x49, 0x49, 0x41, 0x22, 0x1c,  // 0x0f годинник
  7, 0x1c, 0x22, 0x45, 0x49, 0x41, 0x22, 0x1c,  // 0x10 годинник
  7, 0x1c, 0x2a, 0x49, 0x7f, 0x49, 0x2a, 0x1c,  // 0x11 глобус
  7, 0x1c, 0x2a, 0x7f, 0x49, 0x7f, 0x2a, 0x1c,  // 0x12 глобус
  7, 0x1c, 0x3e, 0x6b, 0x49, 0x6b, 0x3e, 0x1c,  // 0x13 глобус
  7, 0x1c, 0x2a, 0x7f, 0x49, 0x7f, 0x2a, 0x1c,  // 0x14 глобус
  7, 0xe0, 0x82, 0x84, 0x8f, 0x84, 0x82, 0xe0,  // 0x15 завантаження
  7, 0xe0, 0x84, 0x88, 0x9f, 0x88, 0x84, 0xe0,  // 0x16 завантаження
  7, 0xe0, 0x88, 0x90, 0xbe, 0x90, 0x88, 0xe0,  // 0x17 завантаження
  7, 0xe0, 0x90, 0xa0, 0xfc, 0xa0, 0x90, 0xe0,  // 0x18 завантаження
};

struct FxIcon { uint8_t first, n; uint16_t ms; };
static const FxIcon ICONS[] = { { 0x01, 8, 90 }, { 0x09, 8, 125 }, { 0x11, 4, 220 }, { 0x15, 4, 180 } };

struct FxText { uint8_t icon; const char *uk, *ru, *en; };
static const FxText TEXTS[] = {
  { 0, "Запуск", "Запуск", "Starting" },
  { 1, "Синхронізація часу", "Синхронизация времени", "Time sync" },
  { 2, "Перевірка інтернету", "Проверка интернета", "Checking internet" },
  { 3, "Перевірка оновлень", "Проверка обновлений", "Checking for updates" },
  { 3, "Завантаження мови табло", "Загрузка языка табло", "Loading display language" },
  { 3, "Завантаження погоди", "Загрузка погоды", "Loading weather" },
  { 3, "Завантаження новин", "Загрузка новостей", "Loading news" },
  { 3, "Завантаження курсів", "Загрузка курсов", "Loading exchange rates" },
  { 3, "Завантаження даних", "Загрузка данных", "Loading data" },
  { 3, "Оновлення прошивки", "Обновление прошивки", "Updating firmware" },
};

bool g_fx_booting = true;
static SemaphoreHandle_t s_mx;
static TaskHandle_t s_task;
static volatile bool s_on;          // триває очікування: табло належить завданню
static bool s_drawn;                // завдання вже перебрало табло (лише під s_mx)
static uint8_t s_kind;
static uint32_t s_t0;
static char s_txt[160];             // напис у кодах шрифту табло (MD_Parola тримає вказівник)
static char s_ico[2];               // поточний кадр значка
static MD_MAX72XX::fontType_t *s_f0, *s_f1;
static uint8_t s_cs0, s_cs1;
static uint8_t s_depth;             // вкладені fx_status_begin/end — показує зовнішній

static bool fx_two_rows() { return g_c34ba == 8 && P.getZoneCount() > 1; }

static const char *fx_text(uint8_t kind) {
  const FxText &t = TEXTS[kind];
  if (g_c2eef == 3) return t.uk;
  if (g_c2eef == 0 || g_c2eef == 2) return t.ru;   // російська, білоруська
  return t.en;
}

// перебрати табло: запам'ятати шрифти й інтервали зон, напис — у зону 0 (низ), значок — у зону 1 (верх)
static void fx_draw_start() {
  bool two = fx_two_rows();
  s_f0 = P.getFont(0); s_cs0 = P.getCharSpacing(0);
  if (two) { s_f1 = P.getFont(1); s_cs1 = P.getCharSpacing(1); }
  display_clear_all();
  g_c34a3 = 0xfe;                   // шрифт напису — той, яким автор пише текст (мова, fs)
  sys_font_apply();
  P.setCharSpacing(0, 1);
  int spd = 0x46 - (int)g_bf04d;
  if (spd < 10) spd = 10;
  P.displayZoneText(0, s_txt, PA_LEFT, spd, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  if (two) {
    P.setFont(1, (MD_MAX72XX::fontType_t *)font_fx_status);
    P.setCharSpacing(1, 0);
  }
}

static void fx_task(void *) {
  uint8_t fr = 0;
  uint32_t tf = 0;
  for (;;) {
    if (!s_on) { ulTaskNotifyTake(pdTRUE, portMAX_DELAY); continue; }
    vTaskDelay(pdMS_TO_TICKS(8));
    xSemaphoreTake(s_mx, portMAX_DELAY);
    if (s_on) {
      uint32_t now = millis();
      if (!s_drawn && now - s_t0 >= FX_STATUS_DELAY_MS) { fx_draw_start(); s_drawn = true; fr = 0; tf = now - 1000; }
      if (s_drawn) {
        const FxIcon &ic = ICONS[TEXTS[s_kind].icon];
        if (fx_two_rows() && now - tf >= ic.ms) {
          tf = now;
          s_ico[0] = ic.first + fr;
          s_ico[1] = 0;
          fr = (fr + 1) % ic.n;
          P.displayZoneText(1, s_ico, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
        }
        P.displayAnimate();
        if (P.getZoneStatus(0)) P.displayReset(0);   // напис — по колу
      }
    }
    xSemaphoreGive(s_mx);
  }
}

void fx_status_begin(uint8_t kind, bool always) {
  if (s_depth++ != 0) return;
  if (!always) {
    if (g_fx_booting && millis() > FX_BOOT_MAX_MS) g_fx_booting = false;
    if (!g_fx_booting) return;
  }
  if (s_mx == nullptr) {
    s_mx = xSemaphoreCreateMutex();
    if (s_mx == nullptr) return;
    if (xTaskCreatePinnedToCore(fx_task, "fxstatus", 4096, nullptr, 2, &s_task, 1) != pdPASS) { s_task = nullptr; return; }
  }
  if (s_task == nullptr || s_on) return;
  xSemaphoreTake(s_mx, portMAX_DELAY);
  s_kind = kind;
  memset(s_txt, 0, sizeof s_txt);
  utf8_to_led(String(fx_text(kind)), s_txt, sizeof s_txt - 1);
  s_t0 = millis();
  s_drawn = false;
  s_on = true;
  xSemaphoreGive(s_mx);
  xTaskNotifyGive(s_task);
}

void fx_status_end() {
  if (s_depth == 0 || --s_depth != 0) return;
  if (s_mx == nullptr || !s_on) return;
  xSemaphoreTake(s_mx, portMAX_DELAY);
  s_on = false;
  if (s_drawn) {
    s_drawn = false;
    bool two = fx_two_rows();
    // зони — у стан «показ завершено» з порожнім текстом, щоб код автора не дочікувався кінця нашого напису
    P.displayZoneText(0, "", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
    if (two) P.displayZoneText(1, "", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
    for (uint8_t i = 0; i < 20 && !(P.getZoneStatus(0) && (!two || P.getZoneStatus(1))); i++) P.displayAnimate();
    display_clear_all();
    P.setFont(0, s_f0); P.setCharSpacing(0, s_cs0);
    if (two) { P.setFont(1, s_f1); P.setCharSpacing(1, s_cs1); }
    g_c34a3 = 0xfe;                 // код автора сам перевстановить свої шрифти
    g_c34a4 = 99;
    // циферблати — «ще не намальовано», щоб код автора перемалював їх повністю (як після повідомлення, f_400e3e5c):
    // годинник зі зсувом малює лише цифри, що змінились, великий і малий — ставлять текст лише на першому кроці
    g_c34a0 = 0; g_c34a1 = 0; g_c34a2 = 0;
    g_c34a6 = 0; g_c34a7 = 0; g_c34a8 = 0;
  }
  xSemaphoreGive(s_mx);
}
