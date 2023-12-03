//
// DEMO7_2.CPP basic full-screen 24-bit color pixel plotting DirectDraw demo
// you must have 24-bit graphics available otherwise this demo will simply exit
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// FUNCTIONS //////////////////////////////////////////////

inline void Plot_Pixel_24(int x, int y,
	int red, int green, int blue,
	UCHAR *video_buffer, int lpitch)
{
	// this function plots a pixel in 24-bit color mode
	// assuming that the caller already locked the surface
	// and is sending a pointer and byte pitch to it

	// in byte or 8-bit math the proper address is: 3*x + y*lpitch
	// this is the address of the low order byte which is the Blue channel
	// since the data is in RGB order
	DWORD pixel_addr = (x + x + x) + y * lpitch;

	// write the data, first blue
	video_buffer[pixel_addr] = blue;

	// now red
	video_buffer[pixel_addr + 1] = green;

	// finally green
	video_buffer[pixel_addr + 2] = red;

} // end Plot_Pixel_24

////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// make a couple aliases to make code cleaner, so we don't
	// have to cast
	int lpitch = framebuffer_pitch;
	UCHAR *video_buffer = (UCHAR *)framebuffer;

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
		Plot_Pixel_24(x, y, red, green, blue, video_buffer, lpitch);

	} // end for index
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init24();
}

///////////////////////////////////////////////////////////
