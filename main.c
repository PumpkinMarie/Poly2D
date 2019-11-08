
/*===============================================================*\

	Arash Habibi

	main.c

	Un programme equivalent à 02_glut.c et qui ne prend en compte
	que trois événements pour quitter le programme.

\*===============================================================*/

#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include "Image.h"

Image *img;
//Coordonnees coord[800];
//int taille = 800;
//int position = -1;
int polygone = 0;

int insert = 1;
int vertex = 0;
int edge = 0;
//Nouv struct
LigneBrisee initLigneBrisee()
{
	return (LigneBrisee)NULL;
}


LigneBrisee ligne;
LigneBrisee ligne_dep;
LigneBrisee vertex_selec;

LigneBrisee aj_sommet(LigneBrisee l,Coordonnees s){
		LigneBrisee new = malloc(sizeof(LigneBrisee*));
		new->p = s;
		if(l !=NULL){
			if (l->suivant!=NULL){
				new->suivant = l->suivant;
				l->suivant->precedent = new;
			}
			else
				new -> suivant = NULL;

			l->suivant=new;
			new->precedent = l;
			}
		else {
			new->suivant = NULL;
			new->precedent = NULL;
			}
		return new;
}

LigneBrisee suppr_sommet(LigneBrisee l, LigneBrisee pt){

	if (pt->precedent == NULL){
		pt->suivant->precedent = NULL;
		l = pt->suivant;
		free(pt);
		return l;
	}
	if	(pt->suivant == NULL){
		pt->precedent->suivant = NULL;
		l = pt->precedent;
		free(pt);
		return l;	
	}

	pt->suivant->precedent = pt->precedent;
	pt->precedent->suivant = pt->suivant;
	free(pt);
	return l;
}


void afficher_sommets (LigneBrisee l){
	LigneBrisee current = l;
	if (l==NULL)
		printf("loooo\n");
	while(current!= NULL){
		printf("%d    %d\n",current->p.x,current->p.y);
		current = current->suivant;
	}
}

void ligne_polygone(LigneBrisee debut, LigneBrisee fin){
	fin->suivant = debut;
	debut->precedent = fin;
}

void polygone_ligne(LigneBrisee debut, LigneBrisee fin){
	fin->suivant = NULL;
	debut->precedent = NULL;
}
//------------------------------------------------------------------
//	C'est le display callback. A chaque fois qu'il faut
//	redessiner l'image, c'est cette fonction qui est
//	appelee. Tous les dessins doivent etre faits a partir
//	de cette fonction.
//------------------------------------------------------------------

