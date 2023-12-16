//
// DEMO9_1_16b.CPP - DirectInput Keyboard Demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

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

// RGB color of the walls
#define WALL_COLOR_R 239
#define WALL_COLOR_G 8 
#define WALL_COLOR_B 0

// GLOBALS ////////////////////////////////////////////////

// windows vars
char buffer[80];                // used to print text

// demo globals
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

BOB          plasma;       // players weapon

BITMAP_IMAGE reactor;      // the background   

// FUNCTIONS ///////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	int dx, dy;             // general deltas used in collision detection

	static int   player_moving = 0; // tracks player motion

	// start the timing clock
	Start_Clock();

	// draw the background reactor image
	Draw_Bitmap16(&reactor, framebuffer, framebuffer_pitch, 0);

	// get player input

	// reset motion flag
	player_moving = 0;

	// test direction of motion, this is a good example of testing the keyboard
	// although the code could be optimized this is more educational

	if (RETRO_KeyState(SDL_SCANCODE_RIGHT) && RETRO_KeyState(SDL_SCANCODE_UP)) {
		// move skelaton
		skelaton.x += 2;
		skelaton.y -= 2;
		dx = 2; dy = -2;

		// set motion flag
		player_moving = 1;

		// check animation needs to change
		if (skelaton.curr_animation != SKELATON_NEAST)
			Set_Animation_BOB(&skelaton, SKELATON_NEAST);

	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT) && RETRO_KeyState(SDL_SCANCODE_UP)) {
			// move skelaton
			skelaton.x -= 2;
			skelaton.y -= 2;
			dx = -2; dy = -2;

			// set motion flag
			player_moving = 1;

			// check animation needs to change
			if (skelaton.curr_animation != SKELATON_NWEST)
				Set_Animation_BOB(&skelaton, SKELATON_NWEST);

		} // end if
		else
			if (RETRO_KeyState(SDL_SCANCODE_LEFT) && RETRO_KeyState(SDL_SCANCODE_DOWN)) {
				// move skelaton
				skelaton.x -= 2;
				skelaton.y += 2;
				dx = -2; dy = 2;

				// set motion flag
				player_moving = 1;

				// check animation needs to change
				if (skelaton.curr_animation != SKELATON_SWEST)
					Set_Animation_BOB(&skelaton, SKELATON_SWEST);

			} // end if
			else
				if (RETRO_KeyState(SDL_SCANCODE_RIGHT) && RETRO_KeyState(SDL_SCANCODE_DOWN)) {
					// move skelaton
					skelaton.x += 2;
					skelaton.y += 2;
					dx = 2; dy = 2;

					// set motion flag
					player_moving = 1;

					// check animation needs to change
					if (skelaton.curr_animation != SKELATON_SEAST)
						Set_Animation_BOB(&skelaton, SKELATON_SEAST);

				} // end if
				else
					if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
						// move skelaton
						skelaton.x += 2;
						dx = 2; dy = 0;

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
							dx = -2; dy = 0;

							// set motion flag
							player_moving = 1;

							// check animation needs to change
							if (skelaton.curr_animation != SKELATON_WEST)
								Set_Animation_BOB(&skelaton, SKELATON_WEST);

						} // end if
						else
							if (RETRO_KeyState(SDL_SCANCODE_UP)) {
								// move skelaton
								skelaton.y -= 2;
								dx = 0; dy = -2;

								// set motion flag
								player_moving = 1;

								// check animation needs to change
								if (skelaton.curr_animation != SKELATON_NORTH)
									Set_Animation_BOB(&skelaton, SKELATON_NORTH);

							} // end if
							else
								if (RETRO_KeyState(SDL_SCANCODE_DOWN)) {
									// move skelaton
									skelaton.y += 2;
									dx = 0; dy = +2;

									// set motion flag
									player_moving = 1;

									// check animation needs to change
									if (skelaton.curr_animation != SKELATON_SOUTH)
										Set_Animation_BOB(&skelaton, SKELATON_SOUTH);

								} // end if

							 // only animate if player is moving
	if (player_moving) {
		// animate skelaton
		Animate_BOB(&skelaton);

		// see if skelaton hit a wall

		// call the color scanner with WALL_COLOR the color of the walls
		// try to center the scan on the feet of the player
		// note since we are uin 16-bit mode, we need to scan the 16 bit value then compare
		// it against the 16-bit color code for the green pixel which has values RB(41,231,41)
		// but depending if this is a 5.5.5 or 5.6.5 the 16-bit value will be different, however
		// during ddraw_init RGB16Bit() was vectored (function pointer) to either 5.5.5 or 5.6.5 
		// depending on the actual surface mode, so it should all work out :)
		if (Color_Scan16(skelaton.x + 16, skelaton.y + 16,
			skelaton.x + skelaton.width - 16, skelaton.y + skelaton.height - 16,
			RGB16Bit(WALL_COLOR_R, WALL_COLOR_G, WALL_COLOR_B),
			RGB16Bit(WALL_COLOR_R, WALL_COLOR_G, WALL_COLOR_B), framebuffer, framebuffer_pitch)) {
			// back the skelaton up along its last trajectory
			skelaton.x -= dx;
			skelaton.y -= dy;
		} // end if

		// check if skelaton is off screen
		if (skelaton.x < 0 || skelaton.x >(screen_width - skelaton.width))
			skelaton.x -= dx;

		if (skelaton.y < 0 || skelaton.y >(screen_height - skelaton.height))
			skelaton.y -= dy;

	} // end if

	// draw the skelaton
	Draw_BOB16(&skelaton, framebuffer, framebuffer_pitch);

	// draw some text
	Draw_Text_GDI16("I STILL HAVE A BONE TO PICK!", 0, screen_height - 32, RGB16Bit(32, 32, 32), framebuffer, framebuffer_pitch);
	Draw_Text_GDI16("(16-Bit Version) USE ARROW KEYS TO MOVE, <ESC> TO EXIT.", 0, 0, RGB16Bit(32, 32, 32), framebuffer, framebuffer_pitch);

	// sync to 30 fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();

	char filename[80]; // used to build up files names

	// load the background
	Load_Bitmap_File(&bitmap16bit, "assets/reactor_24.bmp");

	// create and load the reactor bitmap image
	Create_Bitmap(&reactor, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&reactor, &bitmap16bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// now let's load in all the frames for the skelaton!!!

	// create skelaton bob
	Create_BOB(&skelaton, 0, 0, 56, 72, 32,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM, 0, 0, 16);

	// load the frames in 8 directions, 4 frames each
	// each set of frames has a walk and a fire, frame sets
	// are loaded in counter clockwise order looking down
	// from a birds eys view or the x-z plane
	for (int direction = 0; direction < 8; direction++) {
		// build up file name
		sprintf(filename, "assets/skelsp%d_24.bmp", direction);

		// load in new bitmap file
		Load_Bitmap_File(&bitmap16bit, filename);

		Load_Frame_BOB16(&skelaton, &bitmap16bit, 0 + direction * 4, 0, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB16(&skelaton, &bitmap16bit, 1 + direction * 4, 1, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB16(&skelaton, &bitmap16bit, 2 + direction * 4, 2, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB16(&skelaton, &bitmap16bit, 3 + direction * 4, 0, 1, BITMAP_EXTRACT_MODE_CELL);

		// unload the bitmap file
		Unload_Bitmap_File(&bitmap16bit);

		// set the animation sequences for skelaton
		Load_Animation_BOB(&skelaton, direction, 4, skelaton_anims[direction]);

	} // end for direction

	// set up stating state of skelaton
	Set_Animation_BOB(&skelaton, 0);
	Set_Anim_Speed_BOB(&skelaton, 4);
	Set_Vel_BOB(&skelaton, 0, 0);
	Set_Pos_BOB(&skelaton, 0, 128);
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill the reactor
	Destroy_Bitmap(&reactor);

	// kill skelaton
	Destroy_BOB(&skelaton);
}

///////////////////////////////////////////////////////////
