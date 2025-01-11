#include "Matteo Draw.h"

using namespace std;


void m_draw_bitmap(MATTEO_BITMAP* m_b,float dx, float dy, int flags)
{
	m_b->rigenerate_bitmap();
	al_draw_bitmap(m_b->immagine, dx, dy, flags);
	m_b->add_to_immagini_da_distruggere();
}
void m_draw_scaled_bitmap(MATTEO_BITMAP* m_b,float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags)
{
	m_b->rigenerate_bitmap();
	al_draw_scaled_bitmap(m_b->immagine, sx, sy, sw, sh, dx, dy, dw, dh, flags);
	m_b->add_to_immagini_da_distruggere();
}
void m_draw_bitmap_region(MATTEO_BITMAP* m_b, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	m_b->rigenerate_bitmap();
	al_draw_bitmap_region(m_b->immagine, sx, sy, sw, sh, dx, dy, flags);
	m_b->add_to_immagini_da_distruggere();
}
void m_draw_tinted_bitmap(MATTEO_BITMAP* m_b, ALLEGRO_COLOR tint, float dx, float dy, int flags)
{
	m_b->rigenerate_bitmap();
	al_draw_tinted_bitmap(m_b->immagine, tint, dx, dy, flags);
	m_b->add_to_immagini_da_distruggere();
}

void m_draw_scaled_rotated_bitmap(MATTEO_BITMAP* m_b, float cx, float cy, float dx,float dy,float xscale, float yscale,float angle, int flags)
{
	m_b->rigenerate_bitmap();
	al_draw_scaled_rotated_bitmap(m_b->immagine, cx, cy, dx, dy, xscale, yscale, angle, flags);
	m_b->add_to_immagini_da_distruggere();
}
void m_draw_rotated_bitmap(MATTEO_BITMAP* m_b, float cx, float cy, float dx, float dy, float angle, int flags)
{
	m_b->rigenerate_bitmap();
	al_draw_rotated_bitmap(m_b->immagine, cx, cy, dx, dy, angle, flags);
	m_b->add_to_immagini_da_distruggere();
}
void m_draw_tinted_bitmap_region(MATTEO_BITMAP* m_b,ALLEGRO_COLOR tint,float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	m_b->rigenerate_bitmap();
	al_draw_tinted_bitmap_region(m_b->immagine, tint, sx, sy, sw, sh, dx, dy, flags);
	m_b->add_to_immagini_da_distruggere();
}

bool m_save_bitmap(string filename, MATTEO_BITMAP* immagine)
{
	immagine->rigenerate_bitmap();
	bool risposta=false;
	risposta= al_save_bitmap(filename.c_str(), immagine->immagine);
	return risposta;
}