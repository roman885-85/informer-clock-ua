void colon_timing(int8_t ph) {
  switch (g_c2eed) {
    case 0: break;
    case 1:
L1:   g_c2ee4 = 1; g_c349e = 1000; return;
    case 2:
      if (g_c349c < 6 && ((1 << (g_c349c & 0x1f)) & 0x2d) != 0) break;
      g_c2ee4 = 3;
      if (ph == 1) { g_c349e = 200; return; }
      if (ph != 3) { g_c349e = 0x96; return; }
      goto L500;
    case 3:
      if (5 < g_c349c || ((1 << (g_c349c & 0x1f)) & 0x2d) == 0) {
        g_c2ee4 = 5;
        if (ph != 0) { g_c349e = 100; return; }
        goto L500;
      }
      break;
    case 4:
      if (5 < g_c349c || ((1 << (g_c349c & 0x1f)) & 0x2d) == 0) goto L1;
      break;
    default: return;
  }
  g_c2ee4 = 1;
L500:
  g_c349e = 500;
}
