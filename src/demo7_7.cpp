//
// DEMO7_7.CPP 16-bit surface to surface blitter demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// GLOBALS ////////////////////////////////////////////////

unsigned char *backbuffer = NULL;

// FUNCTIONS //////////////////////////////////////////////

void DEMO_Render2(double deltatime)
{
	RECT source_rect, // used to hold the destination RECT
		dest_rect;  // used to hold the destination RECT

	// get a random rectangle for source
	int x1 = rand() % screen_width;
	int y1 = rand() % screen_height;
	int x2 = rand() % screen_width;
	int y2 = rand() % screen_height;

	// get a random rectangle for destination
	int x3 = rand() % screen_width;
	int y3 = rand() % screen_height;
	int x4 = rand() % screen_width;
	int y4 = rand() % screen_height;

	// now set up the RECT structure to fill the region from
	// (x1,y1) to (x2,y2) on the source surface
	source_rect.left = x1;
	source_rect.top = y1;
	source_rect.right = x2;
	source_rect.bottom = y2;

	// now set up the RECT structure to fill the region from
	// (x3,y3) to (x4,y4) on the destination surface
	dest_rect.left = x3;
	dest_rect.top = y3;
	dest_rect.right = x4;
	dest_rect.bottom = y4;

	Blit_Rect16(source_rect, backbuffer, framebuffer_pitch, dest_rect, framebuffer, framebuffer_pitch);

	RETRO_Flip();
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();

	backbuffer = (unsigned char *)malloc(RETRO.framebuffersize);

	// get alias to start of surface memory for fast addressing
	USHORT *video_buffer = (USHORT *)backbuffer;

	// draw the gradient
	for (int index_y = 0; index_y < screen_height; index_y++) {
		// build color word up
		DWORD color = _RGB16BIT565(0, (index_y >> 3), 0);

		// replicate color in upper and lower 16 bits of 32-bit word 
		color = (color) | (color << 16);

		// now color has two pixel in it in 16.16 or RGB.RGB format, use a DWORD
		// or 32-bit copy to move the bytes into the next video line, we'll need
		// inline assembly though...
		for (int i = 0; i < screen_width; i++) {
			video_buffer[i] = color;
		}

		// now advance video_buffer to next line
		video_buffer += (framebuffer_pitch >> 1);
	} // end for index_y
}

/////////////////////////////////////////////////////////////
