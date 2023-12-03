//
// DEMO7_10.CPP 8-bit bitmap loading demo
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
	// copy memory from double buffer to primary buffer
	memcpy((void *)framebuffer, (void *)bitmap.buffer, screen_width * screen_height);
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// load the 8-bit image
	Load_Bitmap_File(&bitmap, "assets/bitmap8.bmp");

	// load it's palette
	Set_Palette(bitmap.palette);
}

/////////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// unload the bitmap file, we no longer need it
	Unload_Bitmap_File(&bitmap);
}

///////////////////////////////////////////////////////////
