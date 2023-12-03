//
// DEMO7_8.CPP 8-bit software bitmap clipper demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// TYPES //////////////////////////////////////////////////////

// the happy face structure
typedef struct HAPPY_FACE_TYP
{
	int x, y;           // position of happy face
	int xv, yv;        // velocity of happy face
} HAPPY_FACE, *HAPPY_FACE_PTR;

// GLOBALS ////////////////////////////////////////////////

// a low tech bitmap that uses palette entry 1 for the color :)
UCHAR happy_bitmap[64] = { 0,0,0,0,0,0,0,0,
						  0,0,1,1,1,1,0,0,
						  0,1,0,1,1,0,1,0,
						  0,1,1,1,1,1,1,0,
						  0,1,0,1,1,0,1,0,
						  0,1,1,0,0,1,1,0,
						  0,0,1,1,1,1,0,0,
						  0,0,0,0,0,0,0,0 };

UCHAR sad_bitmap[64] = { 0,0,0,0,0,0,0,0,
						0,0,1,1,1,1,0,0,
						0,1,0,1,1,0,1,0,
						0,1,1,1,1,1,1,0,
						0,1,1,0,0,1,1,0,
						0,1,0,1,1,0,1,0,
						0,0,1,1,1,1,0,0,
						0,0,0,0,0,0,0,0 };

HAPPY_FACE happy_faces[100]; // this holds all the happy faces

// FUNCTIONS //////////////////////////////////////////////

void Blit_Clipped(int x, int y,          // position to draw bitmap
	int width, int height, // size of bitmap in pixels
	UCHAR *bitmap,         // pointer to bitmap data
	UCHAR *video_buffer,   // pointer to video buffer surface
	int   mempitch)        // video pitch per line
{
	// this function blits and clips the image sent in bitmap to the 
	// destination surface pointed to by video_buffer
	// the function assumes a 640x480x8 mode 

	// first do trivial rejections of bitmap, is it totally invisible?
	if ((x >= screen_width) || (y >= screen_height) ||
		((x + width) <= 0) || ((y + height) <= 0))
		return;

	// clip source rectangle
	// pre-compute the bounding rect to make life easy
	int x1 = x;
	int y1 = y;
	int x2 = x1 + width - 1;
	int y2 = y1 + height - 1;

	// upper left hand corner first
	if (x1 < 0)
		x1 = 0;

	if (y1 < 0)
		y1 = 0;

	// now lower left hand corner
	if (x2 >= screen_width)
		x2 = screen_width - 1;

	if (y2 >= screen_height)
		y2 = screen_height - 1;

	// now we know to draw only the portions of the bitmap from (x1,y1) to (x2,y2)
	// compute offsets into bitmap on x,y axes, we need this to compute starting point
	// to rasterize from
	int x_off = x1 - x;
	int y_off = y1 - y;

	// compute number of columns and rows to blit
	int dx = x2 - x1 + 1;
	int dy = y2 - y1 + 1;

	// compute starting address in video_buffer 
	video_buffer += (x1 + y1 * mempitch);

	// compute starting address in bitmap to scan data from
	bitmap += (x_off + y_off * width);

	// at this point bitmap is pointing to the first pixel in the bitmap that needs to
	// be blitted, and video_buffer is pointing to the memory location on the destination
	// buffer to put it, so now enter rasterizer loop

	UCHAR pixel; // used to read/write pixels

	for (int index_y = 0; index_y < dy; index_y++) {
		// inner loop, where the action takes place
		for (int index_x = 0; index_x < dx; index_x++) {
			// read pixel from source bitmap, test for transparency and plot
			if ((pixel = bitmap[index_x]))
				video_buffer[index_x] = pixel;

		} // end for index_x

		// advance pointers
		video_buffer += mempitch;  // bytes per scanline
		bitmap += width;     // bytes per bitmap row

	} // end for index_y

} // end Blit_Clipped

////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	static int feeling_counter = 0;   // tracks how we feel :)
	static int happy = 1;             // let's start off being happy

	// increment how we feel
	if (++feeling_counter > 200) {
		feeling_counter = 0;
		happy = -happy;
	} // end if

	// draw all the happy faces
	for (int face = 0; face < 100; face++) {
		// are we happy or sad?
		if (happy == 1) // we are happy :)
			Blit_Clipped(happy_faces[face].x,
				happy_faces[face].y,
				8, 8,
				happy_bitmap,
				(UCHAR *)framebuffer,
				framebuffer_pitch);
		else // we must be sad :(
			Blit_Clipped(happy_faces[face].x,
				happy_faces[face].y,
				8, 8,
				sad_bitmap,
				(UCHAR *)framebuffer,
				framebuffer_pitch);
	} // end face

	// move all happy faces
	for (int face = 0; face < 100; face++) {
		// move
		happy_faces[face].x += happy_faces[face].xv;
		happy_faces[face].y += happy_faces[face].yv;

		// check for off screen, if so wrap
		if (happy_faces[face].x > screen_width)
			happy_faces[face].x = -8;
		else
			if (happy_faces[face].x < -8)
				happy_faces[face].x = screen_width;

		if (happy_faces[face].y > screen_height)
			happy_faces[face].y = -8;
		else
			if (happy_faces[face].y < -8)
				happy_faces[face].y = screen_height;
	} // end face
}

////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// build up the palette data array
	for (int color = 1; color < 255; color++) {
		// fill with random RGB values
		palette[color].peRed = rand() % 256;
		palette[color].peGreen = rand() % 256;
		palette[color].peBlue = rand() % 256;

		// set flags field to PC_NOCOLLAPSE
		palette[color].peFlags = PC_NOCOLLAPSE;
	} // end for color

	// now fill in entry 0 and 255 with black and white
	palette[0].peRed = 0;
	palette[0].peGreen = 0;
	palette[0].peBlue = 0;
	palette[0].peFlags = PC_NOCOLLAPSE;

	palette[255].peRed = 255;
	palette[255].peGreen = 255;
	palette[255].peBlue = 255;
	palette[255].peFlags = PC_NOCOLLAPSE;

	// make color 1 yellow
	palette[1].peRed = 255;
	palette[1].peGreen = 255;
	palette[1].peBlue = 0;
	palette[1].peFlags = PC_NOCOLLAPSE;

	// initialize all the happy faces
	for (int face = 0; face < 100; face++) {
		// set random position
		happy_faces[face].x = rand() % screen_width;
		happy_faces[face].y = rand() % screen_height;

		// set random velocity (-2,+2)
		happy_faces[face].xv = -2 + rand() % 5;
		happy_faces[face].yv = -2 + rand() % 5;

	} // end for face

	Set_Palette(palette);
}
