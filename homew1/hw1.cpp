//Author: OMAR OSEGUERA
//Program: Homework 1 part 1
//cs335 Spring 2016 HW 1
//Purpose:
	//This program demonstrates the use of OpenGL and XWindows
	//This program also demonstrates the waterfall model
	//Waterfall model: Requirements, Design, Coding, Testing, Maintenance
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
//X11 components let us create a window using X Windows system
#include <X11/Xlib.h>
#include <X11/keysym.h>
//OpenGL header files
//lets us use library functions in OpenGL
#include <GL/glx.h>
extern "C"
{
#include "fonts.h"
}
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 4000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
	float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};

struct Game {
	bool bubbles;
	int lastMouse[2];
	Shape box;
	Shape circle;
	Shape array[5];
	Particle particle[MAX_PARTICLES];
	int n;
	//Game constructor
	Game(){
		bubbles = false;
		circle.radius = 160;
		circle.center.x = 320 + (5*65);
		circle.center.y = 300 - (5*60);
		//this for loop is to make boxes for waterfall model
		for(int i = 0; i < 5; i++) {
			array[i].width = 100;
			array[i].height = 15;
			array[i].center.x = 150 +(i*65);
			array[i].center.y = 500 - (i*60);
		}
	}
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);


int main(void)
{
	int done=0;
	srand(time(NULL));
	initXWindows();//initialize openGL
	init_opengl();
	//declare game object
	Game game;
	game.n=0;
	//this big loop is called "the controller"	
	//start animation "event loop"
	while(!done) {
		while(XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_mouse(&e, &game);
			done = check_keys(&e, &game);
		}
		movement(&game);//see if anything moves
		render(&game);//put it to screen
		glXSwapBuffers(dpy, win);//swap buffers
	}
	cleanupXWindows();
	return 0;
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "335 HW1: WATERFALL MODEL by OMAR OSEGUERA");
}

void cleanupXWindows(void) {
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}
//we will put this in another file during our project
void initXWindows(void) {
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };//24 - 24 bit color (8,8,8)(R,G,B)
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if(vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |//tracks evenrs
		ButtonPress | ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
			InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);
	//allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}
#define rnd() (float) rand() / (float) RAND_MAX
void makeParticle(Game *game, int x, int y) {
	if (game->n >= MAX_PARTICLES)
		return;
	//	std::cout << "makeParticle() " << x << " " << y << std::endl;
	//position of particle
	Particle *p = &game->particle[game->n];
	p->s.center.x = x;
	p->s.center.y = y;
	p->velocity.y = rnd() -0.5;
	p->velocity.x = rnd() + 0.5;
	game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
	static int savex = 0;
	static int savey = 0;
	static int n = 0;

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed
			int y = WINDOW_HEIGHT - e->xbutton.y;
			makeParticle(game, e->xbutton.x, y);
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		game->lastMouse[0] = savex;
		game->lastMouse[1] = savey;
		if (++n < 10)
			return;
		int y = WINDOW_HEIGHT - e->xbutton.y;
		makeParticle(game, e->xbutton.x, y);
	}
}

int check_keys(XEvent *e, Game *game)
{
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		if (key == XK_Escape) {
			return 1;
		}
		//You may check other keys here.
		if(key == XK_b) {
			game->bubbles = !game->bubbles;
		}
	}
	return 0;
}

