//
// DEMO9_4.CPP - DirectInput Force Feedback Demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/retromouse.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib2.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

#define BUTTON_SPRAY    0    // defines for each button
#define BUTTON_PENCIL   1
#define BUTTON_ERASE    2
#define BUTTON_EXIT     3

// GLOBALS ////////////////////////////////////////////////

// windows vars
char buffer[80];                // used to print text

// demo globals

BITMAP_IMAGE playfield;       // used to hold playfield
BITMAP_IMAGE mushrooms[4];    // holds mushrooms
BOB          blaster;         // holds bug blaster

int blaster_anim[5] = { 0,1,2,1,0 };  // blinking animation

// lets use a line segment for the missle
int missile_x[32],              // position of missle
missile_y[32],
missile_state[32];          // state of missle 0 off, 1 on

// FUNCTIONS /////////////////////////////////////////////////

int Start_Missile(void)
{
	// this function starts the missle, if it is currently off

	for (int index = 0; index < 32; index++) {
		if (missile_state[index] == 0) {
			// enable missile
			missile_state[index] = 1;

			// computer position of missile
			missile_x[index] = blaster.x + 16;
			missile_y[index] = blaster.y - 4;

			// return success
			return(1);
		} // end if

	} // end for

	// couldn't start missile
	return(0);

} // end Start_Missile

///////////////////////////////////////////////////////////

int Move_Missile(void)
{
	// this function moves the missle 

	// test if missile is alive
	for (int index = 0; index < 32; index++) {
		if (missile_state[index] == 1) {
			// move the missile upward
			if ((missile_y[index] -= 10) < 0) {
				missile_state[index] = 0;
				continue;
			} // end if

			// add missile collision here

		} // end if

	} // end for

	// return failure
	return(0);

} // end Move_Missle

///////////////////////////////////////////////////////////

int Draw_Missile(void)
{
	// this function draws the missile 

	// test if missile is alive
	for (int index = 0; index < 32; index++) {
		if (missile_state[index] == 1) {
			// draw the missile in green
			Draw_Clip_Line(missile_x[index], missile_y[index],
				missile_x[index], missile_y[index] + 6,
				250, framebuffer, framebuffer_pitch);

		} // end if

	} // end for

	// return failure
	return(0);

} // end Draw_Missle

///////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw the background reactor image
	Draw_Bitmap(&playfield, framebuffer, framebuffer_pitch, 0);

	// read mouse
	DInput_Read_Mouse();

	// is the player moving?
	blaster.x += mouse_state.lX;
	blaster.y += mouse_state.lY;

	// test bounds
	if (blaster.x > screen_width - 32)
		blaster.x = screen_width - 32;
	else
		if (blaster.x < 0)
			blaster.x = 0;

	if (blaster.y > screen_height - 32)
		blaster.y = screen_height - 32;
	else
		if (blaster.y < screen_height - 128)
			blaster.y = screen_height - 128;

	// is player firing?
	if (mouse_state.rgbButtons[0])
		Start_Missile();

	// move and draw missle
	Move_Missile();
	Draw_Missile();

	// is it time to blink eyes
	if ((rand() % 100) == 50)
		Set_Animation_BOB(&blaster, 0);

	// draw blaster
	Animate_BOB(&blaster);
	Draw_BOB(&blaster, framebuffer, framebuffer_pitch);

	// draw some text
	Draw_Text_GDI("Let's Rock!!!", 0, 0, RGB(255, 255, 255), framebuffer, framebuffer_pitch);

	// sync to 30 fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();
	RETRO_SetMouseMode(true);

	// load the background
	Load_Bitmap_File(&bitmap8bit, "assets/mush.bmp");

	// set the palette to background image palette
	Set_Palette(bitmap8bit.palette);

	// load in the four frames of the mushroom
	for (int index = 0; index < 4; index++) {
		// create mushroom bitmaps
		Create_Bitmap(&mushrooms[index], 0, 0, 32, 32);
		Load_Image_Bitmap(&mushrooms[index], &bitmap8bit, index, 0, BITMAP_EXTRACT_MODE_CELL);
	} // end for index

	// now create the bug blaster bob
	Create_BOB(&blaster, 0, 0, 32, 32, 3,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM | BOB_ATTR_ANIM_ONE_SHOT,
		0);

	// load in the four frames of the mushroom
	for (int index = 0; index < 3; index++)
		Load_Frame_BOB(&blaster, &bitmap8bit, index, index, 1, BITMAP_EXTRACT_MODE_CELL);

	// unload the bitmap file
	Unload_Bitmap_File(&bitmap8bit);

	// set the animation sequences for bug blaster
	Load_Animation_BOB(&blaster, 0, 5, blaster_anim);

	// set up stating state of bug blaster
	Set_Pos_BOB(&blaster, 320, 400);
	Set_Anim_Speed_BOB(&blaster, 3);

	// create mushroom playfield bitmap
	Create_Bitmap(&playfield, 0, 0, screen_width, screen_height);
	playfield.attr |= BITMAP_ATTR_LOADED;

	// fill in the background
	Load_Bitmap_File(&bitmap8bit, "assets/grass.bmp");

	// load the grass bitmap image
	Load_Image_Bitmap(&playfield, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap8bit);

	// seed random number generator
	srand(Start_Clock());

	// create the random mushroom patch
	for (int index = 0; index < 50; index++) {
		// select a mushroom
		int mush = rand() % 4;

		// set mushroom to random position
		mushrooms[mush].x = rand() % (screen_width - 32);
		mushrooms[mush].y = rand() % (screen_height - 128);

		// now draw the mushroom into playfield
		Draw_Bitmap(&mushrooms[mush], playfield.buffer, playfield.width, 1);

	} // end for
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill the bug blaster
	Destroy_BOB(&blaster);

	// kill the mushroom maker
	for (int index = 0; index < 4; index++)
		Destroy_Bitmap(&mushrooms[index]);

	// kill the playfield bitmap
	Destroy_Bitmap(&playfield);
}

///////////////////////////////////////////////////////////
