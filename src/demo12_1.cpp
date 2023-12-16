//
// DEMO12_1.CPP - fly brain demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

// defines for flys
#define MAX_FLYS        128

// bounding box for flys
#define MIN_X_FLY       190 
#define MAX_X_FLY       450
#define MIN_Y_FLY       350
#define MAX_Y_FLY       470

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background

BOB flys[MAX_FLYS];            // the flys

// FUNCTIONS //////////////////////////////////////////////

void Flys_AI(void)
{
	// this function performs the AI for the flys

	for (int curr_fly = 0; curr_fly < MAX_FLYS; curr_fly++) {
		// test if fly directional counter <= 0
		if (--flys[curr_fly].varsI[0] <= 0) {
			// select a new random directional velocity
			flys[curr_fly].xv = -4 + rand() % 9;
			flys[curr_fly].yv = -4 + rand() % 9;

			// set time for motion to occur
			flys[curr_fly].varsI[0] = 2 + rand() % 8;

		} // end if

		// move the fly
		Move_BOB(&flys[curr_fly]);

		// animate the fly
		Animate_BOB(&flys[curr_fly]);

		// test if fly has left the fresh meat
		if (flys[curr_fly].x > MAX_X_FLY) {
			flys[curr_fly].xv = -flys[curr_fly].xv;
			flys[curr_fly].x = MAX_X_FLY;
		} // end if

		if (flys[curr_fly].x < MIN_X_FLY) {
			flys[curr_fly].xv = -flys[curr_fly].xv;
			flys[curr_fly].x = MIN_X_FLY;
		} // end if

		if (flys[curr_fly].y > MAX_Y_FLY) {
			flys[curr_fly].yv = -flys[curr_fly].yv;
			flys[curr_fly].y = MAX_Y_FLY;
		} // end if

		if (flys[curr_fly].y < MIN_Y_FLY) {
			flys[curr_fly].yv = -flys[curr_fly].yv;
			flys[curr_fly].y = MIN_Y_FLY;
		} // end if

	} // end for curr_fly

} // end Flys_AI

//////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// process the fly ai, move them buzz around dead bodies
	Flys_AI();

	// draw the flys
	for (int index = 0; index < MAX_FLYS; index++)
		Draw_BOB(&flys[index], framebuffer, framebuffer_pitch);

	// sync to 30ish fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// load background image
	Load_Bitmap_File(&bitmap8bit, "assets/flyback.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480);
	Load_Image_Bitmap(&background_bmp, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Set_Palette(bitmap8bit.palette);
	Unload_Bitmap_File(&bitmap8bit);

	// load the fly bitmaps
	Load_Bitmap_File(&bitmap8bit, "assets/flys8.bmp");

	// create master fly bob
	Create_BOB(&flys[0], 320, 200, 8, 8, 4, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0);
	Set_Anim_Speed_BOB(&flys[0], 1);

	// load the fly in 
	for (int index = 0; index < 4; index++)
		Load_Frame_BOB(&flys[0], &bitmap8bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// unload flys
	Unload_Bitmap_File(&bitmap8bit);

	// now replicate flys
	for (int index = 1; index < MAX_FLYS; index++)
		Clone_BOB(&flys[0], &flys[index]);

	// now set all of their values
	for (int index = 0; index < MAX_FLYS; index++) {
		// set positions
		Set_Pos_BOB(&flys[index], 320 - 32 + rand() % 64, 450 - rand() % 32);

		// set start frame randomly
		flys[index].curr_frame = 0;

	} // end for index
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the flys
	for (int index = 0; index < MAX_FLYS; index++)
		Destroy_BOB(&flys[index]);
}

///////////////////////////////////////////////////////////
