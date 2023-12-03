//
// DEMO7_16.CPP 8-bit color rotation demo
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

	// animate the waterfall, the water color entries are from 133-143
	Rotate_Colors(133, 143);

	// wait a sec
	Wait_Clock(30);
}

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// load the 8-bit image
	Load_Bitmap_File(&bitmap, "assets/waterfall8.bmp");
	Set_Palette(bitmap.palette);
}

/////////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// unload the bitmap file, we no longer need it
	Unload_Bitmap_File(&bitmap);
}

/////////////////////////////////////////////////////////////
