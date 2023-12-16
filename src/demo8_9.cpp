//
// DEMO8_9.CPP 8-bit general polygon fill demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// GLOBALS ////////////////////////////////////////////////

POLYGON2D object; // the polygon object

// FUNCTIONS ////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	static int green_inc = 4; // used to animate the green glow

	// start the timing clock
	Start_Clock();

	// do the graphics
	Draw_Filled_Polygon2D(&object, (UCHAR *)framebuffer, framebuffer_pitch);

	// rotate the polygon by 5 degrees
	Rotate_Polygon2D(&object, 5);

	// perform palette animation
	Set_Palette_Entry(1, &palette[1]);

	// animate green
	palette[1].peGreen += green_inc;

	// check if ready to change animation in other direction
	if (palette[1].peGreen > 255 || palette[1].peGreen < 16) {
		// invert increment
		green_inc = -green_inc;
		palette[1].peGreen += green_inc;
	} // end if

	// draw the text
	Draw_Text_GDI("Press <ESC> to exit.", 8, 8, 255, framebuffer, framebuffer_pitch);

	// wait a sec
	Wait_Clock(30);
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

	palette[1].peRed = 0;
	palette[1].peGreen = 16;
	palette[1].peBlue = 0;
	palette[1].peFlags = PC_NOCOLLAPSE;

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
