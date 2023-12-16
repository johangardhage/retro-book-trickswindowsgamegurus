//
// DEMO8_3.CPP 8-bit polygon demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

#define NUM_ASTEROIDS        64

// GLOBALS ////////////////////////////////////////////////

POLYGON2D asteroids[NUM_ASTEROIDS]; // the asteroids

// FUNCTIONS ////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw all the asteroids
	for (int curr_index = 0; curr_index < NUM_ASTEROIDS; curr_index++) {
		// glow asteroids
		asteroids[curr_index].color = rand() % 256;

		// do the graphics
		Draw_Polygon2D(&asteroids[curr_index], (UCHAR *)framebuffer, framebuffer_pitch);

		// move the asteroid
		asteroids[curr_index].x0 += asteroids[curr_index].xv;
		asteroids[curr_index].y0 += asteroids[curr_index].yv;

		// test for out of bounds
		if (asteroids[curr_index].x0 > screen_width + 100)
			asteroids[curr_index].x0 = -100;

		if (asteroids[curr_index].y0 > screen_height + 100)
			asteroids[curr_index].y0 = -100;

		if (asteroids[curr_index].x0 < -100)
			asteroids[curr_index].x0 = screen_width + 100;

		if (asteroids[curr_index].y0 < -100)
			asteroids[curr_index].y0 = screen_height + 100;
	} // end for curr_asteroid

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

	// loop and initialize all asteroids
	for (int curr_index = 0; curr_index < NUM_ASTEROIDS; curr_index++) {
		// initialize the asteroid
		asteroids[curr_index].state = 1;   // turn it on
		asteroids[curr_index].num_verts = 8;
		asteroids[curr_index].x0 = rand() % screen_width; // position it
		asteroids[curr_index].y0 = rand() % screen_height;
		asteroids[curr_index].xv = -8 + rand() % 17;
		asteroids[curr_index].yv = -8 + rand() % 17;
		asteroids[curr_index].color = rand() % 256;
		asteroids[curr_index].vlist = new VERTEX2DF[asteroids[curr_index].num_verts];

		for (int index = 0; index < asteroids[curr_index].num_verts; index++) {
			asteroids[curr_index].vlist[index] = asteroid_vertices[index];
		}
	}
}

/////////////////////////////////////////////////////////////
