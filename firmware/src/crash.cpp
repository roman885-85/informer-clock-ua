// Запис причини аварії (нове, не з оригіналу).
// EN: Crash record (new): panic/exception cause, PC and backtrace saved to RTC memory and written to the event log after reboot.
// Ядро друкує причину аварії й адреси викликів лише в USB-порт; після перезапуску в журналі подій автора лишається
// тільки «CRASH (panic/exception)». Обробники аварій ядра підмінено лінкером (-Wl,--wrap=xt_unhandled_exception,
// panicHandler): перед штатною обробкою вони записують у RTC-пам'ять (переживає перезапуск) причину, адресу команди,
// адресу доступу й до 8 адрес ланцюжка викликів; setup() переносить запис у журнал подій. Код виконується в стані
// аварії (кеш флеш-пам'яті може бути вимкнений), тож лише IRAM і RTC-пам'ять, без викликів.
#include "fw.h"
#include "freertos/xtensa_context.h"

RTC_NOINIT_ATTR FxCrash rtc_crash;
String g_fx_crash_last;

static inline uint32_t IRAM_ATTR pc_fix(uint32_t pc) {      // адреса повернення → адреса виклику (як esp_cpu_process_stack_pc)
  if (pc & 0x80000000) pc = (pc & 0x3fffffff) | 0x40000000;
  return pc - 3;
}
static inline bool IRAM_ATTR sp_ok(uint32_t sp) { return sp >= 0x3ffae010 && sp <= 0x3ffffff0 && (sp & 0xf) == 0; }
static inline bool IRAM_ATTR pc_ok(uint32_t pc) { return pc >= 0x40000000 && pc < 0x40400000; }

static void IRAM_ATTR record(XtExcFrame *f, uint32_t kind) {
  if (rtc_crash.magic == FX_CRASH_MAGIC) return;             // друга аварія в обробці першої — лишаємо першу
  rtc_crash.kind = kind;
  rtc_crash.cause = f->exccause;
  rtc_crash.pc = f->pc;
  rtc_crash.vaddr = f->excvaddr;
  rtc_crash.up = g_fx_alive_ms / 1000;                       // лише читання пам'яті — без таймерів і блокувань
  uint32_t sp = f->a1, next = f->a0, n = 0;
  rtc_crash.bt[n++] = f->pc;
  while (n < 8 && sp_ok(sp)) {
    uint32_t *base = (uint32_t *)sp;
    uint32_t pc = next;
    next = base[-4];
    sp = base[-3];
    if (pc == 0 || !pc_ok(pc_fix(pc))) break;
    rtc_crash.bt[n++] = pc_fix(pc);
  }
  rtc_crash.n = n;
  rtc_crash.magic = FX_CRASH_MAGIC;
}

extern "C" void __real_xt_unhandled_exception(XtExcFrame *f);
extern "C" void IRAM_ATTR __wrap_xt_unhandled_exception(XtExcFrame *f) { record(f, 1); __real_xt_unhandled_exception(f); }
extern "C" void __real_panicHandler(XtExcFrame *f);
extern "C" void IRAM_ATTR __wrap_panicHandler(XtExcFrame *f) { record(f, 2); __real_panicHandler(f); }

// текст запису для журналу подій і сетевого журналу; порожньо — запису немає
void fx_crash_text(String &out) {
  out = "";
  if (rtc_crash.magic != FX_CRASH_MAGIC) return;
  static const char *const EXC[30] = { "IllegalInstruction", "Syscall", "InstructionFetchError", "LoadStoreError",
    "Level1Interrupt", "Alloca", "IntegerDivideByZero", "PCValue", "Privileged", "LoadStoreAlignment", "", "",
    "InstrPIFDataError", "LoadStorePIFDataError", "InstrPIFAddrError", "LoadStorePIFAddrError", "InstTLBMiss",
    "InstTLBMultiHit", "InstFetchPrivilege", "", "InstrFetchProhibited", "", "", "", "LoadStoreTLBMiss",
    "LoadStoreTLBMultihit", "LoadStorePrivilege", "", "LoadProhibited", "StoreProhibited" };
  static const char *const PAN[8] = { "unknown", "debug exception (abort/stack watchpoint)", "double exception",
    "unhandled kernel exception", "coprocessor exception", "interrupt watchdog CPU0", "interrupt watchdog CPU1",
    "cache error (flash cache disabled)" };
  char b[48];
  if (rtc_crash.kind == 1) out = String("exception ") + (rtc_crash.cause < 30 ? EXC[rtc_crash.cause] : "?");
  else out = String("panic: ") + (rtc_crash.cause < 8 ? PAN[rtc_crash.cause] : "?");
  snprintf(b, sizeof b, " (%u) PC=0x%08x addr=0x%08x", (unsigned)rtc_crash.cause, (unsigned)rtc_crash.pc, (unsigned)rtc_crash.vaddr);
  out += b;
  out += " bt:";
  for (uint32_t i = 0; i < rtc_crash.n && i < 8; i++) { snprintf(b, sizeof b, " 0x%08x", (unsigned)rtc_crash.bt[i]); out += b; }
  snprintf(b, sizeof b, " up=%us", (unsigned)rtc_crash.up);
  out += b;
}
