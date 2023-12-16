//
// DEMO13_5.CPP - simple friction demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

#define FRICTION_FACTOR     (float)(0.1)  // friction of table
#define PUCK_START_X        30            // starting location of puck
#define PUCK_START_Y        220
#define PUCK_STATE_RESTING  0             // puck is sitting
#define PUCK_STATE_MOVING   1             // puck has been hit

// extents of table
#define TABLE_MAX_X     620
#define TABLE_MIN_X     14
#define TABLE_MAX_Y     370
#define TABLE_MIN_Y     80

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background
BOB          puck;             // the ship

float friction = FRICTION_FACTOR; // frictional coefficient

// FUNCTIONS //////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// is player changing friction
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT))
		friction += 0.005;
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT))
			if ((friction -= 0.005) < 0)
				friction = 0.0;

	// check if player is hiting puck
	if (RETRO_KeyPressed(SDL_SCANCODE_RETURN) && puck.anim_state == PUCK_STATE_RESTING) {
		// first set state to motion
		puck.anim_state = PUCK_STATE_MOVING;

		// reset puck position
		puck.varsF[2] = PUCK_START_X;
		puck.varsF[3] = PUCK_START_Y;

		// select random initial trajectory
		puck.varsF[0] = 12 + rand() % 8;
		puck.varsF[1] = -8 + rand() % 17;

	} // end if

	// move puck
	puck.varsF[2] += puck.varsF[0];
	puck.varsF[3] += puck.varsF[1];

	///////////////////////////////////////////////////////////

	// apply friction in direction opposite current trajectory
	float fx = -puck.varsF[0];
	float fy = -puck.varsF[1];
	float length_f = sqrt(fx * fx + fy * fy); // normally we would avoid square root at all costs!

	// compute the frictional resitance

	if (fabs(length_f) >= 0.50) {
		fx = friction * fx / length_f;
		fy = friction * fy / length_f;
	} // end if
	else {
		// force 0.0
		fx = fy = 0.0;

		// kill velocity
		puck.varsF[0] = 0.0;
		puck.varsF[1] = 0.0;

		// puck is stuck, let player fire again
		puck.anim_state = PUCK_STATE_RESTING;
	} // end if

	// now apply friction to forward velocity
	puck.varsF[0] += fx;
	puck.varsF[1] += fy;

	// test if puck is off screen
	if (puck.varsF[2] + puck.width >= TABLE_MAX_X || puck.varsF[2] <= TABLE_MIN_X) {
		// invert velocity
		puck.varsF[0] = -puck.varsF[0];

		// move puck
		puck.varsF[2] += puck.varsF[0];
		puck.varsF[3] += puck.varsF[1];

	} // end if

	if (puck.varsF[3] + puck.height >= TABLE_MAX_Y || puck.varsF[3] <= TABLE_MIN_Y) {
		// invert velocity
		puck.varsF[1] = -puck.varsF[1];

		// move puck
		puck.varsF[2] += puck.varsF[0];
		puck.varsF[3] += puck.varsF[1];

	} // end if

	// copy floating point position to bob x,y
	puck.x = puck.varsF[2] + 0.5;
	puck.y = puck.varsF[3] + 0.5;

	// draw skid marks to table if puck is in motion
	if (fabs(puck.varsF[0]) > 0 || fabs(puck.varsF[1]) > 0) {
		Draw_Pixel(puck.x + puck.width / 2 - 2 + rand() % 4, puck.y + puck.height / 2 - 2 + rand() % 4, 16 + rand() % 8,
			background_bmp.buffer, background_bmp.width);
	} // end if

	// draw the puck
	Draw_BOB(&puck, framebuffer, framebuffer_pitch);

	// draw the title
	Draw_Text_GDI("SIMPLE FRICTION DEMO - Hit Space to Shoot Puck, Arrows to Change Friction.", 10, 10, RGB(0, 255, 255), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Friction: X=%f, Y=%f", fx, fy);
	Draw_Text_GDI(buffer, 10, 410, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Velocity: X=%f, Y=%f", puck.varsF[0], puck.varsF[1]);
	Draw_Text_GDI(buffer, 10, 430, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Frictional Coefficient: %f", friction);
	Draw_Text_GDI(buffer, 10, 450, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	// sync to 30 fps = 1/30sec = 33 ms
	Wait_Clock(33);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// seed random number generate
	srand(Start_Clock());

	// load background image
	Load_Bitmap_File(&bitmap8bit, "assets/hockey8.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480);
	Load_Image_Bitmap(&background_bmp, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Set_Palette(bitmap8bit.palette);
	Unload_Bitmap_File(&bitmap8bit);

	// load the bitmaps for ship
	Load_Bitmap_File(&bitmap8bit, "assets/puck8.bmp");

	// create bob
	Create_BOB(&puck, PUCK_START_X, PUCK_START_Y, 32, 32, 1, BOB_ATTR_SINGLE_FRAME | BOB_ATTR_VISIBLE, 0);

	// set state of puck
	puck.anim_state = PUCK_STATE_RESTING;

	// use varsF[0,1] for the x and y velocity
	puck.varsF[0] = 0;
	puck.varsF[1] = 0;

	// use varsF[2,3] for the x and y position, we need more accuracy than ints
	puck.varsF[2] = puck.x;
	puck.varsF[3] = puck.y;

	// load the frame
	Load_Frame_BOB(&puck, &bitmap8bit, 0, 0, 0, BITMAP_EXTRACT_MODE_CELL);

	// unload bitmap image
	Unload_Bitmap_File(&bitmap8bit);

	// create lookup for lighting engine
	RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
		palette,             // source palette
		rgblookup);          // lookup table
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the bobs
	Destroy_BOB(&puck);
}

//////////////////////////////////////////////////////////
