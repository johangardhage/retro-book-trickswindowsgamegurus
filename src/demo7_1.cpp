//
// DEMO7_1.CPP basic full-screen 16-bit color pixel plotting DirectDraw demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// FUNCTIONS //////////////////////////////////////////////

inline void Plot_Pixel_Faster16(int x, int y,
	int red, int green, int blue,
	USHORT *video_buffer, int lpitch16)
{
	// this function plots a pixel in 16-bit color mode
	// assuming that the caller already locked the surface
	// and is sending a pointer and byte pitch to it

	// first build up color WORD
	USHORT pixel = _RGB16BIT565(red, green, blue);

	// write the data
	video_buffer[x + y * lpitch16] = pixel;

} // end Plot_Pixel_Faster16

////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	int lpitch16 = framebuffer_pitch >> 1;
	USHORT *video_buffer = (USHORT *)framebuffer;

	// plot 1000 random pixels with random colors on the
	// primary surface, they will be instantly visible
	for (int index = 0; index < 1000; index++) {
		// select random position and color for 640x480x16
		int red = rand() % 256;
		int green = rand() % 256;
		int blue = rand() % 256;
		int x = rand() % screen_width;
		int y = rand() % screen_height;

		// plot the pixel
		Plot_Pixel_Faster16(x, y, red, green, blue, video_buffer, lpitch16);

	} // end for index
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();
}

///////////////////////////////////////////////////////////
