//
// DEMO13_1.CPP - constant velocity demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

#define NUM_CHOPPERS    24   // number of choppers in simulation

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background
BOB          chopper[NUM_CHOPPERS];  // the fleet

// FUNCTIONS //////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// scroll the background
	Scroll_Bitmap(&background_bmp, -1);

	// move and animate the fleet
	for (int index = 0; index < NUM_CHOPPERS; index++) {
		// move the chopper
		Move_BOB(&chopper[index]);

		// test for off the screen wrap around
		if (chopper[index].x > screen_width)
			chopper[index].x = -chopper[index].width;

		// animate the bat
		Animate_BOB(&chopper[index]);

	} // end for index

	// draw the choppers
	for (int index = NUM_CHOPPERS - 1; index >= 0; index--)
		Draw_Scaled_BOB(&chopper[index], chopper[index].varsI[0], chopper[index].varsI[1], framebuffer, framebuffer_pitch);

	// draw the title
	Draw_Text_GDI("CONSTANT VELOCITY DEMO", 10, 10, RGB(0, 200, 200), framebuffer, framebuffer_pitch);

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
	Load_Bitmap_File(&bitmap8bit, "assets/skyscroll8.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480);
	Load_Image_Bitmap(&background_bmp, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Set_Palette(bitmap8bit.palette);
	Unload_Bitmap_File(&bitmap8bit);

	// load the bitmaps
	Load_Bitmap_File(&bitmap8bit, "assets/chop8.bmp");

	// create bob
	Create_BOB(&chopper[0], 0, 120, 200, 64, 3, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0);

	// set animation speed
	Set_Anim_Speed_BOB(&chopper[0], 1);

	// set motion speed
	Set_Vel_BOB(&chopper[0], 2 + rand() % 6, 0);

	// set position
	Set_Pos_BOB(&chopper[0], rand() % screen_width, rand() % screen_height);

	// load the bob in 
	for (int index = 0; index < 3; index++)
		Load_Frame_BOB(&chopper[0], &bitmap8bit, index, 0, index, BITMAP_EXTRACT_MODE_CELL);

	// now make the remaining 15 clones
	for (int index = 1; index < NUM_CHOPPERS; index++) {
		// clone the bob
		Clone_BOB(&chopper[0], &chopper[index]);

		// set new position and velocity
		Set_Vel_BOB(&chopper[index], 2 + rand() % 6, 0);
		Set_Pos_BOB(&chopper[index], rand() % screen_width, rand() % screen_height);

		// now here the tricky part, alter the size of the bob, but make sure
		// to keep the aspect ratio the same, so it doesn't look scrunched
		float width = 200 - 10 * index;
		float height = (float)width * ((float)64 / (float)200);

		// store new width and height in varsIable cache positions 0,1
		chopper[index].varsI[0] = width;
		chopper[index].varsI[1] = height;

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
	for (int index = 0; index < NUM_CHOPPERS; index++)
		Destroy_BOB(&chopper[index]);
}

//////////////////////////////////////////////////////////
