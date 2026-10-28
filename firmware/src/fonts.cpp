// Дані шрифтів і спрайтів: заголовки з `static` → зовнішні визначення (одна копія на всю прошивку)
// EN: Font and sprite data definitions (single copy for the whole firmware).
#include "fonts.h"
#define static
#include "fonts_2_7_1.h"
#include "sprites_2_7_1.h"
#undef static
