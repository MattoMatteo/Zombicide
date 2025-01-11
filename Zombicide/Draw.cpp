
/*
#include "Draw.h"
#include "dati.h"
#include <math.h>
#include"Matteo Draw.h"
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>


//Classe Madre: Draw_Madre
Draw_Madre::Draw_Madre():drawable(true){}
void Draw_Madre::Draw(MATTEO_BITMAP* target)
{
	;//Non la userò mai quella della madre.
}
void Draw_Madre::anti_draw(MATTEO_BITMAP* target)
{
	;
}
bool Draw_Madre::operator==(Draw_Madre* altro)
{
	//non lo userò mai perchè userò sempre i figli
	return false;
}
void Draw_Madre::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	;//niente perchè tanto uso sempre i figli
}
//Classe figlia: Draw_Classic
Draw_classic::Draw_classic(): Draw_Madre(),immagine(NULL),dx(0),dy(0),flags(0) {}
Draw_classic::Draw_classic(MATTEO_BITMAP* immagine, float dx, float dy, int flags) :
	Draw_Madre(), immagine(immagine),dx(dx), dy(dy), flags(flags) {}
void Draw_classic::Draw(MATTEO_BITMAP* target)
{
	immagine->Draw();
	m_set_target_bitmap(target);
	m_draw_bitmap(immagine, dx, dy, flags);
}
void Draw_classic::anti_draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	int x1, y1, x_dim, y_dim;
	this->get_d_cordinate(x1, y1, x_dim, y_dim);
	int x2 = x_dim + x1;
	int y2 = y_dim + y1;
	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(0, 0, 0));
}
bool Draw_classic::operator==(Draw_Madre* altro)
{
	Draw_classic* p=dynamic_cast<Draw_classic*>(altro);
	if (p != NULL) //il cast è andato a buon fine, quindi il draw madre è in realtà di tipo classic ed è comparabile
		if (this->dx == p->dx && this->dy == p->dy && this->flags == p->flags && this->immagine->compare(p->immagine))
			return true;
	return false;
}
void Draw_classic::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	dx = this->dx;
	dy = this->dy;
	dx_dim = this->immagine->width;
	dy_dim = this->immagine->height;
}
//Classe figlia: Draw_scaled
Draw_scaled::Draw_scaled() :Draw_Madre(),immagine(NULL), sx(0), sy(0), sw(0), sh(0), dx(0), dy(0), dw(0), dh(0), flags(0) {}
Draw_scaled::Draw_scaled(MATTEO_BITMAP* immagine, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags) :
	Draw_Madre(), immagine(immagine),sx(sx), sy(sy), sw(sw), sh(sh), dx(dx), dy(dy), dw(dw), dh(dh), flags(flags) {}
void Draw_scaled::Draw(MATTEO_BITMAP* target)
{
	immagine->Draw();
	m_set_target_bitmap(target);
	m_draw_scaled_bitmap(immagine, sx, sy, sw, sh, dx, dy, dw, dh, flags);
}
void Draw_scaled::anti_draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	int x1, y1, x_dim, y_dim;
	this->get_d_cordinate(x1, y1, x_dim, y_dim);
	int x2 = x_dim + x1;
	int y2 = y_dim + y1;
	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(0, 0, 0, 0));
}
void Draw_scaled::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	dx = this->dx;
	dy = this->dy;
	dx_dim = this->dw;
	dy_dim = this->dh;
}
bool Draw_scaled::operator==(Draw_Madre* altro)
{
	Draw_scaled* p = dynamic_cast<Draw_scaled*>(altro);
	if (p != NULL) //il cast è andato a buon fine, quindi il draw madre è in realtà di tipo scaled ed è comparabile
		if (this->dx == p->dx && this->dy == p->dy && this->dw == p->dw && this->dh == p->dh && this->sx == p->sx &&
			this->sy == p->sy && this->sw == p->sw && this->sh == p->sh && this->flags == p->flags && this->immagine->compare(p->immagine))
			return true;
	return false;
}
//Classe figlia: Draw_region
Draw_region::Draw_region():Draw_Madre(),immagine(NULL), sx(0), sy(0), sw(0), sh(0), dx(0), dy(0), flags(0) {}
Draw_region::Draw_region(MATTEO_BITMAP* immagine, float sx, float sy, float sw, float sh, float dx, float dy, int flags) :
	Draw_Madre(), immagine(immagine), sx(sx), sy(sy), sw(sw), sh(sh), dx(dx), dy(dy), flags(flags) {}
void Draw_region::Draw(MATTEO_BITMAP* target)
{
	immagine->Draw();
	m_set_target_bitmap(target);
	m_draw_bitmap_region(immagine, sx, sy, sw, sh, dx, dy, flags);
}
void Draw_region::anti_draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	int x1, y1, x_dim, y_dim;
	this->get_d_cordinate(x1, y1, x_dim, y_dim);
	int x2 = x_dim + x1;
	int y2 = y_dim + y1;
	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(0, 0, 0, 0));
}
bool Draw_region::operator==(Draw_Madre* altro)
{
	Draw_region* p = dynamic_cast<Draw_region*>(altro);
	if (p != NULL)
		if (this->sx == p->sx && this->sy == p->sy && this->sw == p->sw && this->sh == p->sh &&
			this->dx == p->dx && this->dy == p->dy && this->flags==p->flags && this->immagine->compare(p->immagine))
			return true;
	return false;
}
void Draw_region::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	dx = this->dx;
	dy = this->dy;
	dx_dim = this->sw;
	dy_dim = this->sh;
}
//Classe figlia: Draw_filled_rectangle
Draw_filled_rectangle::Draw_filled_rectangle() :Draw_Madre(),x1(0), y1(0), x2(0), y2(0), color(al_map_rgba(0, 0, 0, 0)) {};
Draw_filled_rectangle::Draw_filled_rectangle(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color) :
	Draw_Madre(), x1(x1), y1(y1), x2(x2), y2(y2), color(color) {}
void Draw_filled_rectangle::Draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	al_draw_filled_rectangle(x1, y1, x2, y2, color);
}
void Draw_filled_rectangle::anti_draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(0, 0, 0, 0));
}
bool Draw_filled_rectangle::operator==(Draw_Madre* altro)
{
	Draw_filled_rectangle *p= dynamic_cast<Draw_filled_rectangle*>(altro);
	if (p != NULL)
		if (this->x1 == p->x1&&this->y1 == p->y1&&this->x2 == p->x2&&this->y2 == p->y2&&
			this->color.a == p->color.a&&this->color.b == p->color.b&&this->color.g == p->color.g&&this->color.r == p->color.r)
			return true;
	return false;
}
void Draw_filled_rectangle::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	dx = this->x1;
	dy = this->y1;
	dx_dim = this->x1+x2;
	dy_dim = this->y1 + y2;
}
//Classe figlia: Draw_tinted
Draw_tinted::Draw_tinted() : Draw_Madre(), immagine(NULL), tint(al_map_rgba(0, 0, 0, 0)), dx(0), dy(0), flags(0) {}
Draw_tinted::Draw_tinted(MATTEO_BITMAP*immagine, ALLEGRO_COLOR tint, float dx, float dy, int flags) :
	Draw_Madre(), immagine(immagine), tint(tint), dx(dx), dy(dy), flags(flags) {}
void Draw_tinted::Draw(MATTEO_BITMAP* target)
{
	immagine->Draw();
	m_set_target_bitmap(target);
	m_draw_tinted_bitmap(immagine, tint, dx, dy, flags);
}
void Draw_tinted::anti_draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	int x1, y1, x_dim, y_dim;
	this->get_d_cordinate(x1, y1, x_dim, y_dim);
	int x2 = x_dim + x1;
	int y2 = y_dim + y1;
	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(0, 0, 0, 0));
}
bool Draw_tinted::operator==(Draw_Madre* altro)
{
	Draw_tinted* p = dynamic_cast<Draw_tinted*>(altro);
	if (p != NULL)
		if (this->tint.a == p->tint.a&&this->tint.b == p->tint.b&&this->tint.r == p->tint.r&&this->tint.g == p->tint.g&&
			this->dx == p->dx && this->dy == p->dy && this->flags == p->flags && this->immagine->compare(p->immagine))
			return true;
	return false;
}
void Draw_tinted::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	dx = this->dx;
	dy = this->dy;
	dx_dim = this->immagine->width;
	dy_dim = this->immagine->height;
}
//Classe figlia: Draw_scaled_rotated
Draw_scaled_rotated::Draw_scaled_rotated() :Draw_Madre(), immagine(NULL), cx(0), cy(0), dx(0), dy(0), xscale(0), yscale(0), angle(0), flags(0) {}
Draw_scaled_rotated::Draw_scaled_rotated(MATTEO_BITMAP* immagine,float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags):
	Draw_Madre(), immagine(immagine), cx(cx), cy(cy), dx(dx), dy(dy), xscale(xscale), yscale(yscale), angle(angle), flags(flags){}
void Draw_scaled_rotated::Draw(MATTEO_BITMAP* target)
{
	immagine->Draw();
	m_set_target_bitmap(target);
	m_draw_scaled_rotated_bitmap(immagine, cx, cy, dx, dy, xscale, yscale, angle, flags);
}
void Draw_scaled_rotated::anti_draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	int x1, y1, x_dim, y_dim;
	this->get_d_cordinate(x1, y1, x_dim, y_dim);
	int x2 = x_dim + x1;
	int y2 = y_dim + y1;
	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(0, 0, 0, 0));
}
bool Draw_scaled_rotated::operator==(Draw_Madre* altro)
{
	Draw_scaled_rotated* p = dynamic_cast<Draw_scaled_rotated*>(altro);
	if (p != NULL)
		if (immagine->compare(p->immagine) && cx == p->cx && cy == p->cy && dx == p->dx && dy == p->dy && xscale == p->xscale && yscale == p->yscale &&
			angle == p->angle && flags == p->flags)
			return true;
	return false;
}
void Draw_scaled_rotated::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	//Ora credo che bisogna convertire tutte le variabili in base all'angolo. Ma in teoria dipende anche dal centro che gira
	//troppo difficile da capire... devo chiedere aiuto
	int xP = this->dx;
	int yP = this->dy;
	int distanza_dal_centro = sqrt(((xP - cx)*(xP - cx)) + ((yP - cy)*(yP - cy)));
	int x1 = distanza_dal_centro*cos(-angle) + cx;
	int y1 = distanza_dal_centro*sin(-angle) + cy;
	xP = this->dx + (this->immagine->width*xscale);
	yP = this->dy + (this->immagine->height*yscale);
	distanza_dal_centro = sqrt(((xP - cx)*(xP - cx)) + ((yP - cy)*(yP - cy)));
	int x2 = distanza_dal_centro*cos(-angle) + cx;
	int y2 = distanza_dal_centro*sin(-angle) + cy;

	if (x1 <= x2)
		dx = x1;
	else
		dx = x2;
	if (y1 <= y2)
		dy = y1;
	else
		dy = y2;
}
//Classe figlia: Draw_rotated
Draw_rotated::Draw_rotated() : Draw_Madre(), immagine(NULL), cx(0), cy(0), dx(0), dy(0), angle(0), flags(0) {}
Draw_rotated::Draw_rotated(MATTEO_BITMAP* immagine, float cx, float cy, float dx, float dy, float angle, int flags) :
	Draw_Madre(), immagine(immagine), cx(cx), cy(cy), dx(dx), dy(dy), angle(angle), flags(flags) {}
void Draw_rotated::Draw(MATTEO_BITMAP* target)
{
	immagine->Draw();
	m_set_target_bitmap(target);
	m_draw_rotated_bitmap(immagine, cx, cy, dx, dy, angle, flags);
}
void Draw_rotated::anti_draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	int x1, y1, x_dim, y_dim;
	this->get_d_cordinate(x1, y1, x_dim, y_dim);
	int x2 = x_dim + x1;
	int y2 = y_dim + y1;
	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(0, 0, 0, 0));
}
bool Draw_rotated::operator==(Draw_Madre* altro)
{
	Draw_rotated* p = dynamic_cast<Draw_rotated*>(altro);
	if (p != NULL)
		if (immagine->compare(p->immagine) && cx == p->cx && cy == p->cy && dx == p->dx && dy == p->dy &&
			angle == p->angle && flags == p->flags)
			return true;
	return false;
}
void Draw_rotated::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	//credo che bisogna convertire tutte le variabili in base all'angolo. Ma in teoria dipende anche dal centro che gira
	//troppo difficile da capire... devo chiedere aiuto
	int xP = this->dx;
	int yP = this->dy;
	int distanza_dal_centro = sqrt(((xP - cx)*(xP- cx)) + ((yP - cy)*(yP - cy)));
	int x1 = distanza_dal_centro*cos(-angle) + cx;
	int y1 = distanza_dal_centro*sin(-angle) + cy;
	xP = this->dx+this->immagine->width;
	yP = this->dy+this->immagine->height;
	distanza_dal_centro = sqrt(((xP - cx)*(xP - cx)) + ((yP - cy)*(yP - cy)));
	int x2 = distanza_dal_centro*cos(-angle) + cx;
	int y2 = distanza_dal_centro*sin(-angle) + cy;

	if (x1 <= x2)
		dx = x1;
	else
		dx = x2;
	if (y1 <= y2)
		dy = y1;
	else
		dy = y2;
}
//Classe figlia: Draw_tinted_region
Draw_tinted_region::Draw_tinted_region() :Draw_Madre(), tint(al_map_rgba(0, 0, 0, 0)), sx(0), sy(0), sw(0), sh(0), dx(0), dy(0), flags(0) {}
Draw_tinted_region::Draw_tinted_region(MATTEO_BITMAP* immagine, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float  dy, int flags) :
	Draw_Madre(), immagine(immagine), tint(tint), sx(sx), sy(sy), sw(sw), sh(sh), dx(dx), dy(dy), flags(flags) {}
void Draw_tinted_region::Draw(MATTEO_BITMAP* target)
{
	immagine->Draw();
	m_set_target_bitmap(target);
	m_draw_tinted_bitmap_region(immagine, tint, sx, sy, sw, sh, dx, dy, flags);
}
void Draw_tinted_region::anti_draw(MATTEO_BITMAP* target)
{
	m_set_target_bitmap(target);
	int x1, y1, x_dim, y_dim;
	this->get_d_cordinate(x1, y1, x_dim, y_dim);
	int x2 = x_dim + x1;
	int y2 = y_dim + y1;
	al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(0, 0, 0, 0));
}
bool Draw_tinted_region ::operator==(Draw_Madre* altro)
{
	Draw_tinted_region* p = dynamic_cast<Draw_tinted_region*>(altro);
	if (p != NULL)
		if (this->immagine->compare(p->immagine) && this->tint.a == p->tint.a&&this->tint.b == p->tint.b&&this->tint.r == p->tint.r&&this->tint.g == p->tint.g&&
			this->sx == p->sx&&this->sy == p->sy&& this->sw == p->sw&&this->sh == p->sh&&this->dx == p->dy&&this->flags == p->flags)
			return true;
	return false;
}
void Draw_tinted_region::get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim)
{
	dx = this->dx;
	dy = this->dy;
	dx_dim = this->sw;
	dy_dim = this->sh;
}
*/
