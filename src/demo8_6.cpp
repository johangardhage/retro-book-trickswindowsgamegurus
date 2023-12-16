//
// DEMO8_6.CPP 8-bit polygon transformation demo with matrices
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                             // general printing buffer

POLYGON2D ship; // the ship

// FUNCTIONS ////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// do the graphics
	Draw_Polygon2D(&ship, (UCHAR *)framebuffer, framebuffer_pitch);

	// test for scale
	if (RETRO_KeyState(SDL_SCANCODE_A)) // scale up
		Scale_Polygon2D_Mat(&ship, 1.1, 1.1);
	else if (RETRO_KeyState(SDL_SCANCODE_S)) // scale down
		Scale_Polygon2D_Mat(&ship, 0.9, 0.9);

	// test for rotate
	if (RETRO_KeyState(SDL_SCANCODE_Z)) // rotate left
		Rotate_Polygon2D_Mat(&ship, -5);
	else if (RETRO_KeyState(SDL_SCANCODE_X)) // rotate right
		Rotate_Polygon2D_Mat(&ship, 5);

	// test for translate
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) // translate right
		Translate_Polygon2D_Mat(&ship, 5, 0);
	else if (RETRO_KeyState(SDL_SCANCODE_LEFT)) // translate left
		Translate_Polygon2D_Mat(&ship, -5, 0);

	// test for translate
	if (RETRO_KeyState(SDL_SCANCODE_UP)) // translate up
		Translate_Polygon2D_Mat(&ship, 0, -5);
	else if (RETRO_KeyState(SDL_SCANCODE_DOWN)) // translate left
		Translate_Polygon2D_Mat(&ship, 0, 5);

	// draw some information
	sprintf(buffer, "(x,y) = [%d, %d]     ", ship.x0, ship.y0);
	Draw_Text_GDI(buffer, 8, screen_height - 16, 255, framebuffer, framebuffer_pitch);
	Draw_Text_GDI("<A> and <S> - Scale    <Z> and <X> - Rotate     <Arrows> - Translate     <ESC> - Exit", 8, 8, 255, framebuffer, framebuffer_pitch);

	// wait a sec
	Wait_Clock(30);
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// define points of ship
	VERTEX2DF ship_vertices[24] =
	{ 1,11,
	 2,8,
	 1,7,
	 1,-1,
	 3,-1,
	 3,-2,
	 11,-3,
	 11,-6,
	 3,-7,
	 2,-8,
	 1,-8,
	 1,-7,
	 -1,-7,
	 -1,-8,
	 -2,-8,
	 -3,-7,
	 -11,-6,
	 -11,-3,
	 -3,-2,
	 -3,-1,
	 -1,-1,
	 -1,7,
	 -2,8,
	 -1, 11 };

	// initialize ship
	ship.state = 1;   // turn it on
	ship.num_verts = 24;
	ship.x0 = screen_width / 2; // position it
	ship.y0 = screen_height / 2;
	ship.xv = 0;
	ship.yv = 0;
	ship.color = 2; // green
	ship.vlist = new VERTEX2DF[ship.num_verts];

	for (int index = 0; index < ship.num_verts; index++)
		ship.vlist[index] = ship_vertices[index];

	// do a quick scale on the vertices, they are a bit too small
	Scale_Polygon2D(&ship, 3.0, 3.0);

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
