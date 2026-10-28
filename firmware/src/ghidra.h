// Типи з декомпіляції Ghidra — щоб перенесений код зберігав ті самі перетворення типів, що й оригінал
// EN: Types used by the reconstruction (keeps the same type conversions as the original machine code).
#pragma once
#include <stdint.h>
typedef uint32_t uint;
typedef uint16_t ushort;
typedef unsigned long long ulonglong;
typedef long long longlong;
typedef uint8_t undefined1;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef uint64_t undefined8;
