//
// DEMO13_4_16b.CPP - 2D trajectory demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

// defines for polygon cannon
#define CANNON_X0  39       // position of tip of cannon
#define CANNON_Y0  372
#define NUM_PROJECTILES 16  // number of projectiles

// MACROS ///////////////////////////////////////////////

#define RAND_RANGE(x,y) ( (x) + (rand()%((y)-(x)+1)))

// TYPES ///////////////////////////////////////////////

typedef struct PROJ_TYP
{
	int state;      // state 0 off, 1 on
	float x, y;      // position
	float xv, yv;   // velocity

} PROJECTILE, *PROJECTILE_PTR;

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                 // used to print text

BITMAP_IMAGE background_bmp;      // holds the background

int cannon_ids[8]; // sound ids

POLYGON2D cannon; // the ship

PROJECTILE missiles[NUM_PROJECTILES]; // array of missiles

float gravity_force = 0.2;  // gravity
float wind_force = -0.01; // wind resistance

// FUNCTIONS //////////////////////////////////////////////

void Init_Projectiles(void)
{
	// this function initializes the projectiles
	memset(missiles, 0, sizeof(PROJECTILE) * NUM_PROJECTILES);

} // end Init_Projectiles

/////////////////////////////////////////////////////////////

void Move_Projectiles(void)
{
	// this function moves all the projectiles and does the physics model
	for (int index = 0; index < NUM_PROJECTILES; index++) {
		if (missiles[index].state == 1) {
			// translate
			missiles[index].x += missiles[index].xv;
			missiles[index].y += missiles[index].yv;

			// apply forces
			missiles[index].xv += wind_force;
			missiles[index].yv += gravity_force;

			// test for off screen
			if (missiles[index].x >= screen_width ||
				missiles[index].y >= screen_height ||
				missiles[index].y < 0) {
				// kill the missile
				missiles[index].state = 0;
			} // end if

		} // end if on

	} // end for index

} // end Move_Projectiles

/////////////////////////////////////////////////////////////

void Draw_Projectiles(void)
{
	// this function draws all the projectiles 
	for (int index = 0; index < NUM_PROJECTILES; index++) {
		// is this one alive?
		if (missiles[index].state == 1) {
			Draw_Rectangle(missiles[index].x - 1, missiles[index].y - 1,
				missiles[index].x + 1, missiles[index].y + 1,
				RGB16Bit(0, 255, 0), framebuffer, framebuffer_pitch);
		} // end if

	} // end for index

} // end Draw_Projectiles

/////////////////////////////////////////////////////////////

void Fire_Projectile(int angle, float vel)
{
	// this function starts a projectile with the given angle and velocity 
	// at the tip of the cannon
	for (int index = 0; index < NUM_PROJECTILES; index++) {
		// find an open projectile
		if (missiles[index].state == 0) {
			// set this missile in motion at the head of cannon with the proper angle
			missiles[index].x = cannon.vlist[1].x + cannon.x0;
			missiles[index].y = cannon.vlist[1].y + cannon.y0;

			// compute velocity vector based on angle
			missiles[index].xv = vel * cos_look[angle];
			missiles[index].yv = -vel * sin_look[angle];

			// mark as active
			missiles[index].state = 1;

			// bail
			break;

		} // end if

	} // end for index

} // end Fire_Projectile

////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	static int   curr_angle = 0; // current angle of elevation from horizon
	static float curr_vel = 10; // current velocity of projectile

	// start the timing clock
	Start_Clock();

	// clear the drawing surface
	//DDraw_Fill_Surface(framebuffer, 0);

	// draw background
	Draw_Bitmap16(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// do the graphics
	Draw_Polygon2D16(&cannon, framebuffer, framebuffer_pitch);

	// test for rotate
	if ((curr_angle < 90) && RETRO_KeyState(SDL_SCANCODE_UP)) // rotate left
	{
		Rotate_Polygon2D_Mat(&cannon, -5);
		curr_angle += 5;
	} // end if
	else
		if ((curr_angle > 0) && RETRO_KeyState(SDL_SCANCODE_DOWN)) // rotate right
		{
			Rotate_Polygon2D_Mat(&cannon, 5);
			curr_angle -= 5;
		} // end if

	 // test for projectile velocity
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		if (curr_vel < 30) curr_vel += 0.1;
	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
			if (curr_vel > 0) curr_vel -= 0.1;
		} // end if

	 // test for wind force
	if (RETRO_KeyPressed(SDL_SCANCODE_W)) {
		if (wind_force < 2) wind_force += 0.01;
	} // end if
	else
		if (RETRO_KeyPressed(SDL_SCANCODE_E)) {
			if (wind_force > -2) wind_force -= 0.01;
		} // end if

	 // test for gravity force
	if (RETRO_KeyPressed(SDL_SCANCODE_G)) {
		if (gravity_force < 15) gravity_force += 0.1;
	} // end if
	else
		if (RETRO_KeyPressed(SDL_SCANCODE_B)) {
			if (gravity_force > -15) gravity_force -= 0.1;
		} // end if

	 // test for fire!
	if (RETRO_KeyPressed(SDL_SCANCODE_RETURN)) {
		Fire_Projectile(curr_angle, curr_vel);

	} // end fire

	// move all the projectiles
	Move_Projectiles();

	// draw the projectiles
	Draw_Projectiles();

	// draw the title
	Draw_Text_GDI16("(16-Bit Version) Trajectory DEMO, Press <ESC> to Exit.", 10, 10, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);
	Draw_Text_GDI16("<RIGHT>, <LEFT> to adjust velocity, <UP>, <DOWN> to adjust angle", 10, 25, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);
	Draw_Text_GDI16("<G>, <B> to adjust gravity, <W>, <E> to adjust wind, <CTRL> to fire.", 10, 40, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Ang=%d, Vel=%f", curr_angle, curr_vel);
	Draw_Text_GDI16(buffer, 10, 60, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Wind force=%f, Gravity Force=%f", wind_force, gravity_force);
	Draw_Text_GDI16(buffer, 10, 75, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

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
	Load_Bitmap_File(&bitmap16bit, "assets/tankgrid24.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&background_bmp, &bitmap16bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// define points of cannon
	VERTEX2DF cannon_vertices[4] = { 0,-2, 30,0, 30,2, 0,2, };

	// initialize ship
	cannon.state = 1;   // turn it on
	cannon.num_verts = 4;
	cannon.x0 = CANNON_X0; // position it
	cannon.y0 = CANNON_Y0;
	cannon.xv = 0;
	cannon.yv = 0;
	cannon.color = RGB16Bit(0, 255, 0); // green
	cannon.vlist = new VERTEX2DF[cannon.num_verts];

	for (int index = 0; index < cannon.num_verts; index++)
		cannon.vlist[index] = cannon_vertices[index];

	// build the 360 degree look ups
	Build_Sin_Cos_Tables();

	// initialize the missiles
	Init_Projectiles();
}

///////////////////////////////////////////////////////////
