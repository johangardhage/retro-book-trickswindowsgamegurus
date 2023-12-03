//
// DEMO7_15.CPP 8-bit color animation demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// GLOBALS ////////////////////////////////////////////////

BITMAP_FILE bitmap;                // holds the bitmap

// FUNCTIONS ////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// copy memory from double buffer to primary buffer
	memcpy((void *)framebuffer, (void *)bitmap.buffer, screen_width * screen_height);

	// animate the lights
	Blink_Colors(BLINKER_RUN, NULL, 0);

	// wait a sec
	Wait_Clock(30);
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// load the 8-bit image
	Load_Bitmap_File(&bitmap, "assets/starshipanim8.bmp");
	Set_Palette(bitmap.palette);

	// create the blinking lights

	BLINKER temp; // used to build up lights

	PALETTEENTRY red = { 255,0,0,PC_NOCOLLAPSE };
	PALETTEENTRY green = { 0,255,0,PC_NOCOLLAPSE };
	PALETTEENTRY black = { 0,0,0,PC_NOCOLLAPSE };

	// add red light
	temp.color_index = 253;
	temp.on_color = red;
	temp.off_color = black;
	temp.on_time = 30; // 30 cycles at 30fps = 1 sec
	temp.off_time = 30;

	// make call, note use of C++ call by reference for 2nd parm
	Blink_Colors(BLINKER_ADD, &temp, 0);

	// now create green light
	temp.color_index = 254;
	temp.on_color = green;
	temp.off_color = black;
	temp.on_time = 90; // 30 cycles at 30fps = 3 secs
	temp.off_time = 90;

	// make call, note use of C++ call by reference for 2nd parm
	Blink_Colors(BLINKER_ADD, &temp, 0);
}

/////////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// unload the bitmap file, we no longer need it
	Unload_Bitmap_File(&bitmap);
}

/////////////////////////////////////////////////////////////
