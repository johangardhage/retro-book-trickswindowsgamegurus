//
// DEMO8_8.CPP 8-bit quad drawing demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// GLOBALS ////////////////////////////////////////////////

POLYGON2D object; // the polygon object

// FUNCTIONS ////////////////////////////////////////////////

void DEMO_Render2(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// generate vertices, note these can be anywhere in space, but they must be in CW order
	int x0 = rand() % screen_width;
	int y0 = rand() % screen_height;
	int width = rand() % screen_width;
	int height = rand() % screen_height;

	// draw the triangle
	Draw_QuadFP_2D(x0, y0,
		x0 + width, y0,
		x0 + width, y0 + height,
		x0, y0 + height,
		rand() % 256, (UCHAR *)framebuffer, framebuffer_pitch);

	// draw the text
	Draw_Text_GDI("Press <ESC> to exit.", 8, 8, 255, framebuffer, framebuffer_pitch);

	// wait a sec
	Wait_Clock(30);

	RETRO_Flip();
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// define points of object (must be convex)
	VERTEX2DF object_vertices[4] = { -100,-100, 100,-100, 100,100, -100, 100 };

	// initialize polygon object
	object.state = 1;   // turn it on
	object.num_verts = 4;
	object.x0 = screen_width / 2; // position it
	object.y0 = screen_height / 2;
	object.xv = 0;
	object.yv = 0;
	object.color = 1; // animated green
	object.vlist = new VERTEX2DF[object.num_verts];

	for (int index = 0; index < object.num_verts; index++)
		object.vlist[index] = object_vertices[index];

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
