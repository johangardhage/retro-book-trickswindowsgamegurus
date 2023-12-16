//
// DEMO8_5.CPP 8-bit polygon scaling demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

#define NUM_ASTEROIDS        64

// GLOBALS ////////////////////////////////////////////////

POLYGON2D asteroid; // the asteroid

// FUNCTIONS ////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// do the graphics
	Draw_Polygon2D(&asteroid, (UCHAR *)framebuffer, framebuffer_pitch);

	// test for scale
	if (RETRO_KeyState(SDL_SCANCODE_A)) {
		Scale_Polygon2D(&asteroid, 1.1, 1.1);
	} else if (RETRO_KeyState(SDL_SCANCODE_S)) {
		Scale_Polygon2D(&asteroid, 0.9, 0.9);
	}

	// rotate the polygon by 5 degrees
	Rotate_Polygon2D(&asteroid, 5);

	// wait a sec
	Wait_Clock(30);
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// define points of asteroid
	VERTEX2DF asteroid_vertices[8] = { 33,-3, 9,-18, -12,-9, -21,-12, -9,6, -15,15, -3,27, 21,21 };

	// initialize asteroid
	asteroid.state = 1;   // turn it on
	asteroid.num_verts = 8;
	asteroid.x0 = screen_width / 2; // position it
	asteroid.y0 = screen_height / 2;
	asteroid.xv = 0;
	asteroid.yv = 0;
	asteroid.color = 255; // white
	asteroid.vlist = new VERTEX2DF[asteroid.num_verts];

	for (int index = 0; index < asteroid.num_verts; index++) {
		asteroid.vlist[index] = asteroid_vertices[index];
	}

	// create sin/cos lookup table

	// generate the tables
	for (int ang = 0; ang < 360; ang++) {
		// convert ang to radians
		float theta = (float)ang * PI / (float)180;

		// insert next entry into table
		cos_look[ang] = cos(theta);
		sin_look[ang] = sin(theta);
	} // end for ang
}

/////////////////////////////////////////////////////////////
