/*====================================================*\
  Arash HABIBI
  Image.h
\*====================================================*/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

#include "Ppm.h"

//-----------------------------------------------------

typedef struct
{
	float _red, _green, _blue;
} Color;

Color C_new(float red, float green, float blue);
void C_check(Color c, char *message);

//-----------------------------------------------------
typedef struct Coordonnees Coordonnees;
struct Coordonnees
{
    int x;
    int y;
};

//-----------------------------------------------------
typedef struct LigneBrisee
{
    Coordonnees p;
	struct LigneBrisee *suivant;
	struct LigneBrisee *precedent;

}sommet,*LigneBrisee;

//-----------------------------------------------------
typedef struct
{
	int _width, _height;
	int _xzoom, _yzoom;
	int _xoffset, _yoffset;
	double _zoom;
	Color _current_color;
	Color **_buffer;
} Image;

Image* I_new(int _width, int _height);
Image* I_read(char *ppmfilename);

void I_fill       (Image *img, Color c);
void I_checker    (Image *img, Color c1, Color c2, int step);

void I_changeColor(Image *img, Color c);
void I_plot       (Image *img, int x, int y);
void I_plotColor  (Image *img, int x, int y, Color c);

void I_focusPoint (Image *img, int x, int y);
void I_zoomInit   (Image *img);
void I_zoom       (Image *img, double zoom_coef);
void I_move       (Image *img, int x, int y);

//TODO
void I_bresenhamOrigin(Image*img, int x, int y);
void I_bresenham(Image*img, int xA, int yA, int xB, int yB);

//void I_ligneBrisee(Image*img,Coordonnees* coord,int taille);
void I_ligneBrisee(Image*img,LigneBrisee l);
void I_Polygone(Image*img,LigneBrisee l);

int determinant(Coordonnees m1, Coordonnees m2, Coordonnees m3);
int I_intersect(Coordonnees m1,Coordonnees m2,Coordonnees n1,Coordonnees n2);
Coordonnees I_intersect_soluce(Coordonnees m1,Coordonnees m2,Coordonnees n1,Coordonnees n2);
//int I_interne(Image*img,Coordonnees m,Coordonnees* coord,int taille);
int I_interne(Image*img,Coordonnees m,LigneBrisee l);
//void I_remplissage(Image*img,Coordonnees* coord,int taille);
void I_remplissage(Image*img,LigneBrisee l);

void I_draw       (Image *img);

#endif



