//
// DEMO12_6_16b.CPP - pattern demo with tracking instruction
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

#define NUM_PATTERNS    4     // number of patterns in system

// pattern instruction opcodes for skelaton

// directional instructions
#define OPC_E    0  // move west
#define OPC_NE   1  // move northeast
#define OPC_N    2  // move north
#define OPC_NW   3  // move northwest
#define OPC_W    4  // move west
#define OPC_SW   5  // move southwest
#define OPC_S    6  // move south
#define OPC_SE   7  // move southeast

// special instructions
#define OPC_STOP        8  // stop for a moment
#define OPC_RAND        9  // select a random direction
#define OPC_TEST_DIST   10 // test distance 
#define OPC_END        -1  // end pattern

#define MIN_LONELYNESS  100 // minimum distance before skelaton
							// gets lonely for its pet bat

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background
BOB          skelaton;         // the AI skelaton

BOB          bat;                // this time your the bat
int          bat_sound_id = -1, // sound of bat flapping wings
laugh_sound_id = -1; // sound of skelaton laughing

// animation sequences for bob
int skelaton_anims[8][4] = { {0,1,0,2},
							 {0 + 4,1 + 4,0 + 4,2 + 4},
							 {0 + 8,1 + 8,0 + 8,2 + 8},
							 {0 + 12,1 + 12,0 + 12,2 + 12},
							 {0 + 16,1 + 16,0 + 16,2 + 16},
							 {0 + 20,1 + 20,0 + 20,2 + 20},
							 {0 + 24,1 + 24,0 + 24,2 + 24},
							 {0 + 28,1 + 28,0 + 28,2 + 28}, };

// patterns in opcode operand format
int pattern_1[] = { OPC_W, 10, OPC_NW, 10, OPC_N, 10, OPC_NE, 10,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_E, 10, OPC_SE, 10, OPC_S, 10, OPC_SW, 10,
				   OPC_W, 10, OPC_RAND, 10,
				   OPC_TEST_DIST, 50, // a distance test                   
				   OPC_W, 20, OPC_NW, 10, OPC_N, 20, OPC_NE, 10,
				   OPC_E, 20, OPC_SE, 10, OPC_S, 20, OPC_SW, 10,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_W, 10, OPC_END,0 };

int pattern_2[] = { OPC_E, 20, OPC_W, 20, OPC_STOP, 20, OPC_NE, 10,
				   OPC_TEST_DIST, 50, // a distance test                   
				   OPC_W, 10, OPC_NW, 10, OPC_SW, 20, OPC_NW, 20,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SW, 20, OPC_NW, 30, OPC_SW, 10, OPC_S, 50,
				   OPC_TEST_DIST, 50, // a distance test                   
				   OPC_W, 2, OPC_NW, 2, OPC_N, 2, OPC_NE, 50,  OPC_TEST_DIST, 50, // a distance test
				   OPC_E,2, OPC_SE,2, OPC_S,2, OPC_RAND, 10, OPC_END,0 };

int pattern_3[] = { OPC_N, 10, OPC_S, 10, OPC_N, 10, OPC_S, 10,
					OPC_TEST_DIST, 50, // a distance test                    
					OPC_E, 10, OPC_W, 10, OPC_E, 10, OPC_W, 10,
					OPC_TEST_DIST, 50, // a distance test
					OPC_NW, 10, OPC_N, 10, OPC_NE, 10, OPC_N, 10,
					OPC_TEST_DIST, 60, // a distance test
					OPC_STOP, 20, OPC_RAND, 5, OPC_E, 50, OPC_S, 50, OPC_W, 50,
					OPC_TEST_DIST, 50, // a distance test                    
					OPC_E, 10, OPC_E, 10, OPC_E, 10, OPC_NW, 100,   OPC_TEST_DIST, 60, // a distance test
					OPC_STOP, 10, OPC_END,0 };

int pattern_4[] = { OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,
				   OPC_TEST_DIST, 50, // a distance test                    
				   OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,
				   OPC_TEST_DIST, 50, // a distance test                    
				   OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_RAND, 10, OPC_RAND, 5,

				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100, OPC_END,0 };

// master pattern array
int *patterns[NUM_PATTERNS] = { pattern_1, pattern_2, pattern_3, pattern_4 };

int *curr_pattern = NULL;  // current pattern being processed

int opcode,                   // general opcode
operand,                  // general operand
skelaton_ip = 0,       // pattern instruction pointer for skelaton
skelaton_counter = 0,       // counter of pattern control
skelaton_pattern_index;   // the current pattern being executed

// used as a index to string lookup to help print out
const char *opcode_names[] = { "OPC_E",
						"OPC_NE",
						"OPC_N",
						"OPC_NW",
						"OPC_W",
						"OPC_SW",
						"OPC_S",
						"OPC_SE",
						"OPC_STOP",
						"OPC_RAND",
						"OPC_TEST_DIST" };

