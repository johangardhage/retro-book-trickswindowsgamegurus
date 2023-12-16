//
// DEMO12_5.CPP - pattern demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

#define NUM_PATTERNS    4     // number of patterns in system

// pattern instruction opcodes for bot

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
#define OPC_STOP 8  // stop for a moment
#define OPC_RAND 9  // select a random direction
#define OPC_END  -1 // end pattern

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background
BOB          bot;              // the demo robot ship

// patterns in opcode operand format
int pattern_1[] = { OPC_W, 10, OPC_NW, 10, OPC_N, 10, OPC_NE, 10,
				   OPC_E, 10, OPC_SE, 10, OPC_S, 10, OPC_SW, 10,
				   OPC_W, 10, OPC_RAND, 10,

				   OPC_W, 20, OPC_NW, 10, OPC_N, 20, OPC_NE, 10,
				   OPC_E, 20, OPC_SE, 10, OPC_S, 20, OPC_SW, 10,
				   OPC_W, 10, OPC_END,0 };

int pattern_2[] = { OPC_E, 20, OPC_W, 20, OPC_STOP, 20, OPC_NE, 10,
				   OPC_W, 10, OPC_NW, 10, OPC_SW, 20, OPC_NW, 20,
				   OPC_SW, 20, OPC_NW, 30, OPC_SW, 10, OPC_S, 50,
				   OPC_W, 2, OPC_NW, 2, OPC_N, 2, OPC_NE, 50,
				   OPC_E,2, OPC_SE,2, OPC_S,2, OPC_RAND, 10, OPC_END,0 };

int pattern_3[] = { OPC_N, 10, OPC_S, 10, OPC_N, 10, OPC_S, 10,
					OPC_E, 10, OPC_W, 10, OPC_E, 10, OPC_W, 10,
					OPC_NW, 10, OPC_N, 10, OPC_NE, 10, OPC_N, 10,
					OPC_STOP, 20, OPC_RAND, 5, OPC_E, 50, OPC_S, 50, OPC_W, 50,
					OPC_E, 10, OPC_E, 10, OPC_E, 10, OPC_NW, 100,
					OPC_STOP, 10, OPC_END,0 };

int pattern_4[] = { OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,

				   OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,

				   OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,

				   OPC_RAND, 10, OPC_RAND, 5,

				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100,

				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100,

				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100, OPC_END,0 };

// master pattern array
int *patterns[NUM_PATTERNS] = { pattern_1, pattern_2, pattern_3, pattern_4 };

int *curr_pattern = NULL;  // current pattern being processed

int bot_ip = 0,       // pattern instruction pointer for bot
bot_counter = 0,       // counter of pattern control
bot_pattern_index;   // the current pattern being executed

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
						"OPC_END", };

// FUNCTIONS //////////////////////////////////////////////

