//
// DEMO13_3_16b.CPP - two mass gravity simulation
// 16-bit version
// note: there is currently no upper limit on your velocity
// since this is more realistic, so be careful not to get 
// out of control
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

#define FRICTION_FACTOR  (float)(0.05)  // friction of the virtual space

// these are the gravity constants, they are selected to simply work
#define VIRTUAL_GRAVITY_CONSTANT     (float)0.01
#define SHIP_MASS                    (float)2
#define BLACK_HOLE_MASS              (float)50000
#define MAX_VEL                      (float)30

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background
BOB          ship;             // the ship
BOB          black_hole;       // the gravity well

// FUNCTIONS //////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap16(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// check the player controls

	// is the player turning right or left?
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		// there are 16 possible positions for the ship to point in
		if (++ship.varsI[0] >= 16)
			ship.varsI[0] = 0;
	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
			// there are 16 possible positions for the ship to point in
			if (--ship.varsI[0] < 0)
				ship.varsI[0] = 15;
		} // end if

	 // now test for forward thrust
	if (RETRO_KeyState(SDL_SCANCODE_UP)) {
		// thrust ship in current direction

		float rad_angle = (float)ship.varsI[0] * (float)3.14159 / (float)8;
		float xv = cos(rad_angle);
		float yv = sin(rad_angle);

		ship.varsF[0] += xv;
		ship.varsF[1] += yv;

		// animate the ship
		ship.curr_frame = ship.varsI[0] + 16 * (rand() % 2);

	} // end if
	else // show non thrust version
		ship.curr_frame = ship.varsI[0];

	// move ship
	ship.varsF[2] += ship.varsF[0];
	ship.varsF[3] += ship.varsF[1];

	// always apply friction in direction opposite current trajectory
	float fx = -ship.varsF[0];
	float fy = -ship.varsF[1];
	float length_f = sqrt(fx * fx + fy * fy); // normally we would avoid square root at all costs!

	// compute the frictional resitance

	if (fabs(length_f) > 0.1) {
		fx = FRICTION_FACTOR * fx / length_f;
		fy = FRICTION_FACTOR * fy / length_f;
	} // end if
	else
		fx = fy = 0;

	// now apply friction to forward velocity
	ship.varsF[0] += fx;
	ship.varsF[1] += fy;

	////////////////////////////////////////////////////////////////////

	// gravity calculation section

	// step 1: compute vector from black hole to ship, note that the centers
	// of each object are used
	float grav_x = (black_hole.x + black_hole.width / 2) - (ship.x + ship.width / 2);
	float grav_y = (black_hole.y + black_hole.height / 2) - (ship.y + ship.height / 2);
	float radius_squared = grav_x * grav_x + grav_y * grav_y; // equal to radius squared
	float length_grav = sqrt(radius_squared);

	// step 2: normalize the length of the vector to 1.0
	grav_x = grav_x / length_grav;
	grav_y = grav_y / length_grav;

	// step 3: compute the gravity force
	float grav_force = (VIRTUAL_GRAVITY_CONSTANT) * (SHIP_MASS * BLACK_HOLE_MASS) / radius_squared;

	// step 4: apply gforce in the direction of grav_x, grav_y with the magnitude of grav_force
	ship.varsF[0] += grav_x * grav_force;
	ship.varsF[1] += grav_y * grav_force;

	////////////////////////////////////////////////////////////////////

	// test if ship is off screen
	if (ship.varsF[2] > screen_width)
		ship.varsF[2] = -ship.width;
	else
		if (ship.varsF[2] < -ship.width)
			ship.varsF[2] = screen_width;

	if (ship.varsF[3] > screen_height)
		ship.varsF[3] = -ship.height;
	else
		if (ship.varsF[3] < -ship.height)
			ship.varsF[3] = screen_height;

	// test if velocity is insane
	if ((ship.varsF[0] * ship.varsF[0] + ship.varsF[1] * ship.varsF[1]) > MAX_VEL) {
		// scale velocity down
		ship.varsF[0] *= .95;
		ship.varsF[1] *= .95;

	} // end if


 // animate the black hole
	Animate_BOB(&black_hole);

	// draw the black hole
	Draw_BOB16(&black_hole, framebuffer, framebuffer_pitch);

	// copy floating point position to bob x,y
	ship.x = ship.varsF[2];
	ship.y = ship.varsF[3];

	// draw the ship
	Draw_BOB16(&ship, framebuffer, framebuffer_pitch);

	// draw the title
	Draw_Text_GDI16("(16-Bit Version) GRAVITY MASS DEMO - Use Arrows to Control Ship.", 10, 10, RGB16Bit(0, 255, 255), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Friction: X=%f, Y=%f", fx, fy);
	Draw_Text_GDI16(buffer, 10, 420, RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Velocity: X=%f, Y=%f", ship.varsF[0], ship.varsF[1]);
	Draw_Text_GDI16(buffer, 10, 440, RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Gravity: X=%f, Y=%f", ship.varsF[2], ship.varsF[3]);
	Draw_Text_GDI16(buffer, 10, 460, RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);

	// sync to 30 fps = 1/30sec = 33 ms
	Wait_Clock(33);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();

	// seed random number generate
	srand(Start_Clock());

	// load background image
	Load_Bitmap_File(&bitmap16bit, "assets/gravsky24.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&background_bmp, &bitmap16bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// load the bitmaps for ship
	Load_Bitmap_File(&bitmap16bit, "assets/blaze24.bmp");

	// create bob
	Create_BOB(&ship, 320, 240, 22, 18, 32, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0, 0, 16);

	// well use varsI[0] to hold the direction 0..15, 0-360 degrees, clockwise
	ship.varsI[0] = 0; // along +x axis to start

	// use varsF[0,1] for the x and y velocity
	ship.varsF[0] = 0;
	ship.varsF[1] = 0;

	// use varsF[2,3] for the x and y position, we need more accuracy than ints
	ship.varsF[2] = ship.x;
	ship.varsF[3] = ship.y;

	// load the frames in
	for (int index = 0; index < 32; index++)
		Load_Frame_BOB16(&ship, &bitmap16bit, index, index % 16, index / 16, BITMAP_EXTRACT_MODE_CELL);

	// unload bitmap image
	Unload_Bitmap_File(&bitmap16bit);

	// load the bitmaps for blackhole
	Load_Bitmap_File(&bitmap16bit, "assets/photon24.bmp");

	// create bob
	Create_BOB(&black_hole, 32 + rand() % (screen_width - 64), 32 + rand() % (screen_height - 64),
		44, 44, 7, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0, 0, 16);

	// set animation speed
	Set_Anim_Speed_BOB(&black_hole, 3);

	// load the frames in
	for (int index = 0; index < 7; index++)
		Load_Frame_BOB16(&black_hole, &bitmap16bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// unload bitmap image
	Unload_Bitmap_File(&bitmap16bit);
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the bobs
	Destroy_BOB(&ship);
	Destroy_BOB(&black_hole);
}

//////////////////////////////////////////////////////////
