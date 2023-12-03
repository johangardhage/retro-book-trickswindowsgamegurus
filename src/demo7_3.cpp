//
// DEMO7_3.CPP basic full-screen 32-bit color pixel plotting DirectDraw demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// FUNCTIONS //////////////////////////////////////////////

inline void Plot_Pixel_32(int x, int y,
	int alpha, int red, int green, int blue,
	UINT *video_buffer, int lpitch32)
{
	// this function plots a pixel in 32-bit color mode
	// assuming that the caller already locked the surface
	// and is sending a pointer and DWORD aligned pitch to it

	// first build up color WORD
	UINT pixel = _RGB32BIT(alpha, red, green, blue);

	// write the data
	video_buffer[x + y * lpitch32] = pixel;

} // end Plot_Pixel_32

////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// make a couple aliases to make code cleaner, so we don't
	// have to cast
	int lpitch32 = framebuffer_pitch >> 2;
	UINT *video_buffer = (UINT *)framebuffer;

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
		Plot_Pixel_32(x, y, 0, red, green, blue, video_buffer, lpitch32);

	} // end for index
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init32();
}

///////////////////////////////////////////////////////////
