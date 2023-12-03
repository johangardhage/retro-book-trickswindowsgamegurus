//
// DEMO7_11.CPP 16-bit bitmap loading demo
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
	// get video pointer to primary surfce
	USHORT *primary_buffer = (USHORT *)framebuffer;
	USHORT *bitmap_buffer = (USHORT *)bitmap.buffer;

	// process each line and copy it into the primary buffer
	for (int index_y = 0; index_y < screen_height; index_y++) {
		for (int index_x = 0; index_x < screen_width; index_x++) {
			USHORT pixel = bitmap_buffer[index_x + (index_y * bitmap.bitmapinfoheader.biWidth)];

			// write the pixel
			primary_buffer[index_x + (index_y * framebuffer_pitch >> 1)] = pixel;

		} // end for index_x

	} // end for index_y
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();

	// load the 24-bit image
	Load_Bitmap_File(&bitmap, "assets/bitmap24.bmp");
}

/////////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// unload the bitmap file, we no longer need it
	Unload_Bitmap_File(&bitmap);
}

/////////////////////////////////////////////////////////////
