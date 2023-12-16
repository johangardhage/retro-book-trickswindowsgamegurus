//
// DEMO8_12.CPP - Sparse universe scrolling demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

// size of universe, could be anything
#define MAX_UNIVERSE_X 6400
#define MAX_UNIVERSE_Y 4800

#define NUM_OBJECTS_UNIVERSE 256 // number of objects in the universe

// used to flag type of object
#define GENERATOR_OBJ   0
#define BEACON_OBJ      1  
#define ALIEN_OBJ       2  

// TYPES ///////////////////////////////////////////////////

// need something to hold each object
typedef struct SPARSE_OBJ_TYP
{
	int type;  // type of object: beacon, generator, or alien
	// could have just used a bob_ptr here, but I like
	// doing this the hard way so you can exactly what's happening

	int x, y;   // position of object 

} SPARSE_OBJ, *SPARSE_OBJ_PTR;

// GLOBALS ////////////////////////////////////////////////

// windows vars
char buffer[80];                // used to print text

// demo globals
BOB generator,  // the generator BOB
alien,      // a little alien ship BOB
beacon;     // a beacon BOB

int world_x = 0,   // current position of viewing window
world_y = 0;

SPARSE_OBJ world[NUM_OBJECTS_UNIVERSE]; // our world consists of 256 objects randomly placed in the universe that is 10x10

// FUNCTIONS ///////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	int width, height, type;      // used to extract bob info
	int x0, y0, x1, y1; // used to compute the upper left and lower right corners of each object
	int visible = 0; // number of visible objects

	BOB_PTR object_ptr; // the object that is going to be rendered

	// start the timing clock
	Start_Clock();

	// check for movement (scrolling)
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		if ((world_x += 8) >= 5760)
			world_x = 5760 - 1;

	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
			if ((world_x -= 8) < 0)
				world_x = 0;

		} // end if

	if (RETRO_KeyState(SDL_SCANCODE_UP)) {
		if ((world_y -= 8) < 0)
			world_y = 0;

	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_DOWN)) {
			if ((world_y += 8) >= 4320)
				world_y = 4320 - 1;

		} // end if

	// based on current world position which defines the viewport, i.e,
	// world_x, world_y to world_x+640-1, world_y+480-1, scan the object list
	// determine any object in that region translate it to screen coords and render it

	for (int index = 0, visible = 0; index < NUM_OBJECTS_UNIVERSE; index++) {
		// get the object position
		x0 = world[index].x;
		y0 = world[index].y;

		// now translate to screen coords, could have done above, but clearer
		x0 = x0 - world_x;
		y0 = y0 - world_y;

		// get type, so we can compute size
		type = world[index].type;

		// compute size
		switch (type) {
		case GENERATOR_OBJ:
		{
			// get size of this object
			width = generator.width;
			height = generator.height;

			// point to it to render it easier without another switch
			object_ptr = &generator;
		} break;

		case BEACON_OBJ:
		{
			// get size of this object
			width = beacon.width;
			height = beacon.height;

			// point to it to render it easier without another switch
			object_ptr = &beacon;
		} break;

		case ALIEN_OBJ:
		{
			// get size of this object
			width = alien.width;
			height = alien.height;

			// point to it to render it easier without another switch
			object_ptr = &alien;

		} break;

		default: break;

		} // end switch

		// compute extents based on width,height
		x1 = x0 + width - 1;
		y1 = y0 + height - 1;

		// do clip trivial clip
		if ((x0 >= screen_width) || (x1 < 0) || (y0 >= screen_height) || (y1 < 0))
			continue; // don't need to render this one
		else {
			// render this object
			object_ptr->x = x0;
			object_ptr->y = y0;

			Draw_BOB(object_ptr, framebuffer, framebuffer_pitch);

			visible++;

		} // end else

	} // end for index

	// animate all the bobs
	Animate_BOB(&generator);
	Animate_BOB(&beacon);
	Animate_BOB(&alien);

	// draw some info
	Draw_Text_GDI("USE ARROW KEYS TO MOVE, <ESC> to Exit.", 8, 8, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "World Position = [%d, %d]  Objects Visible = %d, Objects Clipped = %d   ", world_x, world_y, visible, 256 - visible);
	Draw_Text_GDI(buffer, 8, screen_height - 32, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	// sync to 30 fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// load in the generator bob image
	Load_Bitmap_File(&bitmap8bit, "assets/gens4.bmp");

	// set the palette to the palette of the generators
	Set_Palette(bitmap8bit.palette);

	// create the generator bob
	Create_BOB(&generator, 0, 0, 72, 84, 4,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME, 0);

	// load each bitmap into the generator BOB object
	for (int index = 0; index < 4; index++)
		Load_Frame_BOB(&generator, &bitmap8bit, index, index % 4, index / 4, BITMAP_EXTRACT_MODE_CELL);

	// set up stating state of bob
	Set_Animation_BOB(&generator, 0);
	Set_Anim_Speed_BOB(&generator, 4);

	// unload the map bitmap
	Unload_Bitmap_File(&bitmap8bit);

	// load in the alien bob image
	Load_Bitmap_File(&bitmap8bit, "assets/aliens0.bmp");

	// create the alien bob
	Create_BOB(&alien, 0, 0, 56, 30, 8,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME, 0);

	// load each bitmap into the alien BOB object
	for (int index = 0; index < 8; index++)
		Load_Frame_BOB(&alien, &bitmap8bit, index, index % 5, index / 5, BITMAP_EXTRACT_MODE_CELL);

	// set up stating state of bob
	Set_Animation_BOB(&alien, 0);
	Set_Anim_Speed_BOB(&alien, 4);

	// unload the bitmap
	Unload_Bitmap_File(&bitmap8bit);

	// load in the beacon bob image
	Load_Bitmap_File(&bitmap8bit, "assets/pipe0.bmp");

	// create the alien bob
	Create_BOB(&beacon, 0, 0, 42, 36, 8,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME, 0);

	// load each bitmap into the beacon BOB object
	for (int index = 0; index < 8; index++)
		Load_Frame_BOB(&beacon, &bitmap8bit, index, index % 6, index / 6, BITMAP_EXTRACT_MODE_CELL);

	// set up stating state of bob
	Set_Animation_BOB(&beacon, 0);
	Set_Anim_Speed_BOB(&beacon, 4);

	// unload the bitmap
	Unload_Bitmap_File(&bitmap8bit);

	// now generate the sparse universe
	for (int index = 0; index < NUM_OBJECTS_UNIVERSE; index++) {
		// set random position
		world[index].x = rand() % MAX_UNIVERSE_X;
		world[index].y = rand() % MAX_UNIVERSE_Y;

		// set type
		world[index].type = rand() % 3; // 0, 1, 2 generator, alien, beacon

	} // end for index
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill objects
	Destroy_BOB(&alien);
	Destroy_BOB(&generator);
	Destroy_BOB(&beacon);
}

///////////////////////////////////////////////////////////
