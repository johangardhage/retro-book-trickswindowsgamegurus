//
// DEMO13_2.CPP - acceleration demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

// defines for rocket
#define ROCKET_STATE_ON_PAD    0
#define ROCKET_STATE_IN_FLIGHT 1

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background
BOB          rocket;           // the rocket

// FUNCTIONS //////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	static int debounce_a = 0; // used to debounce acceleration toggle key

	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// test if user is toggling acceleration control
	if (RETRO_KeyPressed(SDL_SCANCODE_A) && !debounce_a) {
		// toggle acceleration
		rocket.varsI[0] = -rocket.varsI[0];

		// debounce key, so it doesn't toggle 60 times!
		debounce_a = 1;

	} // end if

	if (!RETRO_KeyPressed(SDL_SCANCODE_A))
		debounce_a = 0;

	// test if user is changing acceleration
	if (RETRO_KeyState(SDL_SCANCODE_DOWN)) {
		// decrease acceleration factor   
		if ((rocket.varsF[1] -= 0.1) < 0)
			rocket.varsF[1] = 0.1;
	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_UP)) {
			// increase acceleration factor   
			rocket.varsF[1] += 0.1;
		} // end if

	 // test if player is firing rocket
	if (RETRO_KeyState(SDL_SCANCODE_RETURN) && rocket.anim_state == ROCKET_STATE_ON_PAD) {
		// fire the rocket
		rocket.anim_state = ROCKET_STATE_IN_FLIGHT;

	} // end if

	// test state of rocket
	if (rocket.anim_state == ROCKET_STATE_ON_PAD) {
		// make sure there is no animation and frame is 0
		rocket.curr_frame = 0;

	} // end if
	else // rocket is in flight
	{
		// animate the rocket
		Animate_BOB(&rocket);

		// move the rocket

		// update the position with the current velocity
		rocket.y -= (int)(rocket.varsF[0] + 0.5);

		// update velocity with acceleration (if acceleration is on)
		if (rocket.varsI[0] == 1)
			rocket.varsF[0] += rocket.varsF[1];

		// test if the rocket has moved off the screen
		if (rocket.y < -4 * rocket.height) {
			// reset everything
			rocket.anim_state = ROCKET_STATE_ON_PAD;
			rocket.curr_frame = 0;
			rocket.varsF[0] = 0;

			// reset position
			Set_Pos_BOB(&rocket, 312, 420);

		} // end if

	} // end else

	// draw the rocket
	Draw_BOB(&rocket, framebuffer, framebuffer_pitch);

	// draw the title
	Draw_Text_GDI("ACCELERATION DEMO - Space to Launch, Arrows to Change Acceleration.", 10, 10, RGB(0, 255, 255), framebuffer, framebuffer_pitch);

	// draw information

	// first acceleration mode
	if (rocket.varsI[0] == 1)
		sprintf(buffer, "Acceleration is ON");
	else
		sprintf(buffer, "Acceleration is OFF");

	Draw_Text_GDI(buffer, 10, 25, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	// now current velocity
	sprintf(buffer, "Velocity = %f pixels/frame", rocket.varsF[0]);
	Draw_Text_GDI(buffer, 10, 40, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	// and acceleration
	sprintf(buffer, "Acceleration = %f pixels/frame^2", rocket.varsF[1]);
	Draw_Text_GDI(buffer, 10, 55, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	// sync to 30 fps = 1/30sec = 33 ms
	Wait_Clock(33);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// seed random number generate
	srand(Start_Clock());

	// load background image
	Load_Bitmap_File(&bitmap8bit, "assets/gantry.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480);
	Load_Image_Bitmap(&background_bmp, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Set_Palette(bitmap8bit.palette);
	Unload_Bitmap_File(&bitmap8bit);

	// load the bitmaps
	Load_Bitmap_File(&bitmap8bit, "assets/rocket8.bmp");

	// create bob
	Create_BOB(&rocket, 312, 420, 24, 80, 3, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0);

	// set animation speed
	Set_Anim_Speed_BOB(&rocket, 2);

	// load the bob in 
	for (int index = 0; index < 3; index++)
		Load_Frame_BOB(&rocket, &bitmap8bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// set animation state to on pad
	rocket.anim_state = ROCKET_STATE_ON_PAD;

	// use varsI[0] to hold acceleration mode, set to on
	rocket.varsI[0] = 1; // -1 is off

	// use varsF[0,1] to hold the velocity and acceleration
	rocket.varsF[0] = 0; // initial velocity
	rocket.varsF[1] = 0.2; // initial acceleration 0.2 pixel/frame^2

	// unload bitmap image
	Unload_Bitmap_File(&bitmap8bit);

	// create lookup for lighting engine
	RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
		palette,             // source palette
		rgblookup);          // lookup table
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the bobs
	Destroy_BOB(&rocket);
}

//////////////////////////////////////////////////////////
