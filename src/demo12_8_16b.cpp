//
// DEMO12_8_16b.CPP - path finding racing demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

#define NUM_WAYPOINTS 30 // number of waypoints in path

// TYPES /////////////////////////////////////////////////

typedef struct WAYPOINT_TYP
{
	float x, y;

} WAYPOINT, *WAYPOINT_PTR;

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                          // used to print text

BITMAP_IMAGE background_bmp;   // holds the background

BOB          car; // this little race car

int wind_sound_id = -1;        // the ambient wind
int car_sound_id = -1;         // the engine of the car

int vector_display_on = 1;     // used to toggle the informational vector rendering

// the way point list hand compiled :)
WAYPOINT path[NUM_WAYPOINTS] = {
{332,122},
{229,108},
{155,97},
{104,100},
{67,119},
{46,159},
{55,229},
{74,283},
{132,364},
{206,407},
{268,412},
{291,405},
{303,379},
{312,274},
{336,244},
{383,233},
{417,240},
{434,278},
{426,328},
{407,388},
{418,415},
{452,429},
{501,419},
{534,376},
{562,263},
{562,188},
{556,112},
{530,100},
{484,97},
{404,116}, };

// FUNCTIONS //////////////////////////////////////////////

int Find_Nearest_Waypoint(float x, float y)
{
	// this function finds the nearest waypoint to the sent position

	int near_id = 0;
	int near_dist = 1000,
		test_dist;

	for (int index = 0; index < NUM_WAYPOINTS; index++) {
		// is this waypoint closer?
		if ((test_dist = Fast_Distance_2D(path[index].x - x, path[index].y - y)) < near_dist) {
			// set as nearest waypoint
			near_id = index;
			near_dist = test_dist;
		} // end if

	} // end for index

	// test if user want to see all those lines
	if (vector_display_on == 1) {
		// draw it
		Draw_Line16(path[near_id].x, path[near_id].y - 8, path[near_id].x, path[near_id].y + 8,
			RGB16Bit(0, 0, 255), framebuffer, framebuffer_pitch);

		Draw_Line16(path[near_id].x - 8, path[near_id].y, path[near_id].x + 8, path[near_id].y,
			RGB16Bit(0, 0, 255), framebuffer, framebuffer_pitch);
	} // end if

	// return it
	return(near_id);

} // end Find_Nearest_Waypoint

////////////////////////////////////////////////////////////

