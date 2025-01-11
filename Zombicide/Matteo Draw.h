#pragma once
#include "dati.h"

void m_draw_bitmap(MATTEO_BITMAP* m_b, float dx, float dy, int flags);
void m_draw_scaled_bitmap(MATTEO_BITMAP* m_b, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags);
void m_draw_bitmap_region(MATTEO_BITMAP* m_b, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void m_draw_tinted_bitmap(MATTEO_BITMAP* m_b, ALLEGRO_COLOR tint, float dx, float dy, int flags);
void m_draw_tinted_bitmap_region(MATTEO_BITMAP* m_b, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void m_draw_rotated_bitmap(MATTEO_BITMAP* m_b, float cx, float cy, float dx, float dy, float angle, int flags);
void m_draw_scaled_rotated_bitmap(MATTEO_BITMAP* m_b, float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags);
bool m_save_bitmap(string filename, MATTEO_BITMAP* immagine);