// FUNCTIONS //////////////////////////////////////////////

void Skelaton_AI(void)
{
	// this function controls the ai of the skelaton and the pattern
	// processing


	// these are local defines to help compute the direction to move the
	// skelaton if it needs to hunt down bat, note they are all in power
	// of 2 order, hence, they are mutually exclusive

#define WEST_BIT     1
#define EAST_BIT     2
#define NORTH_BIT    4 
#define SOUTH_BIT    8 

	// test if it's time to process a new instruction
	if (curr_pattern == NULL) {
		// select a random pattern in pattern bank
		skelaton_pattern_index = rand() % NUM_PATTERNS;
		curr_pattern = patterns[skelaton_pattern_index];

		// now reset instuction pointer
		skelaton_ip = 0;

		// reset counter
		skelaton_counter = 0;

	} // end if

	// process next instruction if it's time
	if (--skelaton_counter <= 0) {
		// get next instruction
		opcode = curr_pattern[skelaton_ip++];
		operand = curr_pattern[skelaton_ip++];

		// test what the opcode is
		switch (opcode) {
		case OPC_E:
		{
			// set direction to east
			Set_Vel_BOB(&skelaton, 3, 0);

			// set animation to east
			Set_Animation_BOB(&skelaton, opcode);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_NE:
		{
			// set direction to northeast
			Set_Vel_BOB(&skelaton, 3, -3);

			// set animation to northeast
			Set_Animation_BOB(&skelaton, opcode);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_N:
		{
			// set direction to north
			Set_Vel_BOB(&skelaton, 0, -3);

			// set animation to north
			Set_Animation_BOB(&skelaton, opcode);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_NW:
		{
			// set direction to northwest
			Set_Vel_BOB(&skelaton, -3, -3);

			// set animation to northwest
			Set_Animation_BOB(&skelaton, opcode);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_W:
		{
			// set direction to west
			Set_Vel_BOB(&skelaton, -3, 0);

			// set animation to west
			Set_Animation_BOB(&skelaton, opcode);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_SW:
		{
			// set direction to southwest
			Set_Vel_BOB(&skelaton, -3, 3);

			// set animation to southwest
			Set_Animation_BOB(&skelaton, opcode);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_S:
		{
			// set direction to south
			Set_Vel_BOB(&skelaton, 0, 3);

			// set animation to south
			Set_Animation_BOB(&skelaton, opcode);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_SE:
		{
			// set direction to southeast
			Set_Vel_BOB(&skelaton, 3, 3);

			// set animation to southeast
			Set_Animation_BOB(&skelaton, opcode);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_STOP:
		{
			// stop motion
			Set_Vel_BOB(&skelaton, 0, 0);

			// set counter to instuction operand
			skelaton_counter = operand;

		} break;

		case OPC_RAND:
		{
			// set counter to instuction operand
			skelaton_counter = 0;

		} break;

		case OPC_TEST_DIST:
		{
			// test distance between bat and skelaton
			// if bat is too far, then move toward bat

			int dx = (bat.x - skelaton.x);
			int dy = (bat.y - skelaton.y);

			// test distance against minimum lonelyness
			if (sqrt((dx * dx) + (dy * dy)) > MIN_LONELYNESS) {
				// the skelaton needs to be pointed toward the bat (player)
				// this is a bit hard because we need to point the skelaton
				// in 1 of 8 directions, instead of just giving him a velocity
				// to solve the problem well break it up into a dx and a dy and then
				// use a look up table to set everything up right

				int direction = 0; // the bit encoded direction

				// first east-west
				if (bat.x > skelaton.x)
					direction |= EAST_BIT;
				else
					if (bat.x < skelaton.x)
						direction |= WEST_BIT;

				// now north-south
				if (bat.y > skelaton.y)
					direction |= SOUTH_BIT;
				else
					if (bat.y < skelaton.y)
						direction |= NORTH_BIT;

				// test final direction, note this could be compressed into
				// another look up table, but this is simpler 
				switch (direction) {
				case WEST_BIT:
				{
					// set motion
					Set_Vel_BOB(&skelaton, -3, 0);

					// set animation 
					Set_Animation_BOB(&skelaton, OPC_W);

					// set counter to instuction operand
					skelaton_counter = operand;

				} break;

				case EAST_BIT:
				{
					// set motion
					Set_Vel_BOB(&skelaton, 3, 0);

					// set animation 
					Set_Animation_BOB(&skelaton, OPC_E);

					// set counter to instuction operand
					skelaton_counter = operand;

				} break;

				case NORTH_BIT:
				{
					// set motion
					Set_Vel_BOB(&skelaton, 0, -3);

					// set animation 
					Set_Animation_BOB(&skelaton, OPC_N);

					// set counter to instuction operand
					skelaton_counter = operand;

				} break;

				case SOUTH_BIT:
				{
					// set motion
					Set_Vel_BOB(&skelaton, 0, 3);

					// set animation 
					Set_Animation_BOB(&skelaton, OPC_S);

					// set counter to instuction operand
					skelaton_counter = operand;
				} break;

				case (NORTH_BIT | WEST_BIT):
				{
					// set motion
					Set_Vel_BOB(&skelaton, -3, -3);

					// set animation 
					Set_Animation_BOB(&skelaton, OPC_NW);

					// set counter to instuction operand
					skelaton_counter = operand;
				} break;

				case (NORTH_BIT | EAST_BIT):
				{
					// set motion
					Set_Vel_BOB(&skelaton, 3, -3);

					// set animation 
					Set_Animation_BOB(&skelaton, OPC_NE);

					// set counter to instuction operand
					skelaton_counter = operand;
				} break;

				case (SOUTH_BIT | WEST_BIT):
				{
					// set motion
					Set_Vel_BOB(&skelaton, -3, 3);

					// set animation 
					Set_Animation_BOB(&skelaton, OPC_SW);

					// set counter to instuction operand
					skelaton_counter = operand;
				} break;

				case (SOUTH_BIT | EAST_BIT):
				{
					// set motion
					Set_Vel_BOB(&skelaton, 3, 3);

					// set animation 
					Set_Animation_BOB(&skelaton, OPC_SE);

					// set counter to instuction operand
					skelaton_counter = operand;
				} break;

				default: break;

				} // end switch 

			} // end if

		} break;

		case OPC_END:
		{
			// stop motion
			Set_Vel_BOB(&skelaton, 0, 0);

			// select a random pattern in pattern bank
			skelaton_pattern_index = rand() % NUM_PATTERNS;
			curr_pattern = patterns[skelaton_pattern_index];

			// now reset instuction pointer
			skelaton_ip = 0;

			// reset counter
			skelaton_counter = 0;

		} break;

		default: break;

		} // end switch

	} // end if

	// draw stats
	sprintf(buffer, "Pattern #%d", skelaton_pattern_index);
	Draw_Text_GDI16(buffer, 10, 400, RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Opcode=%s Operand=%d", opcode_names[opcode], operand);
	Draw_Text_GDI16(buffer, 10, 416, RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Instruction Ptr=%d ", skelaton_ip);
	Draw_Text_GDI16(buffer, 10, 432, RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Counter=%d ", skelaton_counter);
	Draw_Text_GDI16(buffer, 10, 448, RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);

} // end skelaton_AI

//////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap16(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// do the ai on skelaton
	Skelaton_AI();

	// animate the bat
	Animate_BOB(&bat);

	// the animate the skelaton unless its stopped
	if (opcode != OPC_STOP)
		Animate_BOB(&skelaton);

	// move skelaton
	Move_BOB(&skelaton);

	// test if skelaton is off screen, if so wrap around
	if (skelaton.x >= screen_width)
		skelaton.x = -skelaton.width;
	else
		if (skelaton.x < -skelaton.width)
			skelaton.x = screen_width;

	if (skelaton.y >= screen_height)
		skelaton.y = -skelaton.height;
	else
		if (skelaton.y < -skelaton.height)
			skelaton.y = screen_height;

	// let player move the bat
	// allow player to move
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT))
		bat.x += 4;
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT))
			bat.x -= 4;

	if (RETRO_KeyState(SDL_SCANCODE_UP))
		bat.y -= 4;
	else
		if (RETRO_KeyState(SDL_SCANCODE_DOWN))
			bat.y += 4;

	// test if bat is off screen, if so wrap around
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

	// draw the skelaton
	Draw_BOB16(&skelaton, framebuffer, framebuffer_pitch);

	// draw the bat
	Draw_BOB16(&bat, framebuffer, framebuffer_pitch);

	// draw title
	Draw_Text_GDI16("(16-Bit Version) Skelaton Pattern Demo With Tracking", 10, 10, RGB16Bit(0, 255, 255), framebuffer, framebuffer_pitch);

	// sync to 30ish fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();

	char filename[80]; // used to build up filenames

	// load background image
	Load_Bitmap_File(&bitmap16bit, "assets/dungeon24.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&background_bmp, &bitmap16bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// load the bat bitmaps
	Load_Bitmap_File(&bitmap16bit, "assets/bats2_24.bmp");

	// create bat bob
	Create_BOB(&bat, 320, 200, 16, 16, 5, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0, 0, 16);
	Set_Anim_Speed_BOB(&bat, 2);

	// load the bat in 
	for (int index = 0; index < 5; index++)
		Load_Frame_BOB16(&bat, &bitmap16bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// unload bat
	Unload_Bitmap_File(&bitmap16bit);

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
	// kill all the bobs
	Destroy_BOB(&skelaton);
	Destroy_BOB(&bat);
}

//////////////////////////////////////////////////////////