void Bot_AI(void)
{
	// this function controls the ai of the bot and the pattern
	// processing

	static int opcode,  // current pattern opcode
		operand; // current operand 

	// test if it's time to process a new instruction
	if (curr_pattern == NULL) {
		// select a random pattern in pattern bank
		bot_pattern_index = rand() % NUM_PATTERNS;
		curr_pattern = patterns[bot_pattern_index];

		// now reset instuction pointer
		bot_ip = 0;

		// reset counter
		bot_counter = 0;

	} // end if

	// process next instruction if it's time
	if (--bot_counter <= 0) {
		// get next instruction
		opcode = curr_pattern[bot_ip++];
		operand = curr_pattern[bot_ip++];

		// test what the opcode is
		switch (opcode) {
		case OPC_E:
		{
			// set direction to east
			Set_Vel_BOB(&bot, 6, 0);

			// set animation to east
			Set_Animation_BOB(&bot, opcode);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_NE:
		{
			// set direction to northeast
			Set_Vel_BOB(&bot, 6, -6);

			// set animation to northeast
			Set_Animation_BOB(&bot, opcode);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_N:
		{
			// set direction to north
			Set_Vel_BOB(&bot, 0, -6);

			// set animation to north
			Set_Animation_BOB(&bot, opcode);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_NW:
		{
			// set direction to northwest
			Set_Vel_BOB(&bot, -6, -6);

			// set animation to northwest
			Set_Animation_BOB(&bot, opcode);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_W:
		{
			// set direction to west
			Set_Vel_BOB(&bot, -6, 0);

			// set animation to west
			Set_Animation_BOB(&bot, opcode);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_SW:
		{
			// set direction to southwest
			Set_Vel_BOB(&bot, -6, 6);

			// set animation to southwest
			Set_Animation_BOB(&bot, opcode);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_S:
		{
			// set direction to south
			Set_Vel_BOB(&bot, 0, 6);

			// set animation to south
			Set_Animation_BOB(&bot, opcode);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_SE:
		{
			// set direction to southeast
			Set_Vel_BOB(&bot, 6, 6);

			// set animation to southeast
			Set_Animation_BOB(&bot, opcode);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_STOP:
		{
			// stop motion
			Set_Vel_BOB(&bot, 0, 0);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_RAND:
		{
			// set direction randomly
			Set_Vel_BOB(&bot, -4 + rand() % 9, -4 + rand() % 9);

			// set animation to south
			Set_Animation_BOB(&bot, OPC_S);

			// set counter to instuction operand
			bot_counter = operand;

		} break;

		case OPC_END:
		{
			// stop motion
			Set_Vel_BOB(&bot, 0, 0);

			// select a random pattern in pattern bank
			bot_pattern_index = rand() % NUM_PATTERNS;
			curr_pattern = patterns[bot_pattern_index];

			// now reset instuction pointer
			bot_ip = 0;

			// reset counter
			bot_counter = 0;

		} break;

		default: break;

		} // end switch

	} // end if

	// draw stats
	sprintf(buffer, "Pattern #%d", bot_pattern_index);
	Draw_Text_GDI(buffer, 10, 400, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Opcode=%s Operand=%d", opcode_names[opcode], operand);
	Draw_Text_GDI(buffer, 10, 416, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Instruction Ptr=%d ", bot_ip);
	Draw_Text_GDI(buffer, 10, 432, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Counter=%d ", bot_counter);
	Draw_Text_GDI(buffer, 10, 448, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

} // end Bot_AI

//////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// do the ai on bot
	Bot_AI();

	// the animate the bot
	Animate_BOB(&bot);

	// move bot
	Move_BOB(&bot);

	// test if bot is off screen, if so wrap around
	if (bot.x >= screen_width)
		bot.x = -bot.width;
	else
		if (bot.x < -bot.width)
			bot.x = screen_width;

	if (bot.y >= screen_height)
		bot.y = -bot.height;
	else
		if (bot.y < -bot.height)
			bot.y = screen_height;

	// draw the bot
	Draw_BOB(&bot, framebuffer, framebuffer_pitch);

	// draw title
	Draw_Text_GDI("I-ROBOT 3D - Pattern Demo", 10, 10, RGB(0, 255, 255), framebuffer, framebuffer_pitch);

	// sync to 30ish fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	int bot_anim[2];

	// load background image
	Load_Bitmap_File(&bitmap8bit, "assets/trekb8.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480);
	Load_Image_Bitmap(&background_bmp, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Set_Palette(bitmap8bit.palette);
	Unload_Bitmap_File(&bitmap8bit);

	// load the bots bitmaps
	Load_Bitmap_File(&bitmap8bit, "assets/bots8.bmp");

	// create bat bob
	Create_BOB(&bot, 320, 200, 116, 60, 16, BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, 0);
	Set_Anim_Speed_BOB(&bot, 2);

	// load the bot in 
	for (int index = 0; index < 16; index++)
		Load_Frame_BOB(&bot, &bitmap8bit, index, index % 2, index / 2, BITMAP_EXTRACT_MODE_CELL);

	// unload bot
	Unload_Bitmap_File(&bitmap8bit);

	// create animations for bot
	for (int index = 0; index < 8; index++) {
		// generate the animation on the fly
		bot_anim[0] = index * 2;
		bot_anim[1] = bot_anim[0] + 1;

		// load the generated animation
		Load_Animation_BOB(&bot, index, 2, bot_anim);
	} // end for index

	// set the animation to right direction
	Set_Animation_BOB(&bot, 0);

	// create lookup for lighting engine
	RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
		palette,             // source palette
		rgblookup);          // lookup table
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the bobs
	Destroy_BOB(&bot);
}

//////////////////////////////////////////////////////////
