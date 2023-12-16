//
// DEMO13_6.CPP - simple rectangular region collision demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

#define NUM_BALLS       10   // number of pool balls

// extents of table
#define TABLE_MAX_X     468
#define TABLE_MIN_X     176
#define TABLE_MAX_Y     448
#define TABLE_MIN_Y     44

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background
BOB          balls[NUM_BALLS]; // the balls

int ball_ids[8];               // sound ids for balls

// FUNCTIONS //////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// move all the balls
	for (int index = 0; index < NUM_BALLS; index++) {
		// move the ball
		Move_BOB(&balls[index]);

		// test for collision with table edges
		if ((balls[index].x + balls[index].width >= TABLE_MAX_X) ||
			(balls[index].x <= TABLE_MIN_X)) {
			// invert velocity
			balls[index].xv = -balls[index].xv;

		} // end if

		if ((balls[index].y + balls[index].height >= TABLE_MAX_Y) ||
			(balls[index].y <= TABLE_MIN_Y)) {
			// invert velocity
			balls[index].yv = -balls[index].yv;

		} // end if

	} // end for index

	// draw the balls
	for (int index = 0; index < NUM_BALLS; index++)
		Draw_BOB(&balls[index], framebuffer, framebuffer_pitch);

	// draw the title
	Draw_Text_GDI("ELASTIC COLLISION DEMO", 10, 10, RGB(255, 255, 255), framebuffer, framebuffer_pitch);

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
	Load_Bitmap_File(&bitmap8bit, "assets/ptable8.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480);
	Load_Image_Bitmap(&background_bmp, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Set_Palette(bitmap8bit.palette);
	Unload_Bitmap_File(&bitmap8bit);

	// load the bitmaps
	Load_Bitmap_File(&bitmap8bit, "assets/poolballs8.bmp");

	// create master ball
	Create_BOB(&balls[0], 0, 0, 24, 24, 6, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0);

	// load the imagery in
	for (int index = 0; index < 6; index++)
		Load_Frame_BOB(&balls[0], &bitmap8bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// create all the clones
	for (int index = 1; index < NUM_BALLS; index++)
		Clone_BOB(&balls[0], &balls[index]);

	// now set the initial conditions of all the balls
	for (int index = 0; index < NUM_BALLS; index++) {
		// set position randomly
		balls[index].x = 320 - 50 + rand() % 100;
		balls[index].y = 240 - 100 + rand() % 200;

		// set initial velocity
		balls[index].xv = -6 + rand() % 13;
		balls[index].yv = -6 + rand() % 13;

		// set ball
		balls[index].curr_frame = rand() % 6;

	} // end for index

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
	for (int index = 0; index < NUM_BALLS; index++)
		Destroy_BOB(&balls[index]);
}

//////////////////////////////////////////////////////////
