/*====================================================*\
  Arash HABIBI
  Image.c
\*====================================================*/

#include "Image.h"

//------------------------------------------------------------------------

Color C_new(float red, float green, float blue)
{
	Color c;
	c._red = red;
	c._green = green;
	c._blue = blue;
	return c;
}

//------------------------------------------------------------------------

void C_check(Color c, char *message)
{
	fprintf(stderr,"%s : %f %f %f\n",message,c._red,c._green,c._blue);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

Image* I_new(int width, int height)
{
	Image *img_new = (Image*)malloc(sizeof(Image));
	img_new->_width = width;
	img_new->_height = height;
	img_new->_xzoom = 0;
	img_new->_yzoom = 0;
	img_new->_zoom = 1.0;

	img_new->_xoffset=0;
	img_new->_yoffset=0;

	img_new->_current_color = C_new(255,255,255);

	img_new->_buffer = (Color**)calloc(width,sizeof(Color*));

	int x;
	for(x=0;x<width;x++)
		img_new->_buffer[x] = (Color*)calloc(height,sizeof(Color));

	return img_new;
}

//------------------------------------------------------------------------

static void _plot(Image *img, int x, int y, Color c)
{
	img->_buffer[x][y] = c;
}

//-----

static int _isPpm(char *imagefilename)
{
	FILE *imagefile;
	imagefile = fopen(imagefilename,"r");
	if(imagefile==NULL) {perror(imagefilename); exit(1); }

	else
	{
		int c1 = fgetc(imagefile);
		int c2 = fgetc(imagefile);
		fclose(imagefile);

		if((c1=='P')&&(c2=='6'))	return 1;
		else						return 0;
	}
}

//-----

Image* I_read(char *imagefilename)
{
	Image *img;
	char command[100];

	if(_isPpm(imagefilename))	sprintf(command,"cp %s input.ppm",imagefilename);
	else					sprintf(command,"convert %s input.ppm",imagefilename);

	int stat = system(command);
	if(stat!=0)
	{
		fprintf(stderr,"Convert : %s -> input.ppm impossible conversion.\n", imagefilename);
		exit(1);
	}
	else
	{
		Ppm ppm = PPM_nouv("input.ppm", PPM_LECTURE);
		system("rm input.ppm");

		fprintf(stderr,"%d x %d\n",PPM_largeur(ppm),PPM_hauteur(ppm));

		if(ppm!=NULL)
		{
			img = I_new(PPM_largeur(ppm),PPM_hauteur(ppm));
			int nb_bits=ppm->_nb_bits;
			int valmax = ppm->_valmax;

			int nb_pixels = img->_width*img->_height;

			if(nb_bits <= 8)
			{
				unsigned char *donnees = (unsigned char*)calloc(3*nb_pixels,sizeof(unsigned char));
				PPM_lectureDonneesChar(ppm, donnees);

				int x,y;
				for(y=0;y<img->_height;y++)
					for(x=0;x<img->_width;x++)
					{
						int indice = (img->_height-y)*img->_width + x;
						Color c = C_new((1.0*donnees[3*indice  ])/valmax,
										(1.0*donnees[3*indice+1])/valmax,
										(1.0*donnees[3*indice+2])/valmax);
						_plot(img,x,y,c);
					}
			}
			else
			{
				unsigned short *donnees = (unsigned short*)calloc(3*nb_pixels,sizeof(unsigned short));
				PPM_lectureDonneesShort(ppm, donnees);
				int x,y;
				for(y=0;y<img->_height;y++)
					for(x=0;x<img->_width;x++)
					{
						int indice = (img->_height-y)*img->_width + x;
						Color c = C_new((1.0*donnees[3*indice  ])/valmax,
										(1.0*donnees[3*indice+1])/valmax,
										(1.0*donnees[3*indice+2])/valmax);
						img->_buffer[x][y] = c;
					}
			}
			PPM_fermeture(ppm);
			return(img);
		}
		else
			return(NULL);
	}
}

//------------------------------------------------------------------------

void I_fill(Image *img, Color c)
{
	int x,y;
	for(x=0;x<img->_width;x++)
		for(y=0;y<img->_height;y++)
			img->_buffer[x][y]=c;
}

//------------------------------------------------------------------------

void I_checker(Image *img, Color c1, Color c2, int step)
{
	int x,y;
	for(x=0;x<img->_width;x++)
		for(y=0;y<img->_height;y++)
		{
			int n_x = x/step;
			int n_y = y/step;
			if((n_x+n_y)%2==0)	_plot(img,x,y,c1);
			else				_plot(img,x,y,c2);
		}
}

//------------------------------------------------------------------------

void I_changeColor(Image *img, Color c)
{
	img->_current_color = c;
}

//------------------------------------------------------------------------

void I_plot(Image *img, int x, int y)
{
	if((x>=0)&&(x<img->_width)&&
	   (y>=0)&&(y<img->_height))
		img->_buffer[x][y] = img->_current_color;
	else
	{
		fprintf(stderr,"I_plot : ERROR !!!\n");
		fprintf(stderr,"x (=%d) must be in the [%d,%d] range and\n", x, 0, img->_width);
		fprintf(stderr,"y (=%d) must be in the [%d,%d] range\n", y, 0, img->_height);
	}
}

//------------------------------------------------------------------------

void I_plotColor(Image *img, int x, int y, Color c)
{
	if((x>=0)&&(x<img->_width)&&
	   (y>=0)&&(y<img->_height))
		img->_buffer[x][y] = c;
	else
	{
		fprintf(stderr,"I_plotColor : ERROR !!!\n");
		fprintf(stderr,"x (=%d) must be in the [%d,%d] range and\n", x, 0, img->_width);
		fprintf(stderr,"y (=%d) must be in the [%d,%d] range\n", y, 0, img->_height);
	}
}

//------------------------------------------------------------------------
// Changement de repère

static void _windowToImage(Image *img, int xwin, int ywin, int *ximg, int *yimg)
{

	*ximg = img->_xoffset + img->_xzoom + (xwin-img->_xzoom) / img->_zoom;
	*yimg = img->_yoffset + img->_yzoom + (ywin-img->_yzoom) / img->_zoom;
}

//-----
// Changement de repère inverse
/*
static void _imageToWindow(Image *img, int ximg, int yimg, int *xwin, int *ywin)
{

	*xwin = img->_xoffset + img->_xzoom + (ximg-img->_xzoom-img->_xoffset) * img->_zoom;
	*ywin = img->_yoffset + img->_yzoom + (yimg-img->_yzoom-img->_yoffset) * img->_zoom;
}
*/
//-----

void I_focusPoint(Image *img, int xwin, int ywin)
{
	int dx = xwin - img->_xzoom;
	int dy = ywin - img->_yzoom;
	img->_xoffset -= dx*(1-1.0/img->_zoom);
	img->_yoffset -= dy*(1-1.0/img->_zoom);
	img->_xzoom = xwin;
	img->_yzoom = ywin;
}

//------------------------------------------------------------------------

void I_zoomInit(Image *img)
{
	img->_xoffset = 0;
	img->_yoffset = 0;
	img->_zoom = 1.0;
}

//------------------------------------------------------------------------

void I_zoom(Image *img, double zoom_coef)
{
	img->_zoom = img->_zoom * zoom_coef;
}

//------------------------------------------------------------------------

void I_move(Image *img, int x, int y)
{
	img->_xoffset += x;
	img->_yoffset += y;
}

//------------------------------------------------------------------------

//TODO

void I_bresenhamOrigin(Image*img, int x, int y){
	int incrd1 = 2*y; int incrd2 = 2*(y-x);
	int xp = 0; int yp = 0; int d = 2*y-x;
	while(xp<x){
		I_plot(img,xp,yp);
		xp+=1;
		if(d<0)
			d+=incrd1;
		else{
			yp+=1;
			d+=incrd2;
		}
	}
}


void Z2_to_1erOctent(int xA,int yA,int xB,int yB,int *xAf,int *yAf,int *xBf,int *yBf){
  if (xB>xA){
    *xAf = xA; *xBf = xB;
  }
  else{
    *xAf = -xA; *xBf = -xB;
  }
  if (yB>yA){
    *yAf = yA; *yBf = yB;
  }
  else{
    *yAf = -yA; *yBf = -yB;
  }
  if(abs(xB-xA) < abs(yB-yA)){	
    int temp = *xAf;
    *xAf = *yAf;
    *yAf = temp;
    temp = *xBf;
    *xBf = *yBf;
    *yBf = temp;
  }
}

void Octent_to_Z2(int xA,int yA,int xB,int yB,int x,int y,int *xf,int *yf){
  if(abs(xB-xA) < abs(yB-yA)){
    int temp = x;
    x = y;
    y = temp;
  }
  if (xB>xA)
    *xf = x;
  else
    *xf = -x;
  if (yB>yA)
    *yf = y;
  else
    *yf = -y;
}

//------------------------------------------------------------------------
void I_bresenham(Image*img, int xA, int yA, int xB, int yB){
	int xA_to = xA;int yA_to = yA;int xB_to = xB;int yB_to = yB;
	Z2_to_1erOctent(xA, yA, xB, yB,&xA_to,&yA_to,&xB_to,&yB_to);
	int dx = xB_to-xA_to; int dy = yB_to-yA_to;
	int incrd1 = 2*dy; int incrd2 = 2*(dy-dx);
	int x = xA_to; int y = yA_to; int d = 2*dy-dx;
	int xf,yf;
	while(x<xB_to){
	    Octent_to_Z2(xA,yA,xB,yB,x,y,&xf,&yf);
		I_plot(img,xf,yf);
		x+=1;
		if(d<0)
			d+=incrd1;
		else{
			y+=1;
			d+=incrd2;
		}
	}
}

/*void I_ligneBrisee(Image*img,Coordonnees * coord,int taille){
	int i;
	for (i = 1 ; i <= taille ; i++)
    {
		I_bresenham(img,coord[i-1].x,coord[i-1].y,coord[i].x,coord[i].y);
    }
}*/

void I_ligneBrisee(Image*img,LigneBrisee l){
	while(l->suivant!=NULL)
    {
		I_bresenham(img,l->p.x,l->p.y,l->suivant->p.x,l->suivant->p.y);
		l = l->suivant;
    }
}

void I_Polygone(Image*img,LigneBrisee l){
	LigneBrisee new = l;
	I_bresenham(img,l->p.x,l->p.y,l->suivant->p.x,l->suivant->p.y);
	new = new->suivant;
	while(new != l)
    {
		I_bresenham(img,new->p.x,new->p.y,new->suivant->p.x,new->suivant->p.y);
		new = new->suivant;
    }
}

int determinant(Coordonnees m1, Coordonnees m2, Coordonnees m3){
	int det = m1.x * (m2.y-m3.y) - m2.x * (m1.y-m3.y) + m3.x * (m1.y-m2.y);
	return det;
}
//------------------------------------------------------------------------
int I_intersect(Coordonnees m1,Coordonnees m2,Coordonnees m3,Coordonnees m4){
	int s1 = determinant(m1,m2,m3) ,s2 = determinant(m1,m2,m4),s3 = determinant(m3,m4,m1),s4 = determinant(m3,m4,m2);
	if (((s1<0 && s2>0) || (s1>0 && s2<0)) && ((s3<0 && s4>0) || (s3>0 && s4<0)))
		return 1;
	return 0;
}

Coordonnees I_intersect_soluce(Coordonnees m1,Coordonnees m2,Coordonnees m3,Coordonnees m4){
	int det = (m2.x-m1.x)*(m3.y-m4.y)-(m3.x-m4.x)*(m2.y-m1.y);
	Coordonnees inter = {-1,-1};
	float t1,t2; int test1,test2;
	if (det == 0)
		return inter;
	else{
		t1 = (float)((m3.x-m1.x)*(m3.y-m4.y)-(m3.x-m4.x)*(m3.y-m1.y))/det;
		t2 = (float)((m2.x-m1.x)*(m3.y-m1.y)-(m3.x-m1.x)*(m2.y-m1.y))/det;
		test1 = (m3.x-m1.x)*(m3.y-m4.y)-(m3.x-m4.x)*(m3.y-m1.y);
		test2 = (m2.x-m1.x)*(m3.y-m1.y)-(m3.x-m1.x)*(m2.y-m1.y);
		if(t1>1 || t1<0 || t2>1 || t2<0)
			return inter;
		else{
			if (test1 == 0){
				inter.x = m1.x;
				inter.y = m1.y;
			}
			else if (test1 == det){
				inter.x = m2.x;
				inter.y = m2.y;
			}
			else {
				if (test2 == 0){
					inter.x = m3.x;
					inter.y = m3.y;
				}
				else if (test2 == det){
					inter.x = m4.x;
					inter.y = m4.y;
				}
				else{
					inter.x = (int)(m1.x + (test1*(m2.x-m1.x))/det);
					inter.y = (int)(m1.y + t1*(m2.y-m1.y));
				}	
			}
		}
	}
	return inter;
}

//------------------------------------------------------------------------
int I_interne(Image*img,Coordonnees m,LigneBrisee l){
	LigneBrisee new = l;
	int inter = 0;
	int y = m.y;
	Coordonnees p = {img->_width-1,y};

	if((y == new->p.y) && (m.x < new->p.x))
		if((new->suivant->p.y > y && new->precedent->p.y < y) || (new->suivant->p.y < y && new->precedent->p.y > y) )
			inter+=1;
		else
			inter+=0;
	else	
		inter += I_intersect(m,p,new->p,new->suivant->p);	
	new = new->suivant;

	while (new!=l){
		if((y == new->p.y) && (m.x < new->p.x))
			if((new->suivant->p.y > y && new->precedent->p.y < y) || (new->suivant->p.y < y && new->precedent->p.y > y) )
				inter+=1;
			else
				inter+=0;
		else	
			inter += I_intersect(m,p,new->p,new->suivant->p);
		new = new->suivant;
	}

	if ((inter%2) != 0){
		return 1;}
	return 0;
}

/*int I_interne(Image*img,Coordonnees m,Coordonnees* coord,int taille){
	int inter = 0;
	int y = m.y;
	Coordonnees p = {img->_width-1,y};
	for(int i = 0;i<taille;i++){
		if((y == coord[i].y) && (m.x < coord[i].x) && i==0)
			if((coord[i+1].y > y && coord[taille].y < y) || (coord[i+1].y < y && coord[taille].y > y) )
				inter+=1;
			else
				inter+=0;		
		else if((y == coord[i].y) && (m.x < coord[i].x))
			if((coord[i+1].y > y && coord[i-1].y < y) || (coord[i+1].y < y && coord[i-1].y > y) )
				inter+=1;
			else
				inter+=0;
		else	
			inter += I_intersect(m,p,coord[i],coord[i+1]);
	}
	if(y == (coord[taille].y) && (m.x < coord[taille].x))
		if((coord[0].y > y && coord[taille-1].y < y) || (coord[0].y < y && coord[taille-1].y > y))
			inter+=1;
		else
			inter+=0;	
	else
		inter += I_intersect(m,p,coord[taille],coord[0]);

	if ((inter%2) != 0){
		return 1;}
	return 0;
}*/

/*int appartient(int pt, Coordonnees* pts_inter, int taille ){
	for (int i = 0;i<taille;i++){
		if (pt == pts_inter[i].x)
			return 1;
	}
	return 0;
}*/

int appartient(int pt,LigneBrisee l ){
	LigneBrisee new = l;
	if (pt == new->p.x)
		return 1;
	new = new->suivant;
	while (new!=l){
		if (pt == new->p.x)
			return 1;	
		new = new->suivant;		
	}
	return 0;
}

/*void I_remplissage(Image*img,Coordonnees* coord,int taille){
	int x,y,c;
	int xmin = img->_width,xmax = 0,ymin = img->_height,ymax = 0;
	for(c=0;c<=taille;c++){
		if (coord[c].x < xmin)
			xmin = coord[c].x;
		if (coord[c].x > xmax)
			xmax = coord[c].x;
		if (coord[c].y < ymin)
			ymin = coord[c].y;
		if (coord[c].y > ymax)
			ymax = coord[c].y;
	}

	for (y=ymin;y<=ymax;y++){
		Coordonnees dmin = {xmin,y};
		Coordonnees dmax = {xmax,y};
		Coordonnees pts_inter[taille];
		int nb_inter = 0;
		for(int i = 0;i<taille;i++){
			//if(I_intersect(dmin,dmax,coord[i],coord[i+1])==1){
				Coordonnees test = I_intersect_soluce(dmin,dmax,coord[i],coord[i+1]);
				if(test.x!=coord[i+1].x){
					if(test.x == coord[i].x){
						printf("%d   %d\n",y,test.x);
						if (i==0){
							if((coord[i+1].y > coord[i].y && coord[taille].y < coord[i].y) || (coord[i+1].y < coord[i].y && coord[taille].y > coord[i].y))
								pts_inter[nb_inter++] = test;
						}
						else if((coord[i+1].y > coord[i].y && coord[i-1].y < coord[i].y) || (coord[i+1].y < coord[i].y && coord[i-1].y > coord[i].y))
							pts_inter[nb_inter++] = test;
	
					}
					else
					{
						pts_inter[nb_inter++] = test;
					}
					
				}
				if (test.x != -1 && test.y != -1){
					if(test.x!=coord[i+1].x){
						if(test.x == coord[i].x){
							if (i==0){
								if((coord[i+1].y > coord[i].y && coord[taille].y < coord[i].y) || (coord[i+1].y < coord[i].y && coord[taille].y > coord[i].y))
									pts_inter[nb_inter++] = test;
							}
							else if((coord[i+1].y > coord[i].y && coord[i-1].y < coord[i].y) || (coord[i+1].y < coord[i].y && coord[i-1].y > coord[i].y))
								pts_inter[nb_inter++] = test;
						}
						else
						{
						pts_inter[nb_inter++] = test;
						}
					}	
				}
				//}
		}
		//if(I_intersect(dmin,dmax,coord[taille],coord[0])==1){
			Coordonnees test = I_intersect_soluce(dmin,dmax,coord[taille],coord[0]);
			if(test.x!=coord[0].x){
				if(test.x == coord[taille].x){
					if((coord[0].y > coord[taille].y && coord[taille-1].y < coord[taille].y) || (coord[0].y < coord[taille].y && coord[taille-1].y > coord[taille].y))
						pts_inter[nb_inter++] = test;

				}
				else
				{
					pts_inter[nb_inter++] = test;
				}
				
			}
		if (test.x != -1 && test.y != -1)
			if(test.x!=coord[0].x){
				if(test.x == coord[taille].x){
					if((coord[0].y > coord[taille].y && coord[taille-1].y < coord[taille].y) || (coord[0].y < coord[taille].y && coord[taille-1].y > coord[taille].y))
						pts_inter[nb_inter++] = test;

				}
				else
				{
					pts_inter[nb_inter++] = test;
				}
			}
		//}

		int est_allume = 0;
		for(x = xmin;x<=xmax;x++){
			if (appartient(x,pts_inter,nb_inter)){	
				est_allume = (est_allume+1)%2;
			}
			if (est_allume == 1)
			I_plot(img,x,y);
		}
			
	}
}*/

/*void I_remplissage(Image*img,Coordonnees* coord,int taille){
	int i,j,c;
	int xmin = img->_height,xmax = 0,ymin = img->_width,ymax = 0;
	for(c=0;c<=taille;c++){
		if (coord[c].x < xmin)
			xmin = coord[c].x;
		if (coord[c].x > xmax)
			xmax = coord[c].x;
		if (coord[c].y < ymin)
			ymin = coord[c].y;
		if (coord[c].y > ymax)
			ymax = coord[c].y;
	}
	for (i=ymin;i<=ymax;i++){
		for (j=xmin;j<=xmax;j++){
			Coordonnees p = {j,i};
			if (I_interne(img,p,coord,taille)!=0)
				img->_buffer[j][i] = img->_current_color;
		}	
	}
}*/

void I_remplissage(Image*img,LigneBrisee l){
	LigneBrisee new = l->suivant;
	int i,j;
	int xmin = l->p.x,xmax = l->p.x,ymin = l->p.y,ymax = l->p.y;

	while (new!=l){
		if (new->p.x < xmin)
			xmin = new->p.x;
		if (new->p.x > xmax)
			xmax = new->p.x;
		if (new->p.y < ymin)
			ymin = new->p.y;
		if (new->p.y > ymax)
			ymax = new->p.y;
		new = new->suivant;	
	}

	for (i=ymin;i<=ymax;i++){
		for (j=xmin;j<=xmax;j++){
			Coordonnees p = {j,i};
			if (I_interne(img,p,l)!=0)
				img->_buffer[j][i] = img->_current_color;
		}	
	}
}

//------------------------------------------------------------------------

void I_draw(Image *img)
{
	glBegin(GL_POINTS);
	int xwin, ywin, ximg, yimg;
	for(xwin=0;xwin<img->_width;xwin++)
		for(ywin=0;ywin<img->_height;ywin++)
		{
			_windowToImage(img, xwin, ywin, &ximg, &yimg);
			Color c;
			if((ximg>=0)&&(ximg<img->_width)&&
			   (yimg>=0)&&(yimg<img->_height))
				c = img->_buffer[ximg][yimg];
			else
				c = C_new(0,0,0);

			glColor3f(c._red,c._green,c._blue);
			glVertex2i(xwin,ywin);
		}
	glEnd();
}

//------------------------------------------------------------------------





