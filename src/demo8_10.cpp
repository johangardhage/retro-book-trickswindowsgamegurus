//
// DEMO8_10.CPP - Page scrolling demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

// skelaton directions
#define SKELATON_EAST         0
#define SKELATON_NEAST        1  
#define SKELATON_NORTH        2
#define SKELATON_NWEST        3
#define SKELATON_WEST         4
#define SKELATON_SWEST        5
#define SKELATON_SOUTH        6
#define SKELATON_SEAST        7

#define START_GLOW_COLOR  152 // starting color index to glow
#define END_GLOW_COLOR    159 // ending color index to glow
#define FLOOR_COLOR       116

// GLOBALS ////////////////////////////////////////////////

BOB          skelaton;     // the player skelaton

// animation sequences for bob
int skelaton_anims[8][4] = { {0,1,0,2},
							 {0 + 4,1 + 4,0 + 4,2 + 4},
							 {0 + 8,1 + 8,0 + 8,2 + 8},
							 {0 + 12,1 + 12,0 + 12,2 + 12},
							 {0 + 16,1 + 16,0 + 16,2 + 16},
							 {0 + 20,1 + 20,0 + 20,2 + 20},
							 {0 + 24,1 + 24,0 + 24,2 + 24},
							 {0 + 28,1 + 28,0 + 28,2 + 28}, };

BITMAP_IMAGE landscape[3];      // the background    landscape 3 pages 3x1
int curr_page = 0;              // current page of landscape

// FUNCTIONS ////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	int dx;             // general deltas used in collision detection

	static int   player_moving = 0; // tracks player motion
	static PALETTEENTRY glow = { 0,0,0,PC_NOCOLLAPSE };  // used to animation green border

	// start the timing clock
	Start_Clock();

	// draw the background reactor image
	Draw_Bitmap(&landscape[curr_page], framebuffer, framebuffer_pitch, 0);

	// reset motion flag
	player_moving = 0;

	// test direction of motion
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		// move skelaton
		skelaton.x += 2;
		dx = 2;

		// set motion flag
		player_moving = 1;

		// check animation needs to change
		if (skelaton.curr_animation != SKELATON_EAST)
			Set_Animation_BOB(&skelaton, SKELATON_EAST);

	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
			// move skelaton
			skelaton.x -= 2;
			dx = -2;

			// set motion flag
			player_moving = 1;

			// check animation needs to change
			if (skelaton.curr_animation != SKELATON_WEST)
				Set_Animation_BOB(&skelaton, SKELATON_WEST);

		} // end if

	 // apply downward gravity to player,
	 // so player follows terrain
	skelaton.y += 1;

	// only animate if player is moving
	if (player_moving) {
		// animate skelaton
		Animate_BOB(&skelaton);
	} // end if

	// call the color scanner with FLOOR_COLOR, the color of the glowing floor
	// try to center the scan on the feet of the player, make sure feet stay
	// in contact with floor
	while (Color_Scan(skelaton.x + 16, skelaton.y + 24,
		skelaton.x + skelaton.width - 32, skelaton.y + skelaton.height - 12,
		FLOOR_COLOR, FLOOR_COLOR, framebuffer, framebuffer_pitch)) {
		// push the skelaton upward, to keep it on the floor
		skelaton.y -= 1;

	} // end while

	// draw the skelaton
	Draw_BOB(&skelaton, framebuffer, framebuffer_pitch);

	// this performs the animation for the glowing rock
	static int glow_count = 0;

	// increment action counter and test
	if (++glow_count > 5) {
		// rotate the colors
		Rotate_Colors(152, 159);
		// reset the counter
		glow_count = 0;
	} // end if

	// animate the floor
	glow.peGreen = rand() % 256;
	Set_Palette_Entry(FLOOR_COLOR, &glow);

	// draw some text
	Draw_Text_GDI("USE ARROW KEYS TO MOVE RIGHT & LEFT, <ESC> to Exit.", 8, 8, RGB(255, 255, 255), framebuffer, framebuffer_pitch);

	// test for page flip right
	if (skelaton.x > screen_width - (skelaton.width >> 1)) {
		// bump back regardless
		skelaton.x -= dx;

		// test for page flip
		if (curr_page < 2) {
			// scroll to next page to right
			curr_page++;

			// reset character to left edge of screen
			skelaton.x = -(skelaton.width >> 1);

		} // end if

	} // end if   
	else // page flip right?
		if (skelaton.x < -(skelaton.width >> 1)) {
			// bump back regardless
			skelaton.x = -(skelaton.width >> 1);

			// test for page flip
			if (curr_page > 0) {
				// scroll to next page to left
				curr_page--;

				// reset character to right edge of screen
				skelaton.x = screen_width - (skelaton.width >> 1);

			} // end if

		} // end if   

	// sync to 30 fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	char filename[80]; // used to build up files names

	// load in each page of the scrolling background
	for (int index = 0; index < 3; index++) {
		// build up file name
		sprintf(filename, "assets/landscape%d.bmp", index + 1);

		// load the background
		Load_Bitmap_File(&bitmap8bit, filename);

		// create and load the reactor bitmap image
		Create_Bitmap(&landscape[index], 0, 0, 640, 480);
		Load_Image_Bitmap(&landscape[index], &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
		Unload_Bitmap_File(&bitmap8bit);

	} // end for index

	// set the palette to background image palette
	Set_Palette(bitmap8bit.palette);

	// now let's load in all the frames for the skelaton!!!

	// create skelaton bob
	Create_BOB(&skelaton, 0, 0, 72, 74, 32, // 56x72
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM, 0);

	// load the frames in 8 directions, 4 frames each
	// each set of frames has a walk and a fire, frame sets
	// are loaded in counter clockwise order looking down
	// from a birds eys view or the x-z plane
	for (int direction = 0; direction < 8; direction++) {
		// build up file name
		sprintf(filename, "assets/quensp%d.bmp", direction); // skelsp

		// load in new bitmap file
		Load_Bitmap_File(&bitmap8bit, filename);

		Load_Frame_BOB(&skelaton, &bitmap8bit, 0 + direction * 4, 0, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB(&skelaton, &bitmap8bit, 1 + direction * 4, 1, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB(&skelaton, &bitmap8bit, 2 + direction * 4, 2, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB(&skelaton, &bitmap8bit, 3 + direction * 4, 0, 1, BITMAP_EXTRACT_MODE_CELL);

		// unload the bitmap file
		Unload_Bitmap_File(&bitmap8bit);

		// set the animation sequences for skelaton
		Load_Animation_BOB(&skelaton, direction, 4, skelaton_anims[direction]);

	} // end for direction

	// set up stating state of skelaton
	Set_Animation_BOB(&skelaton, 0);
	Set_Anim_Speed_BOB(&skelaton, 4);
	Set_Vel_BOB(&skelaton, 0, 0);
	Set_Pos_BOB(&skelaton, 16, 256); // right above the floor

	// create lookup for lighting engine
	RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
		palette,             // source palette
		rgblookup);          // lookup table
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill the landscape
	for (int index = 0; index < 3; index++)
		Destroy_Bitmap(&landscape[index]);

	// kill skelaton
	Destroy_BOB(&skelaton);
}

///////////////////////////////////////////////////////////