void movement(Game *game)//physics
{
	Particle *p;
	if(game->bubbles){
		for(int i = MAX_PARTICLES; i >= 0; i--) {
			makeParticle(game, game->lastMouse[0], WINDOW_HEIGHT 
			- game->lastMouse[1]);
		}
	}

	if (game->n <= 0)
		return;

	for(int i = 0; i < game->n; i++) {
		p = &game->particle[i];
		p->s.center.x += p->velocity.x;//adds velocity to center of particle to move it
		p->s.center.y += p->velocity.y;
		//gravity
		p->velocity.y -= 0.2;
		//check for collision with shapes...
		for(int j = 0; j < 5; j++) {
			Shape *arr;//pointer to array of Boxes
			arr = &game->array[j];
			if(p->s.center.y >= arr->center.y - (arr->height) && 
					p->s.center.y <= arr->center.y + (arr->height) &&
					p->s.center.x >= arr->center.x - (arr->width) &&
					p->s.center.x <= arr->center.x + (arr->width))
			{	
				p->velocity.y *= -0.5;
			}
		}
		//collision with circle
		Shape *circle;
		circle = &game->circle;
		float diff1, diff2, distance;
		diff1 = p->s.center.x - circle->center.x;
		diff2 = p->s.center.y - circle->center.y;
		distance = sqrt((diff1*diff1) + (diff2*diff2));
		if(distance < circle->radius) {
	//	std::cout << "DISTANCE: " << distance << " RADIUS: " << circle->radius << std::endl;
			//move particle to circle edge
			p->s.center.x = circle->center.x + (diff1/distance) *
			circle->radius + 1.01;
			p->s.center.y = circle->center.y + (diff2/distance) *
			circle->radius + 1.01;
			
			//collision
			p->velocity.x -= 0.08;
			p->velocity.y += 1;	
		}
		//check for off-screen
		if (p->s.center.y < 0.0) {
		//	std::cout << "off screen" << std::endl;
			//delete element i in array (below)
			game->particle[i] = game->particle[game->n-1];//whatever is at end of array put at spot of particle out of screen
			game->n--;
		}
	}
}

void render(Game *game)
{
	
	float w, h;
	Rect r;
	glClear(GL_COLOR_BUFFER_BIT);//clear screen
	r.bot = WINDOW_HEIGHT - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ffffff, "Waterfall model");
	ggprint8b(&r, 16, 0x00ffffff, "OMAR OSEGUERA");
	ggprint8b(&r, 16, 0x00ffffff, "CS 335");

	//Draw shapes...
	//draw circle
	Shape *circle;
	circle = &game->circle;
	float x, y, radius;
	x =circle->center.x; 
	y = circle->center.y;
	radius = circle->radius;
	glPushMatrix();
	glColor3ub(100, 0, 0);
	int triangleAmount = 20;
	float twicePi = 2.0f * M_PI;
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x,y);//center of circle
		for(int i = 0; i <= triangleAmount; i++) {
			glVertex2f(
			x + (radius * cos(i * twicePi / triangleAmount)),
			y + (radius * sin(i * twicePi/ triangleAmount)));
		}
	glEnd();
	glPopMatrix();

	//DRAWING Array of BOX Shape *arr
	//draw box w/ text
	const char *text[5] = 
		{"Requirements", "Design", "Coding", "Testing", "Maintenance"};
	for(int i = 0; i < 5; i++){
		Shape *arr;
		glColor3ub(i*90,140,90);
		arr = &game->array[i];
		glPushMatrix();
		glTranslatef(arr->center.x, arr->center.y, arr->center.z);
		w = arr->width;
		h = arr->height;
		r.bot = arr->height - 20;
		r.left = arr->width - 150;
		glBegin(GL_QUADS);
		glVertex2i(-w, -h);
		glVertex2i(-w, h);
		glVertex2i(w, h);
		glVertex2i(w, -h);
		glEnd();
		ggprint8b(&r, 22, 0x00ffffff, text[i]);
		glPopMatrix();
	}
	//draw all particles here
	glPushMatrix();
	for(int i = 0; i < game->n; i++)
	{
		glColor3ub(0,0,i*9 + 1);//making diff blues
		Vec *c = &game->particle[i].s.center;
		w = 2;//particle size
		h = 2;//particle size
		glBegin(GL_QUADS);
		glVertex2i(c->x-w, c->y-h);
		glVertex2i(c->x-w, c->y+h);
		glVertex2i(c->x+w, c->y+h);
		glVertex2i(c->x+w, c->y-h);
		glEnd();
		glPopMatrix();
	}
}

