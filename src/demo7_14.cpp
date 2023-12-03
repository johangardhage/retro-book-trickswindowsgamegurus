//
// DEMO7_14.CPP 8-bit blitting demo with scaling
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// TYPES //////////////////////////////////////////////////////

// this will hold our little alien
typedef struct ALIEN_OBJ_TYP
{
	UCHAR *frames[3]; // 3 frames of animation for complete walk cycle
	int x, y;                        // position of alien
	int velocity;                   // x-velocity
	int current_frame;              // current frame of animation
	int counter;                    // used to time animation
	int width;
	int height;                     // size of surface
	float scale;                    // scale of object
} ALIEN_OBJ, *ALIEN_OBJ_PTR;

// GLOBALS ////////////////////////////////////////////////

BITMAP_FILE bitmap, bitmap2;                // holds the bitmap
ALIEN_OBJ aliens[3];             // 3 aliens, one on each level

// FUNCTIONS ////////////////////////////////////////////////

int Draw_Surface_Scaled(UCHAR *source, // source surface to draw
	int x, int y,                 // position to draw at
	int width_src, int height_src,// size of source surface
	int width_dest, int height_dest,// size of dest surface
	UCHAR *dest,    // surface to draw the surface on
	int transparent = 1)          // transparency flag
{
	// draw the surface at the x,y defined by dest, send both the original
	// source size of surface, along with the desired size, if they are 
	// different then directdraw will scale the bitmap for you
	// note that we are sending
	// the size of the surface, we could query for it, but that takes time
	// basically, we are really lacking datastructure as this point, since
	// you would create a datastructure that keep important info about the
	// surface, so you did't have to query it from directdraw

	RECT dest_rect,   // the destination rectangle
		source_rect; // the source rectangle                             

	// fill in the destination rect
	dest_rect.left = x;
	dest_rect.top = y;
	dest_rect.right = x + width_dest - 1;
	dest_rect.bottom = y + height_dest - 1;

	// fill in the source rect
	source_rect.left = 0;
	source_rect.top = 0;
	source_rect.right = width_src - 1;
	source_rect.bottom = height_src - 1;

	// test transparency flag

	if (transparent) {
		Blit_Rect(source_rect, source, width_src, dest_rect, dest, framebuffer_pitch, 0);
	} // end if
	else {
		Blit_Rect(source_rect, source, width_src, dest_rect, dest, framebuffer_pitch);
	} // end if

	// return success
	return(1);
}

///////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// lookup for proper walking sequence
	static int animation_seq[4] = { 0,1,0,2 };

	// start the timing clock
	Start_Clock();

	// copy memory from double buffer to primary buffer
	memcpy((void *)framebuffer, (void *)bitmap.buffer, screen_width * screen_height);

	// move objects around
	for (int index = 0; index < 3; index++) {
		// move each object to the right at its given velocity
		aliens[index].x++; // =aliens[index].velocity;

		// test if off screen edge, and wrap around
		if (aliens[index].x > screen_width)
			aliens[index].x = -80;

		// animate bot
		if (++aliens[index].counter >= (8 - aliens[index].velocity)) {
			// reset counter
			aliens[index].counter = 0;

			// advance to next frame
			if (++aliens[index].current_frame > 3) {
				aliens[index].current_frame = 0;
			}
		}
	}

	// draw all the bots
	for (int index = 0; index < 3; index++) {
		// draw objects but with scaled destination size
		Draw_Surface_Scaled(aliens[index].frames[animation_seq[aliens[index].current_frame]],
			aliens[index].x, aliens[index].y,
			aliens[index].width, aliens[index].height,
			(float)aliens[index].width * aliens[index].scale,
			(float)aliens[index].height * aliens[index].scale,
			framebuffer);
	}

	// wait a sec
	Wait_Clock(30);
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// load the 8-bit image
	Load_Bitmap_File(&bitmap, "assets/alley8.bmp");
	Set_Palette(bitmap.palette);

	// initialize all the aliens

	// alien on level 1 of complex
	aliens[0].x = rand() % screen_width;
	aliens[0].y = 116 - 72;
	aliens[0].velocity = 2 + rand() % 4;
	aliens[0].current_frame = 0;
	aliens[0].counter = 0;
	aliens[0].width = 72; // set real size
	aliens[0].height = 80;
	aliens[0].scale = ((float)(1 + rand() % 20)) / 10; // scale from 0.1 to 2.0
	// fix up feet so they still contact floor
	aliens[0].y += (72 - aliens[0].scale * 72);

	// alien on level 2 of complex
	aliens[1].x = rand() % screen_width;
	aliens[1].y = 246 - 72;
	aliens[1].velocity = 2 + rand() % 4;
	aliens[1].current_frame = 0;
	aliens[1].counter = 0;
	aliens[1].width = 72; // set real size
	aliens[1].height = 80;
	aliens[1].scale = ((float)(1 + rand() % 20)) / 10; // scale from 0.1 to 2.0
	// fix up feet so they still contact floor
	aliens[1].y += (72 - aliens[1].scale * 72);

	// alien on level 3 of complex
	aliens[2].x = rand() % screen_width;
	aliens[2].y = 382 - 72;
	aliens[2].velocity = 2 + rand() % 4;
	aliens[2].current_frame = 0;
	aliens[2].counter = 0;
	aliens[2].width = 72; // set real size
	aliens[2].height = 80;
	aliens[2].scale = ((float)(1 + rand() % 20)) / 10; // scale from 0.1 to 2.0

	// fix up feet so they still contact floor
	aliens[2].y += (72 - aliens[2].scale * 72);

	// now load the bitmap containing the alien imagery
	// then scan the images out into the surfaces of alien[0]
	// and copy then into the other two, be careful of reference counts!

	// load the 8-bit image
	Load_Bitmap_File(&bitmap2, "assets/dedsp0.bmp");

	// create each surface and load bits
	for (int index = 0; index < 3; index++) {
		// create surface to hold image
		aliens[0].frames[index] = (unsigned char *)malloc(72 * 80);

		// now load bits...
		Scan_Image_Bitmap(&bitmap2,                 // bitmap file to scan image data from
			aliens[0].frames[index], // surface to hold data
			72, 80, index, 0);               // cell to scan image from    

	} // end for index

	// now for the tricky part. There is no need to create more surfaces with the same
	// data, so I'm going to copy the surface pointers member for member to each alien
	// however, be careful, since the reference counts do NOT go up, you still only need
	// to release() each surface once!

	for (int index = 0; index < 3; index++) {
		aliens[1].frames[index] = aliens[2].frames[index] = aliens[0].frames[index];
	}
}

/////////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// unload the bitmap file, we no longer need it
	Unload_Bitmap_File(&bitmap);
	Unload_Bitmap_File(&bitmap2);
}

/////////////////////////////////////////////////////////////
