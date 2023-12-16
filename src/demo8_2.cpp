//
// DEMO8_2.CPP 8-bit line clipping demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// FUNCTIONS ////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw 1000 random lines
	for (int index = 0; index < 1000; index++) {
		Draw_Clip_Line(rand() % screen_width, rand() % screen_height,
			rand() % screen_width, rand() % screen_height,
			rand() % 256,
			(UCHAR *)framebuffer, framebuffer_pitch);
	}

	// wait a sec
	Wait_Clock(30);
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	min_clip_x = ((screen_width / 2) - 100),      // clipping rectangle 
	max_clip_x = ((screen_width / 2) + 100),
	min_clip_y = ((screen_height / 2) - 100),
	max_clip_y = ((screen_height / 2) + 100);
}

/////////////////////////////////////////////////////////////
