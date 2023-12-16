//
// DEMO12_2_16b.CPP - tracking demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background

BOB          bat,              // the ai bat bob
ghost,            // the player ghost bob
arch;             // to do the 3d arch effect

static int ghost_anim[] = { 0,1,2,1 }; // animation sequence for ghost

// FUNCTIONS //////////////////////////////////////////////

void Bat_AI(void)
{
	// this function performs the bat ai

	// do tracking algorithm

	// first x-axis    
	if (ghost.x > bat.x)
		bat.x += 2;
	else
		if (ghost.x < bat.x)
			bat.x -= 2;

	// now y-axis
	if (ghost.y > bat.y)
		bat.y += 2;
	else
		if (ghost.y < bat.y)
			bat.y -= 2;

	// check boundaries
	if (bat.x >= screen_width)
		bat.x = -bat.width;
	else
		if (bat.x < -bat.width)
			bat.x = screen_width;

	if (bat.y >= screen_height)
		bat.y = -bat.height;
	else
		if (bat.y < -bat.height)
			bat.y = screen_height;

} // end Bat_AI

//////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap16(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// call the bat AI
	Bat_AI();

	// the animate the bat
	Animate_BOB(&bat);

	// draw the bat
	Draw_BOB16(&bat, framebuffer, framebuffer_pitch);

	// allow player to move
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT))
		ghost.x += 4;
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT))
			ghost.x -= 4;

	if (RETRO_KeyState(SDL_SCANCODE_UP))
		ghost.y -= 4;
	else
		if (RETRO_KeyState(SDL_SCANCODE_DOWN))
			ghost.y += 4;

	// test if player is off screen, if so wrap around
	if (ghost.x >= screen_width)
		ghost.x = -ghost.width;
	else
		if (ghost.x < -ghost.width)
			ghost.x = screen_width;

	if (ghost.y >= screen_height)
		ghost.y = -ghost.height;
	else
		if (ghost.y < -ghost.height)
			ghost.y = screen_height;

	// the animate the ghost
	Animate_BOB(&ghost);

	// draw the ghost
	Draw_BOB16(&ghost, framebuffer, framebuffer_pitch);

	// draw the arches to create a 3D effect
	Set_Pos_BOB(&arch, 18, 270);
	Draw_BOB16(&arch, framebuffer, framebuffer_pitch);

	Set_Pos_BOB(&arch, 496, 270);
	Draw_BOB16(&arch, framebuffer, framebuffer_pitch);

	// draw title
	Draw_Text_GDI16("(16-Bit Version) GOING BATS! - Tracking Demo", 10, 10, RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);

	// sync to 30ish fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();

	// load background image
	Load_Bitmap_File(&bitmap16bit, "assets/ghostback_24.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&background_bmp, &bitmap16bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// load the bat bitmaps
	Load_Bitmap_File(&bitmap16bit, "assets/bats8_2_24.bmp");

	// create bat bob
	Create_BOB(&bat, 320, 200, 16, 16, 5, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0, 0, 16);
	Set_Anim_Speed_BOB(&bat, 2);

	// load the bat in 
	for (int index = 0; index < 5; index++)
		Load_Frame_BOB16(&bat, &bitmap16bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// create the arch bob
	Create_BOB(&arch, 320, 200, 128, 152, 1, BOB_ATTR_SINGLE_FRAME | BOB_ATTR_VISIBLE, 0, 0, 16);

	// load the arch
	Load_Frame_BOB16(&arch, &bitmap16bit, 0, 1, 18, BITMAP_EXTRACT_MODE_ABS);

	// unload arch and bat bitmap
	Unload_Bitmap_File(&bitmap16bit);


	// load the ghost bitmaps
	Load_Bitmap_File(&bitmap16bit, "assets/ghosts8_24.bmp");

	// create ghost bob
	Create_BOB(&ghost, 100, 200, 64, 100, 3, BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, 0, 0, 16);
	Set_Anim_Speed_BOB(&ghost, 10);

	// load the ghost in 
	for (int index = 0; index < 3; index++)
		Load_Frame_BOB16(&ghost, &bitmap16bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// unload ghost
	Unload_Bitmap_File(&bitmap16bit);

	// set animation for ghost
	Load_Animation_BOB(&ghost, 0, 4, ghost_anim);

	// set the animation
	Set_Animation_BOB(&ghost, 0);
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the bobs
	Destroy_BOB(&bat);
	Destroy_BOB(&ghost);
}

//////////////////////////////////////////////////////////
