//
// DEMO13_11_16b.CPP - Time based motion modeling
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

// variable indices
#define SHIP_X_POS  0
#define SHIP_Y_POS  1
#define SHIP_X_VEL  2

#define SHIP_INITIAL_VELOCITY   0.050    // 50 pixels/sec

// MACROS ///////////////////////////////////////////////

#define RAND_RANGE(x,y) ( (x) + (rand()%((y)-(x)+1)))
#define DOT_PRODUCT(ux,uy,vx,vy) ((ux)*(vx) + (uy)*(vy))

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                 // used to print text

BITMAP_IMAGE background_bmp;      // holds the background
BOB          ship;                // the ship

// FUNCTIONS //////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	static int delay = 30; // initial delay per frame

	// start the timing clock
	DWORD start_time = Start_Clock();

	// draw background
	Draw_Bitmap16(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// update delay
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT))
		delay += 5;
	else
		if (delay > 5 && RETRO_KeyState(SDL_SCANCODE_LEFT))
			delay -= 5;

	// draw the ship
	ship.x = ship.varsF[SHIP_X_POS] + 0.5;
	ship.y = ship.varsF[SHIP_Y_POS] + 0.5;
	ship.curr_frame = 0;
	Draw_BOB16(&ship, framebuffer, framebuffer_pitch);

	// draw shadow
	ship.curr_frame = 1;
	ship.x -= 64;
	ship.y += 128;
	Draw_BOB16(&ship, framebuffer, framebuffer_pitch);

	// draw the title
	Draw_Text_GDI16("(16-Bit Version) Time Based Kinematic Motion DEMO, Press <ESC> to Exit.", 10, 10, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Frame rate = %f, use <RIGHT>, <LEFT> arrows to change load", 1000 * 1 / (float)delay);
	Draw_Text_GDI16(buffer, 10, 25, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Ship Velocity is %f pixels/sec", 1000 * ship.varsF[SHIP_X_VEL]);
	Draw_Text_GDI16(buffer, 10, 40, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	// this models the load in the game
	Wait_Clock(delay);

	// move the ship based on time //////////////////////////////////
	// x = x + v*dt

	// compute dt
	float dt = Get_Clock() - start_time; // in milliseconds

	// move based on 30 pixels per seconds or .03 pixels per millisecond
	ship.varsF[SHIP_X_POS] += (ship.varsF[SHIP_X_VEL] * dt);

	// test for off screen
	if (ship.varsF[SHIP_X_POS] > screen_width + ship.width)
		ship.varsF[SHIP_X_POS] = -ship.width;
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();

	// seed random number generate
	srand(Start_Clock());

	// load background image
	Load_Bitmap_File(&bitmap16bit, "assets/lava24.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&background_bmp, &bitmap16bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// load the bitmaps
	Load_Bitmap_File(&bitmap16bit, "assets/lavaship24.bmp");

	// create ship
	Create_BOB(&ship, 0, 0, 80, 32, 2, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0, 0, 16);

	// load the imagery in
	for (int index = 0; index < 2; index++)
		Load_Frame_BOB16(&ship, &bitmap16bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// unload bitmap image
	Unload_Bitmap_File(&bitmap16bit);

	// position the ship to left of screen

	// use element 0,1 to hold float accurate position
	ship.varsF[SHIP_X_POS] = 0;
	ship.varsF[SHIP_Y_POS] = screen_height / 2;

	// use index 2 to hold x velocity
	ship.varsF[SHIP_X_VEL] = SHIP_INITIAL_VELOCITY;
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the bobs
	Destroy_BOB(&ship);
}

//////////////////////////////////////////////////////////
