/*
#pragma once
#include "dati.h"

extern class MATTEO_BITMAP;

class Draw_Madre
{
public:
	bool drawable;
	Draw_Madre();
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator ==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim);
};
class Draw_classic:public Draw_Madre
{
private:
	MATTEO_BITMAP* immagine;
	float dx, dy;
	int flags;
public:
	Draw_classic(MATTEO_BITMAP* immagine, float dx, float dy, int flags);
	Draw_classic();
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim);
 };
class Draw_scaled :public Draw_Madre
{
private:
	MATTEO_BITMAP* immagine;
	float sx, sy, sw, sh, dx, dy, dw, dh;
	int flags;
public:
	Draw_scaled();
	Draw_scaled(MATTEO_BITMAP* immagine, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags);
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim);
};
class Draw_region :public Draw_Madre
{
private:
	MATTEO_BITMAP* immagine;
	float sx, sy, sw, sh, dx, dy;
	int flags;
public:
	Draw_region();
	Draw_region(MATTEO_BITMAP* immagine, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim);
};
class Draw_filled_rectangle:public Draw_Madre
{
private:
	float x1, y1, x2, y2;
	ALLEGRO_COLOR color;
public:
	Draw_filled_rectangle();
	Draw_filled_rectangle(float x1,float y1, float x2, float y2, ALLEGRO_COLOR color);
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim);
};
class Draw_tinted :public Draw_Madre
{
private:
	MATTEO_BITMAP*immagine;
	ALLEGRO_COLOR tint;
	float dx, dy;
	int flags;
public:
	Draw_tinted();
	Draw_tinted(MATTEO_BITMAP*immagine, ALLEGRO_COLOR tint, float dx, float dy, int flags);
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int &dx_dim, int &dy_dim);
};
class Draw_scaled_rotated :public Draw_Madre
{
private:
	MATTEO_BITMAP* immagine;
	float cx, cy, dx, dy, xscale, yscale, angle;
	int flags;
public:
	Draw_scaled_rotated();
	Draw_scaled_rotated(MATTEO_BITMAP* immagine,float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags);
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int& dx_dim, int &dy_dim);
};
class Draw_rotated :public Draw_Madre
{
private:
	MATTEO_BITMAP* immagine;
	float cx, cy, dx, dy, angle;
	int flags;
public:
	Draw_rotated();
	Draw_rotated(MATTEO_BITMAP* immagine, float cx, float cy, float dx, float dy, float angle, int flags);
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int& dx_dim, int &dy_dim);
};
class Draw_tinted_region :public Draw_Madre
{
private:
	MATTEO_BITMAP* immagine;
	ALLEGRO_COLOR tint;
	float sx, sy, sw, sh, dx, dy;
	int flags;
public:
	Draw_tinted_region();
	Draw_tinted_region(MATTEO_BITMAP* immagine, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float  dy, int flags);
	virtual void Draw(MATTEO_BITMAP* target);
	virtual void anti_draw(MATTEO_BITMAP* target);
	virtual bool operator==(Draw_Madre* altro);
	virtual void get_d_cordinate(int &dx, int &dy, int& dx_dim, int &dy_dim);
};
*/