void display_CB()
{
    glClear(GL_COLOR_BUFFER_BIT);

	I_draw(img);

    glutSwapBuffers();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir en fonction de la position de
// la souris (x,y), en fonction du bouton de la souris qui a été
// pressé ou relaché.
//------------------------------------------------------------------

void mouse_CB(int button, int state, int x, int y)
{
	if((button==GLUT_LEFT_BUTTON)&&(state==GLUT_DOWN)){
		I_focusPoint(img,x,img->_height-y);
		if(!polygone && insert){
			//position++;
			Coordonnees p = {x,img->_height-y};
			//coord[position] = p;
			I_plot(img,x,img->_height-y);

			if(ligne == NULL){
				ligne = aj_sommet(ligne,p);
				ligne_dep = ligne;
			}
			else{
				ligne = aj_sommet(ligne,p);
			}
			//afficher_sommets(ligne_dep);
			//printf("\n");

			//if (position!=0)
			//	I_bresenham(img,coord[position-1].x,coord[position-1].y,p.x,p.y);
			if (ligne->precedent!=NULL)
				I_bresenham(img,ligne->precedent->p.x,ligne->precedent->p.y,ligne->p.x,ligne->p.y);

		}
	}

	glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche (non-spéciale) du clavier.
//------------------------------------------------------------------

void keyboard_CB(unsigned char key, int x, int y)
{
	// fprintf(stderr,"key=%d\n",key);
	switch(key)
	{
	case 27 : exit(1); break;
	case 'z' : I_zoom(img,2.0); break;
	case 'Z' : I_zoom(img,0.5); break;
	case 'i' : I_zoomInit(img); 
				insert = 1;
				vertex = 0;
				edge = 0;
				break;
	case 'c' : 
		if (polygone){
			polygone_ligne(ligne_dep,ligne);
			I_fill(img, C_new(0,0,0));
			//I_ligneBrisee(img,coord,position);
			I_ligneBrisee(img,ligne_dep);
			polygone--;
		} 
		else{
			/*if(position>0){
				I_fill(img, C_new(0,0,0));
				I_ligneBrisee(img,coord,position);
				I_bresenham(img,coord[0].x,coord[0].y,coord[position].x,coord[position].y);
				polygone++;
			}*/
			if(ligne!=NULL){
				polygone++;
				I_fill(img, C_new(0,0,0));
				I_ligneBrisee(img,ligne_dep);
				I_bresenham(img,ligne->p.x,ligne->p.y,ligne_dep->p.x,ligne_dep->p.y);
				ligne_polygone(ligne_dep,ligne);
			}
		}
		break;
		
	case 'f' : //I_remplissage(img,coord,position);
		if (polygone)
			I_remplissage(img,ligne_dep);
		break;
	
	//- VERTEX 
	case 'v' : 
		vertex_selec = ligne_dep;
		I_plotColor  (img, ligne_dep->p.x, ligne_dep->p.y, C_new(255,0,0));
		vertex=1;
		insert = 0;
		edge = 0;
		break;

	//- page précédente alternative
	case 'q' : 
		if (vertex){
			if (vertex_selec->precedent!=NULL){
				I_plotColor  (img, vertex_selec->p.x, vertex_selec->p.y, C_new(255,255,255));
				vertex_selec = vertex_selec->precedent;
				I_plotColor  (img, vertex_selec->p.x, vertex_selec->p.y, C_new(255,0,0));
			}
		}
		if (edge){
			if (vertex_selec->precedent!=NULL){
				I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
				vertex_selec = vertex_selec->precedent;
				I_changeColor(img, C_new(255,0,0));
				I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
				I_changeColor(img, C_new(255,255,255));
			}
		}
		break;
	//- page suivante alternative
	case 'd' : 
		if (vertex){
			if (vertex_selec->suivant!=NULL){
				I_plotColor  (img, vertex_selec->p.x, vertex_selec->p.y, C_new(255,255,255));
				vertex_selec = vertex_selec->suivant;
				I_plotColor  (img, vertex_selec->p.x, vertex_selec->p.y, C_new(255,0,0));
			}
		}
		if (edge){
			if (vertex_selec->suivant->suivant!=NULL){
				I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
				vertex_selec = vertex_selec->suivant;
				I_changeColor(img, C_new(255,0,0));
				I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
				I_changeColor(img, C_new(255,255,255));	
			}
		}
		break;

	// - suppr
	case 127 : 
	if (vertex){
		if (vertex_selec == ligne)
			ligne = suppr_sommet(ligne,vertex_selec);				
		else 
			ligne_dep = suppr_sommet(ligne_dep,vertex_selec);
		I_fill(img, C_new(0,0,0));
		if(polygone)I_Polygone(img,ligne_dep);
		else I_ligneBrisee(img,ligne_dep);
		vertex_selec = ligne_dep;
	}
	break;

	// - edge
	case 'e' :
		vertex = 0;
		insert = 0;
		edge = 1;
		vertex_selec = ligne_dep;
		I_changeColor(img, C_new(255,0,0));
		I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
		I_changeColor(img, C_new(255,255,255));
	break;
		
	// - shift (bouton du milieu mais pas de souris actuellement)
	case 'm' : 
		if (edge){
			Coordonnees milieu;
			milieu.x = abs((vertex_selec->suivant->p.x + vertex_selec->p.x)/2);
			milieu.y = abs((vertex_selec->suivant->p.y + vertex_selec->p.y)/2);
			vertex_selec = aj_sommet(vertex_selec ,milieu);
			I_fill(img, C_new(0,0,0));
			if(polygone)I_Polygone(img,ligne_dep);
			else I_ligneBrisee(img,ligne_dep);
		}
	break;

	default : fprintf(stderr,"keyboard_CB : %d : unknown key.\n",key);
	}
	glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche spéciale (F1, F2 ... F12, home, end, insert,
// haut, bas, droite, gauche etc).
//------------------------------------------------------------------

void special_CB(int key, int x, int y)
{
	// int mod = glutGetModifiers();

	int d = 10;

	switch(key)
	{
	case GLUT_KEY_PAGE_DOWN:
		if (vertex){
			if (vertex_selec->precedent!=NULL){
				I_plotColor  (img, vertex_selec->p.x, vertex_selec->p.y, C_new(255,255,255));
				vertex_selec = vertex_selec->precedent;
				I_plotColor  (img, vertex_selec->p.x, vertex_selec->p.y, C_new(255,0,0));
			}
		if (edge){
			if (vertex_selec->precedent!=NULL){
				I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
				vertex_selec = vertex_selec->precedent;
				I_changeColor(img, C_new(255,0,0));
				I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
				I_changeColor(img, C_new(255,255,255));
			}
		}
		}
		break;
	case GLUT_KEY_PAGE_UP:
		if (vertex){
			if (vertex_selec->suivant!=NULL){
				I_plotColor  (img, vertex_selec->p.x, vertex_selec->p.y, C_new(255,255,255));
				vertex_selec = vertex_selec->suivant;
				I_plotColor  (img, vertex_selec->p.x, vertex_selec->p.y, C_new(255,0,0));
			}
		}
		if (edge){
			if (vertex_selec->suivant->suivant!=NULL){
				I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
				vertex_selec = vertex_selec->suivant;
				I_changeColor(img, C_new(255,0,0));
				I_bresenham(img,vertex_selec->p.x,vertex_selec->p.y,vertex_selec->suivant->p.x,vertex_selec->suivant->p.y);
				I_changeColor(img, C_new(255,255,255));	
			}
		}
		break;
	case GLUT_KEY_UP    : 
		if(vertex){
			vertex_selec->p.y++;
			I_fill(img, C_new(0,0,0));
			if(polygone)I_Polygone(img,ligne_dep);
			else I_ligneBrisee(img,ligne_dep);
		}
		else I_move(img,0,d); 
		break;
	case GLUT_KEY_DOWN  : 
		if(vertex){
			vertex_selec->p.y--;
			I_fill(img, C_new(0,0,0));
			if(polygone)I_Polygone(img,ligne_dep);
			else I_ligneBrisee(img,ligne_dep);
		}
		else I_move(img,0,-d); 
		break;
	case GLUT_KEY_LEFT  : 		
		if(vertex){
			vertex_selec->p.x--;
			I_fill(img, C_new(0,0,0));
			if(polygone)I_Polygone(img,ligne_dep);
			else I_ligneBrisee(img,ligne_dep);
		}
		else I_move(img,d,0); 
		break;
	case GLUT_KEY_RIGHT : 
		if(vertex){
			vertex_selec->p.x++;
			I_fill(img, C_new(0,0,0));
			if(polygone)I_Polygone(img,ligne_dep);
			else I_ligneBrisee(img,ligne_dep);
		}
		else I_move(img,-d,0); 
	break;
	default : fprintf(stderr,"special_CB : %d : unknown key.\n",key);
	}
	glutPostRedisplay();
}

//------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if((argc!=3)&&(argc!=2))
	{
		fprintf(stderr,"\n\nUsage \t: %s <width> <height>\nou",argv[0]);
		fprintf(stderr,"\t: %s <ppmfilename> \n\n",argv[0]);
		exit(1);
	}
	else
	{
		int largeur, hauteur;
		if(argc==2)
		{
			img = I_read(argv[1]);
			largeur = img->_width;
			hauteur = img->_height;
		}
		else
		{
			largeur = atoi(argv[1]);
			hauteur = atoi(argv[2]);
			img = I_new(largeur,hauteur);
			//ligne_dep = suppr_sommet(ligne_dep,1);
			//afficher_sommets(ligne_dep);
		}
		ligne = initLigneBrisee();
		ligne_dep = ligne;
		int windowPosX = 100, windowPosY = 100;

		glutInitWindowSize(largeur,hauteur);
		glutInitWindowPosition(windowPosX,windowPosY);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE );
		glutInit(&argc, argv);
		glutCreateWindow(argv[0]);

		glViewport(0, 0, largeur, hauteur);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glOrtho(0,largeur,0,hauteur,-1,1);

		glutDisplayFunc(display_CB);
		glutKeyboardFunc(keyboard_CB);
		glutSpecialFunc(special_CB);
		glutMouseFunc(mouse_CB);
		// glutMotionFunc(mouse_move_CB);
		// glutPassiveMotionFunc(passive_mouse_move_CB);
		glutMainLoop();

		return 0;
	}
}