void Draw_Waypoints(int mode = 1)
{
	// this function draws the waypoints and the path network

	for (int index = 0; index < NUM_WAYPOINTS; index++) {
		// draw network line too?
		if (mode > 0)
			Draw_Line16(path[index].x, path[index].y,
				path[(index + 1) % NUM_WAYPOINTS].x, path[(index + 1) % NUM_WAYPOINTS].y,
				RGB16Bit(255, 0, 0),
				framebuffer, framebuffer_pitch); // video buffer and memory pitch

		Draw_Pixel16(path[index].x, path[index].y, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);
		Draw_Pixel16(path[index].x + 1, path[index].y, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);
		Draw_Pixel16(path[index].x, path[index].y + 1, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);
		Draw_Pixel16(path[index].x + 1, path[index].y + 1, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	} // end for index

} // end Draw_Waypoints

////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	static float speed = 0; // speed of car

	static int curr_waypoint = -1,
		test_waypoint;

	static int display_debounce = 0; // used to debounce keyboard

	int ai_on = 1; // used to enable display ai based on user taking controls

	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap16(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// draw the waypoints
	if (vector_display_on == 1)
		Draw_Waypoints();

	// is user toggling vector display
	if (RETRO_KeyPressed(SDL_SCANCODE_D) && !display_debounce) {
		display_debounce = 1;
		vector_display_on = -vector_display_on;
	}  // end if

	if (!RETRO_KeyPressed(SDL_SCANCODE_D))
		display_debounce = 0;

	// these controls not really needed since AI drives...

	// allow player to move
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		if (++car.curr_frame > 15)
			car.curr_frame = 0;

		// turn off ai this cycle
		ai_on = 0;
	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
			if (--car.curr_frame < 0)
				car.curr_frame = 15;

			// turn off ai this cycle
			ai_on = 0;
		} // end if

	if (RETRO_KeyState(SDL_SCANCODE_UP)) {
		if ((speed += 0.1) > 4)
			speed = 4.0;

		// turn off ai this cycle
		ai_on = 0;
	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_DOWN)) {
			speed -= 0.1;

			// turn off ai this cycle
			ai_on = 0;
		} // end if

	 // ai speed control -- pretty dumb huh :)
	 // better to tie it to angle that it's turning
	if (ai_on)
		if ((speed += 0.05) > 4)
			speed = 4.0;

	// apply friction
	if ((speed -= 0.01) < 0)
		speed = 0;

	// let ai turn the car

	// find nearest waypoint first
	test_waypoint = (Find_Nearest_Waypoint(car.varsF[0], car.varsF[1]) + 1) % NUM_WAYPOINTS;

	// test if this is a new waypoint and not the last
	if (test_waypoint != curr_waypoint)
		curr_waypoint = test_waypoint;

	// compute vector toward waypoint
	float wx = (path[curr_waypoint].x - car.varsF[0]);
	float wy = (path[curr_waypoint].y - car.varsF[1]);

	// normalize wx, wy
	float length = Fast_Distance_2D(wx + 0.5, wy + 0.5);
	wx /= length;
	wy /= length;

	// compute angle in radians
	float car_angle = ((-90 + car.curr_frame * 22.5) * PI) / 180;

	// compute velocity vector of car (use look up for cos, sin in real life
	float xv = cos(car_angle);
	float yv = sin(car_angle);

	// draw vectors
	if (vector_display_on == 1) {
		Draw_Line16(car.varsF[0], car.varsF[1], path[curr_waypoint].x, path[curr_waypoint].y,
			250, framebuffer, framebuffer_pitch);

		Draw_Line16(car.varsF[0], car.varsF[1], car.varsF[0] + 16 * xv, car.varsF[1] + 16 * yv,
			246, framebuffer, framebuffer_pitch);
	} // end if

	// now turn car into waypoint direction, get sign of cross product
	// between each vector
	float sign_cross = (xv * wy - yv * wx);

	if (ai_on) {
		// test the sign to determine which way to turn plus a little slosh
		if (sign_cross > 0.3) {
			if (++car.curr_frame > 15)
				car.curr_frame = 0;

			sprintf(buffer, "AI Turning RIGHT");
			Draw_Text_GDI16(buffer, 320, 460, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

		} // end if
		else
			if (sign_cross < -0.3) {
				if (--car.curr_frame < 0)
					car.curr_frame = 15;

				sprintf(buffer, "AI Turning LEFT");
				Draw_Text_GDI16(buffer, 320, 460, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

			} // end if
			else {
				sprintf(buffer, "AI Turning 0");
				Draw_Text_GDI16(buffer, 320, 460, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

			} // end else
	} // end if
	else {
		sprintf(buffer, "AI Disabled by user override");
		Draw_Text_GDI16(buffer, 320, 460, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);
	} // end if

	// adjust floating position
	car.varsF[0] += (xv * speed);
	car.varsF[1] += (yv * speed);

	// test for off screen -- user could cause this
	if (car.varsF[0] > screen_width)
		car.varsF[0] = 0;
	else
		if (car.varsF[0] < 0)
			car.varsF[0] = screen_width;

	if (car.varsF[1] > screen_height)
		car.varsF[1] = 0;
	else
		if (car.varsF[1] < 0)
			car.varsF[1] = screen_height;

	// position sprite on pixel center
	car.x = (0.5 + car.varsF[0] - 8);
	car.y = (0.5 + car.varsF[1] - 8);

	// draw the car
	Draw_BOB16(&car, framebuffer, framebuffer_pitch);

	// show nearest waypoint
	sprintf(buffer, "Nearest Waypoint %d", Find_Nearest_Waypoint(car.varsF[0], car.varsF[1]));
	Draw_Text_GDI16(buffer, 64, 460, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	sprintf(buffer, "(16-Bit Version) Path following demo. Use arrow keys to override AI, <D> to toggle info, and <ESC> to Exit.");
	Draw_Text_GDI16(buffer, 8, 8, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	// sync to 30ish fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init16();

	// load background image
	Load_Bitmap_File(&bitmap16bit, "assets/racetrack24.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&background_bmp, &bitmap16bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// load the car bitmaps
	Load_Bitmap_File(&bitmap16bit, "assets/simimg24.bmp");

	// create car bob
	Create_BOB(&car, 334, 64, 16, 16, 16, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0, 0, 16);

	// set floating position and direction to west
	car.varsF[0] = car.x;
	car.varsF[1] = car.y;
	car.curr_frame = 12;

	// load the car in 
	for (int index = 0; index < 16; index++)
		Load_Frame_BOB16(&car, &bitmap16bit, index, index, 3, BITMAP_EXTRACT_MODE_CELL);

	// unload car
	Unload_Bitmap_File(&bitmap16bit);
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the bobs
	Destroy_BOB(&car);
}

/////////////////////////////////////////////////////////////////
