//
// DEMO12_3.CPP - vector tracking demo min game
// the vector tracking code can be found in Move_Mines()
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

// mathematical defines
const float FRICTION = 0.1;

// size of universe
#define UNIVERSE_MIN_X   (-8000)
#define UNIVERSE_MAX_X   (8000)
#define UNIVERSE_MIN_Y   (-8000)
#define UNIVERSE_MAX_Y   (8000)

// weapons defines
#define MAX_PLASMA         32   // max number of plasma pulses
#define PLASMA_SPEED       16   // pixels/sec of plasma pulse
#define PLASMA_SPEED_SLOW  12   // slow version for gunships

#define PLASMA_STATE_OFF   0   // this plasma is dead or off
#define PLASMA_STATE_ON    1   // this one is alive and in flight
#define PLASMA_RANGE_1     30  // type 1 plasma disruptor

#define PLASMA_ANIM_PLAYER     0  // a plasma torpedeo from player
#define PLASMA_ANIM_ENEMY      1  // a plasma torpedeo from any enemy

// asteroid field defines
#define MAX_ROCKS             300
#define ROCK_STATE_OFF        0   // this rock is dead or off
#define ROCK_STATE_ON         1   // this one is alive and in flight

#define ROCK_LARGE            0   // sizes of rock
#define ROCK_MEDIUM           1
#define ROCK_SMALL            2

// explosion defines 
#define MAX_BURSTS            8
#define BURST_STATE_OFF       0   // this burst is dead or off
#define BURST_STATE_ON        1   // this one is alive

// defines for player states
#define PLAYER_STATE_DEAD           0
#define PLAYER_STATE_DYING          1
#define PLAYER_STATE_INVINCIBLE     2
#define PLAYER_STATE_ALIVE          3
#define PLAYER_REGEN_COUNT          100 // click before regenerating

#define WRAITH_INDEX_DIR            3 // index of direction var

#define MAX_PLAYER_SPEED            16

// sound id's
#define MAX_FIRE_SOUNDS       8
#define MAX_EXPL_SOUNDS       8

// defines for stations
#define MAX_STATIONS           20

// life state for stations
#define STATION_STATE_DEAD       0
#define STATION_STATE_ALIVE      1
#define STATION_STATE_DAMAGED    2  
#define STATION_STATE_DYING      3

#define STATION_SHIELDS_ANIM_ON  0  // animations for shields
#define STATION_SHIELDS_ANIM_OFF 1 

#define STATION_RANGE_RING       300

#define INDEX_STATION_DAMAGE     2  // tracks the current damage of station
#define MAX_STATION_DAMAGE      100

#define STATION_MIN_UPLINK_DISTANCE  150

// defines for mines
#define MAX_MINES                16

// life state for stations
#define MINE_STATE_DEAD       0
#define MINE_STATE_ALIVE      1
#define MINE_STATE_DAMAGED    2  
#define MINE_STATE_DYING      3

#define MINE_STATE_AI_ACTIVATED   1
#define MINE_STATE_AI_SLEEP       0

#define INDEX_MINE_AI_STATE       1  // state of ai system
#define INDEX_MINE_DAMAGE         2  // tracks the current damage of MINE
#define INDEX_MINE_CONTACT_COUNT  3  // tracks how long mine has been in contact with player

#define MAX_MINE_DAMAGE            50 
#define MAX_MINE_CONTACT_COUNT     20

#define MAX_MINE_VELOCITY        16  
#define MIN_MINE_TRACKING_DIST   1000
#define MIN_MINE_ACTIVATION_DIST 250 

// defines for the star field

#define MAX_STARS                256   // number of stars in universe

#define STAR_PLANE_0             0    // far plane
#define STAR_PLANE_1             1    // near plane
#define STAR_PLANE_2             2

#define STAR_COLOR_0             8    // color of farthest star plane
#define STAR_COLOR_1             7
#define STAR_COLOR_2             15   // color of nearest star plane

// defines for particle system
#define PARTICLE_STATE_DEAD               0
#define PARTICLE_STATE_ALIVE              1

// types of particles
#define PARTICLE_TYPE_FLICKER             0
#define PARTICLE_TYPE_FADE                1 

// color of particle
#define PARTICLE_COLOR_RED                0
#define PARTICLE_COLOR_GREEN              1
#define PARTICLE_COLOR_BLUE               2
#define PARTICLE_COLOR_WHITE              3

#define MAX_PARTICLES                     128

// color ranges
#define COLOR_RED_START                   32
#define COLOR_RED_END                     47

#define COLOR_GREEN_START                 96
#define COLOR_GREEN_END                   111

#define COLOR_BLUE_START                  144
#define COLOR_BLUE_END                    159

#define COLOR_WHITE_START                 16
#define COLOR_WHITE_END                   31

// indices used to access data arrays in BOBs 
#define INDEX_WORLD_X                     8
#define INDEX_WORLD_Y                     9

// defines for the states of the main loop
#define GAME_STATE_INIT                  0
#define GAME_STATE_MENU                  1
#define GAME_STATE_RESTART               2
#define GAME_STATE_RUNNING               3
#define GAME_STATE_UPLINK                4
#define GAME_STATE_EXIT                  5
#define GAME_STATE_WAITING_FOR_EXIT      6
#define GAME_STATE_PAUSED                7

// the menu defines
#define MENU_STATE_MAIN                  0   // main menu state 
#define MENU_STATE_INST                  1   // instructions state
#define MENU_SEL_NEW_GAME                0
#define MENU_SEL_INSTRUCTIONS            1
#define MENU_SEL_EXIT                    2
#define MAX_INSTRUCTION_PAGES            6  

// defines for font
#define FONT_NUM_CHARS         96  // entire character set
#define FONT_WIDTH             12
#define FONT_HEIGHT            12
#define FONT_WIDTH_NEXT_NUM    8
#define FONT_WIDTH_NEXT_LOWER  7
#define FONT_WIDTH_NEXT_UPPER  8

// number of starting objects
#define NUM_ACTIVE_MINES             16
#define NUM_ACTIVE_STATIONS          8

inline void Copy_Palette(LPPALETTEENTRY dest, LPPALETTEENTRY source)
{
	memcpy(dest, source, 256 * sizeof(PALETTEENTRY));
}

// TYPES //////////////////////////////////////////////////

// used to contain a single star
typedef struct STAR_TYP
{
	int x, y;                   // position of star
	UCHAR color;               // color of star
	int plane;                 // plane that star is in

} STAR, *STAR_PTR;

// a single particle
typedef struct PARTICLE_TYP
{
	int state;           // state of the particle
	int type;            // type of particle effect
	int x, y;             // world position of particle
	int xv, yv;           // velocity of particle
	int curr_color;      // the current rendering color of particle
	int start_color;     // the start color or range effect
	int end_color;       // the ending color of range effect
	int counter;         // general state transition timer
	int max_count;       // max value for counter

} PARTICLE, *PARTICLE_PTR;

// GLOBALS ////////////////////////////////////////////////

char buffer[80];                // used to print text

BITMAP_IMAGE background_bmp, menu_bmp; // used to hold backgrounds
BITMAP_IMAGE menu_sel_bmps[3];   // holds the menu selections
BITMAP_IMAGE andre;

BOB wraith;                   // the player 
BOB plasma[MAX_PLASMA];       // plasma pulses
BOB rocks[MAX_ROCKS];         // the asteroids
BOB rock_s, rock_m, rock_l;   // the master templates for surface info
BOB bursts[MAX_BURSTS];       // the explosion bursts
BOB stations[MAX_STATIONS];   // the starbase stations
BOB mines[MAX_MINES];         // the predator mines
BOB hud;                      // the art for the scanner hud
BOB stationsmall;             // small station bob

int rock_sizes[3] = { 96,56,32 }; // X,Y dims for scaler

STAR stars[MAX_STARS];          // the star field

PARTICLE particles[MAX_PARTICLES]; // the particles for the particle engine

// player state variables
int player_state = PLAYER_STATE_ALIVE;
int player_score = 0;   // the score
int player_ships = 3;   // ships left
int player_regen_counter = 0; // used to track when to regen
int player_damage = 0;   // damage of player
int player_counter = 0;   // used for state transition tracking
int player_regen_count = 0;   // used to regenerate player
int player_shield_count = 0;  // used to display shields
int ready_counter = 0,        // used to draw a little "get ready"
ready_state = 0;

float mine_tracking_rate = 2; // rate that mines track player

int win_counter = 0,          // tracks if player won
player_won = 0;

int station_id = -1,          // uplink station id
num_stations_destroyed = 0;

int intro_done = 0;           // flags if intro has played already

int game_paused = 0,
pause_debounce = 0,
huds_on = 1,
scanner_on = 1,
huds_debounce = 0,
scanner_debounce = 0;

// color palettes, so we don't have to reload all the time
PALETTEENTRY         game_palette[256]; // holds the main game palette
PALETTEENTRY         menu_palette[256]; // gee what do you think
PALETTEENTRY         andre_palette[256]; // for me

// positional and state info for player
float player_x = 0,
player_y = 0,
player_dx = 0,
player_dy = 0,
player_xv = 0,
player_yv = 0,
vel = 0;

// these contain the virual cos, sin lookup tables for the 16 sector circle
float cos_look16[16],
sin_look16[16];

int expl_ids[MAX_EXPL_SOUNDS] = { -1,-1,-1,-1,-1,-1,-1,-1 };
int fire_ids[MAX_FIRE_SOUNDS] = { -1,-1,-1,-1,-1,-1,-1,-1 };
int game_state = GAME_STATE_INIT;   // initial game state

// FUNCTIONS //////////////////////////////////////////////

int Pad_Name(const char *filename, const char *extension, char *padstring, int num)
{
	char buffer[80];

	// build up blank padstring
	sprintf(padstring, "%s0000.%s", filename, extension);

	// this function pads a string with 0's
	snprintf(buffer, 10, "%d", num);

	// compute the position of the last digit
	int last_digit = strlen(filename) + 4 - 1;

	// now copy the number into the padstring at the correct position
	memcpy(padstring + last_digit - strlen(buffer) + 1, buffer, strlen(buffer));

	// return success
	return(1);

} // end Pad_Name

///////////////////////////////////////////////////////////

void Load_Andre(void)
{
	// this function loads all the buttons for the interface

	// load image
	Load_Bitmap_File(&bitmap8bit, "assets/enemyai.dat");

	// save the palette
	Copy_Palette(andre_palette, bitmap8bit.palette);

	Create_Bitmap(&andre, 320 - 216 / 2, 240 - 166 / 2, 216, 166);
	Load_Image_Bitmap(&andre, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);

	// unload object bitmap
	Unload_Bitmap_File(&bitmap8bit);

} // end Load_Andre

/////////////////////////////////////////////////////////////////

void Init_Reset_Particles(void)
{
	// this function serves as both an init and reset for the particles

	// loop thru and reset all the particles to dead
	for (int index = 0; index < MAX_PARTICLES; index++) {
		particles[index].state = PARTICLE_STATE_DEAD;
		particles[index].type = PARTICLE_TYPE_FADE;
		particles[index].x = 0;
		particles[index].y = 0;
		particles[index].xv = 0;
		particles[index].yv = 0;
		particles[index].start_color = 0;
		particles[index].end_color = 0;
		particles[index].curr_color = 0;
		particles[index].counter = 0;
		particles[index].max_count = 0;
	} // end if

} // end Init_Reset_Particles

/////////////////////////////////////////////////////////////////////////

void Start_Particle(int type, int color, int count, int x, int y, int xv, int yv)
{
	// this function starts a single particle

	int pindex = -1; // index of particle

	// first find open particle
	for (int index = 0; index < MAX_PARTICLES; index++)
		if (particles[index].state == PARTICLE_STATE_DEAD) {
			// set index
			pindex = index;
			break;
		} // end if    

	// did we find one
	if (pindex == -1)
		return;

	// set general state info
	particles[pindex].state = PARTICLE_STATE_ALIVE;
	particles[pindex].type = type;
	particles[pindex].x = x;
	particles[pindex].y = y;
	particles[pindex].xv = xv;
	particles[pindex].yv = yv;
	particles[pindex].counter = 0;
	particles[pindex].max_count = count;

	// set color ranges, always the same
	switch (color) {
	case PARTICLE_COLOR_RED:
	{
		particles[pindex].start_color = COLOR_RED_START;
		particles[pindex].end_color = COLOR_RED_END;
	} break;

	case PARTICLE_COLOR_GREEN:
	{
		particles[pindex].start_color = COLOR_GREEN_START;
		particles[pindex].end_color = COLOR_GREEN_END;
	} break;

	case PARTICLE_COLOR_BLUE:
	{
		particles[pindex].start_color = COLOR_BLUE_START;
		particles[pindex].end_color = COLOR_BLUE_END;
	} break;

	case PARTICLE_COLOR_WHITE:
	{
		particles[pindex].start_color = COLOR_WHITE_START;
		particles[pindex].end_color = COLOR_WHITE_END;
	} break;

	break;

	} // end switch

	// what type of particle is being requested
	if (type == PARTICLE_TYPE_FLICKER) {
		// set current color
		particles[pindex].curr_color = RAND_RANGE(particles[pindex].start_color, particles[pindex].end_color);

	} // end if
	else {
		// particle is fade type
		// set current color
		particles[pindex].curr_color = particles[pindex].start_color;
	} // end if

} // end Start_Particle

////////////////////////////////////////////////////////////////////////////////

void Start_Particle_Explosion(int type, int color, int count,
	int x, int y, int xv, int yv, int num_particles)
{
	// this function starts a particle explosion at the given position and velocity

	while (--num_particles >= 0) {
		// compute random trajectory angle
		int ang = rand() % 16;

		// compute random trajectory velocity
		float vel = 2 + rand() % 4;

		Start_Particle(type, color, count,
			x + RAND_RANGE(-4, 4), y + RAND_RANGE(-4, 4),
			xv + cos_look16[ang] * vel, yv + sin_look16[ang] * 16);

	} // end while

} // end Start_Particle_Explosion

////////////////////////////////////////////////////////////////////////////////

void Draw_Particles(void)
{
	// this function draws all the particles

	for (int index = 0; index < MAX_PARTICLES; index++) {
		// test if particle is alive
		if (particles[index].state == PARTICLE_STATE_ALIVE) {
			// render the particle, perform world to screen transform
			int x = particles[index].x - player_x + (screen_width / 2);
			int y = particles[index].y - player_y + (screen_height / 2);

			// test for clip
			if (x >= screen_width || x < 0 || y >= screen_height || y < 0)
				continue;

			// draw the pixel
			Draw_Pixel(x, y, particles[index].curr_color, framebuffer, framebuffer_pitch);

		} // end if

	} // end for index

} // end Draw_Particles

////////////////////////////////////////////////////////////////////

void Move_Particles(void)
{
	// this function moves and animates all particles

	for (int index = 0; index < MAX_PARTICLES; index++) {
		// test if this particle is alive
		if (particles[index].state == PARTICLE_STATE_ALIVE) {
			// translate particle
			particles[index].x += particles[index].xv;
			particles[index].y += particles[index].yv;

			// now based on type of particle perform proper animation
			if (particles[index].type == PARTICLE_TYPE_FLICKER) {
				// simply choose a color in the color range and assign it to the current color
				particles[index].curr_color = RAND_RANGE(particles[index].start_color, particles[index].end_color);

				// now update counter
				if (++particles[index].counter >= particles[index].max_count) {
					// kill the particle
					particles[index].state = PARTICLE_STATE_DEAD;

				} // end if

			} // end if
			else {
				// must be a fade, be careful!
				// test if it's time to update color
				if (++particles[index].counter >= particles[index].max_count) {
					// reset counter
					particles[index].counter = 0;

					// update color
					if (++particles[index].curr_color > particles[index].end_color) {
						// transition is complete, terminate particle
						particles[index].state = PARTICLE_STATE_DEAD;

					} // end if

				} // end if

			} // end else

		} // end if 

	} // end for index

} // end Move_Particles

////////////////////////////////////////////////////////////////////////////////////

void Init_Stars(void)
{
	// this function initializes all the stars in the star field

	int index; // looping variable

	for (index = 0; index < MAX_STARS; index++) {

		// select plane that star will be in

		switch (rand() % 3) {
		case STAR_PLANE_0:
		{
			stars[index].color = STAR_COLOR_0;
			stars[index].plane = STAR_PLANE_0;
		} break;

		case STAR_PLANE_1:
		{
			stars[index].color = STAR_COLOR_1;
			stars[index].plane = STAR_PLANE_1;
		} break;

		case STAR_PLANE_2:
		{
			stars[index].color = STAR_COLOR_2;
			stars[index].plane = STAR_PLANE_2;
		} break;

		default:break;

		} // end switch

		// set fields that aren't plane specific
		stars[index].x = rand() % screen_width;   // change this latter to reflect clipping
		stars[index].y = rand() % screen_height;  // region

	} // end for index

} // end Init_Stars

/////////////////////////////////////////////////////////////////////////////

void Move_Stars(void)
{
	// this function moves the star field, note that the star field is always
	// in screen coordinates, otherwise, we would need thousands of stars to
	// fill up the universe instead of 50!

	int index,   // looping variable
		star_x,  // used as fast aliases to star position
		star_y,
		plane_0_dx,
		plane_0_dy,
		plane_1_dx,
		plane_1_dy,
		plane_2_dx,
		plane_2_dy;

	// pre-compute plane translations
	plane_0_dx = -int(player_xv) >> 2;
	plane_0_dy = -int(player_yv) >> 2;

	plane_1_dx = -int(player_xv) >> 1;
	plane_1_dy = -int(player_yv) >> 1;

	plane_2_dx = -int(player_xv);
	plane_2_dy = -int(player_yv);

	// move all the stars based on the motion of the player

	for (index = 0; index < MAX_STARS; index++) {
		// locally cache star position to speed up calculations
		star_x = stars[index].x;
		star_y = stars[index].y;

		// test which star field star is in so it is translated with correct
		// perspective

		switch (stars[index].plane) {
		case STAR_PLANE_0:
		{
			// move the star based on differntial motion of player
			// far plane is divided by 4

			star_x += plane_0_dx;
			star_y += plane_0_dy;

		} break;

		case STAR_PLANE_1:
		{
			// move the star based on differntial motion of player
			// middle plane is divided by 2

			star_x += plane_1_dx;
			star_y += plane_1_dy;

		} break;

		case STAR_PLANE_2:
		{
			// move the star based on differntial motion of player
			// near plane is divided by 1

			star_x += plane_2_dx;
			star_y += plane_2_dy;

		} break;

		} // end switch plane

		// test if a star has flown off an edge

		if (star_x >= screen_width)
			star_x = star_x - screen_width;
		else
			if (star_x < 0)
				star_x = screen_width + star_x;

		if (star_y >= screen_height)
			star_y = star_y - screen_height;
		else
			if (star_y < 0)
				star_y = screen_height + star_y;

		// reset stars position in structure
		stars[index].x = star_x;
		stars[index].y = star_y;

	} // end for index

} // end Move_Stars

/////////////////////////////////////////////////////////////////////////////

void Draw_Stars(void)
{
	// this function draws all the stars

	// draw all the stars
	for (int index = 0; index < MAX_STARS; index++) {
		// draw stars 
		Draw_Pixel(stars[index].x, stars[index].y, stars[index].color, framebuffer, framebuffer_pitch);

	} // end for index

} // end Draw_Stars

///////////////////////////////////////////////////

void Draw_Scanner(void)
{
	// this function draws all the stars

	int index, sx, sy; // looping and position

	// draw all the rocks
	for (index = 0; index < MAX_ROCKS; index++) {
		// draw rock blips
		if (rocks[index].state == ROCK_STATE_ON) {
			sx = ((rocks[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (screen_width / 2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((rocks[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_Y) >> 7) + 32;

			Draw_Pixel(sx, sy, 8, framebuffer, framebuffer_pitch);
		} // end if

	} // end for index

	// draw all the mines
	for (index = 0; index < MAX_MINES; index++) {
		// draw gunship blips
		if (mines[index].state == MINE_STATE_ALIVE) {
			sx = ((mines[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (screen_width / 2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((mines[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_Y) >> 7) + 32;

			Draw_Pixel(sx, sy, 12, framebuffer, framebuffer_pitch);
			Draw_Pixel(sx, sy + 1, 12, framebuffer, framebuffer_pitch);

		} // end if

	} // end for index

	// draw all the stations
	for (index = 0; index < MAX_STATIONS; index++) {
		// draw station blips
		if (stations[index].state == STATION_STATE_ALIVE) {
			sx = ((stations[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (screen_width / 2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((stations[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_Y) >> 7) + 32;

			// test for state
			if (stations[index].anim_state == STATION_SHIELDS_ANIM_ON) {
				stationsmall.curr_frame = 0;
				stationsmall.x = sx - 3;
				stationsmall.y = sy - 3;
				Draw_BOB(&stationsmall, framebuffer, framebuffer_pitch);


			} // end if
			else {

				stationsmall.curr_frame = 1;
				stationsmall.x = sx - 3;
				stationsmall.y = sy - 3;
				Draw_BOB(&stationsmall, framebuffer, framebuffer_pitch);


			} // end if

		} // end if

	} // end for index

	// draw player as white blip
	sx = ((int(player_x) - UNIVERSE_MIN_X) >> 7) + (screen_width / 2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
	sy = ((int(player_y) - UNIVERSE_MIN_Y) >> 7) + 32;

	int col = rand() % 256;

	Draw_Pixel(sx, sy, col, framebuffer, framebuffer_pitch);
	Draw_Pixel(sx + 1, sy, col, framebuffer, framebuffer_pitch);
	Draw_Pixel(sx, sy + 1, col, framebuffer, framebuffer_pitch);
	Draw_Pixel(sx + 1, sy + 1, col, framebuffer, framebuffer_pitch);

	// now draw the art around the edges

	hud.x = 320 - 64;
	hud.y = 32 - 4;
	hud.curr_frame = 0;
	Draw_BOB(&hud, framebuffer, framebuffer_pitch);

	hud.x = 320 + 64 - 16;
	hud.y = 32 - 4;
	hud.curr_frame = 1;
	Draw_BOB(&hud, framebuffer, framebuffer_pitch);

	hud.x = 320 - 64;
	hud.y = 32 + 128 - 20;
	hud.curr_frame = 2;
	Draw_BOB(&hud, framebuffer, framebuffer_pitch);

	hud.x = 320 + 64 - 16;
	hud.y = 32 + 128 - 20;
	hud.curr_frame = 3;
	Draw_BOB(&hud, framebuffer, framebuffer_pitch);


} // end Draw_Scanner

///////////////////////////////////////////////////////////

void Init_Stations(void)
{
	// this function loads and initializes the stations to a known state

	static int shields_on_anim[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
		shields_off_anim[1] = { 16 };

	int frame;  // looping va

	// create the first bob
	Create_BOB(&stations[0], 0, 0, 192, 144, 17,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		0);

	// load animation frames 
	for (frame = 0; frame <= 16; frame++) {
		// load the rocks imagery 
		Pad_Name("assets/station", "bmp", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load the actual .BMP
		Load_Frame_BOB(&stations[0], &bitmap8bit, frame, 0, 0, BITMAP_EXTRACT_MODE_ABS);

		// unload data infile
		Unload_Bitmap_File(&bitmap8bit);

	} // end if

	// set state to off
	stations[0].state = STATION_STATE_ALIVE;

	// set anim state
	stations[0].anim_state = STATION_SHIELDS_ANIM_ON;

	// set damage to 0
	stations[0].varsI[INDEX_STATION_DAMAGE] = 0;

	// set animation rate
	Set_Anim_Speed_BOB(&stations[0], 15);

	// load in the shield on/off animations
	Load_Animation_BOB(&stations[0], STATION_SHIELDS_ANIM_ON, 16, shields_on_anim);
	Load_Animation_BOB(&stations[0], STATION_SHIELDS_ANIM_OFF, 1, shields_off_anim);

	// set animation to on
	Set_Animation_BOB(&stations[0], STATION_SHIELDS_ANIM_ON);

	// make copies
	for (int ship = 1; ship < MAX_STATIONS; ship++) {
		Clone_BOB(&stations[0], &stations[ship]);
	} // end for pulse


	// load the miniature station
	Create_BOB(&stationsmall, 0, 0, 8, 8, 2,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		0);

	Load_Bitmap_File(&bitmap8bit, "assets/stationsmall8.bmp");

	// load the actual .BMP
	Load_Frame_BOB(&stationsmall, &bitmap8bit, 0, 0, 0, BITMAP_EXTRACT_MODE_CELL);
	Load_Frame_BOB(&stationsmall, &bitmap8bit, 1, 1, 0, BITMAP_EXTRACT_MODE_CELL);

	// unload data infile
	Unload_Bitmap_File(&bitmap8bit);

} // end Init_Stations

///////////////////////////////////////////////////////////

void Start_Burst(int x, int y, int width, int height, int xv, int yv)
{
	// this function starts a burst up

	// now test if it's time to add a new burst to the list

	// scan for a burst to initialize
	for (int index = 0; index < MAX_BURSTS; index++) {
		// is this burst available?
		if (bursts[index].state == BURST_STATE_OFF) {
			// set animation rate
			Set_Anim_Speed_BOB(&bursts[index], 1);
			bursts[index].curr_frame = 0;

			// set velocity
			Set_Vel_BOB(&bursts[index], xv, yv);

			// set position
			Set_Pos_BOB(&bursts[index], 0, 0);
			bursts[index].varsI[INDEX_WORLD_X] = x;
			bursts[index].varsI[INDEX_WORLD_Y] = y;

			// set size of burst
			bursts[index].varsI[0] = width;
			bursts[index].varsI[1] = height;

			// turn burst on
			bursts[index].state = BURST_STATE_ON;

			// shoot some particles out
			Start_Particle_Explosion(PARTICLE_TYPE_FLICKER, RAND_RANGE(PARTICLE_COLOR_RED, PARTICLE_COLOR_GREEN), 20 + rand() % 40,
				bursts[index].varsI[INDEX_WORLD_X], bursts[index].varsI[INDEX_WORLD_Y],
				bursts[index].xv, bursts[index].yv,
				8 + rand() % 8);

			// later
			return;

		} // end if

	} // end for index

} // end Start_Burst

///////////////////////////////////////////////////////////

void Init_Mines(void)
{
	// this function loads and initializes the mines to a known state

	int frame;  // looping va

	// create the first bob
	Create_BOB(&mines[0], 0, 0, 48, 36, 16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		0);

	// load animation frames 
	for (frame = 0; frame <= 15; frame++) {
		// load the mine
		Pad_Name("assets/predmine", "bmp", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load the actual .BMP
		Load_Frame_BOB(&mines[0], &bitmap8bit, frame, 0, 0, BITMAP_EXTRACT_MODE_ABS);

		// unload data infile
		Unload_Bitmap_File(&bitmap8bit);

	} // end if

	// set state to off
	mines[0].state = MINE_STATE_DEAD;

	// set ai state to sleep mode
	mines[0].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_SLEEP;

	// set damage to 0
	mines[0].varsI[INDEX_MINE_DAMAGE] = 0;

	// set contact count to 0
	mines[0].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

	// set animation rate
	Set_Anim_Speed_BOB(&mines[0], 3);

	// make copies
	for (int mine = 1; mine < MAX_MINES; mine++) {
		Clone_BOB(&mines[0], &mines[mine]);
	} // end for mine

} // end Init_Mines

////////////////////////////////////////////////////////////

void Reset_Mines(void)
{
	// this functions resets all the mines

	// make copies
	for (int mine = 0; mine < MAX_MINES; mine++) {
		// set state to off
		mines[mine].state = MINE_STATE_DEAD;

		// set ai state to sleep mode
		mines[mine].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_SLEEP;

		// set damage to 0
		mines[mine].varsI[INDEX_MINE_DAMAGE] = 0;

		// set contact count to 0
		mines[mine].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

	} // end for mine

} // end Reset_Mines

///////////////////////////////////////////////////////////

void Start_Mine(int override = 0, int x = 0, int y = 0, int ai_state = MINE_STATE_AI_ACTIVATED)
{
	// this functions starts a mine, note that if override = 1
	// then the function uses the sent data otherwise it's random

	// first find an available stations
	for (int index = 0; index < MAX_MINES; index++) {
		// is this one dead
		if (mines[index].state == MINE_STATE_DEAD) {

			if (!override) {
				// position the mine
				int xpos = RAND_RANGE((UNIVERSE_MIN_X + 256), (UNIVERSE_MAX_X - 256));
				int ypos = RAND_RANGE((UNIVERSE_MIN_Y + 256), (UNIVERSE_MAX_Y - 256));

				// set position
				mines[index].varsI[INDEX_WORLD_X] = xpos;
				mines[index].varsI[INDEX_WORLD_Y] = ypos;

				// set velocity
				mines[index].xv = RAND_RANGE(-8, 8);
				mines[index].yv = RAND_RANGE(-8, 8);

				// set remaining state variables
				mines[index].state = MINE_STATE_ALIVE;

				// set damage to 0
				mines[index].varsI[INDEX_MINE_DAMAGE] = 0;

				// set ai state
				mines[index].varsI[INDEX_MINE_AI_STATE] = ai_state;

				// set contact count
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

				// done so exit
				return;

			} else {
				// set position
				mines[index].varsI[INDEX_WORLD_X] = x;
				mines[index].varsI[INDEX_WORLD_Y] = y;

				// set velocity
				mines[index].xv = 0;
				mines[index].yv = 0;

				// set remaining state variables
				mines[index].state = MINE_STATE_ALIVE;

				// set damage to 0
				mines[index].varsI[INDEX_MINE_DAMAGE] = 0;

				// set ai state
				mines[index].varsI[INDEX_MINE_AI_STATE] = ai_state;

				// set contact count
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

				// done so exit
				return;

			} // end else


		} // end if alive

	} // end for index

} // end Start_Mine

////////////////////////////////////////////////////////////

void Move_Mines(void)
{
	// this function moves/animates all the mines

	for (int index = 0; index < MAX_MINES; index++) {
		// test if station is alive
		if (mines[index].state == MINE_STATE_ALIVE) {

			// test for activation
			if (mines[index].varsI[INDEX_MINE_AI_STATE] == MINE_STATE_AI_ACTIVATED) {

				// move the mines
				mines[index].varsI[INDEX_WORLD_X] += mines[index].xv;
				mines[index].varsI[INDEX_WORLD_Y] += mines[index].yv;

				// add damage trails
				if (mines[index].varsI[INDEX_MINE_DAMAGE] > (MAX_MINE_DAMAGE >> 1) &&
					(rand() % 3) == 1) {
					Start_Particle(PARTICLE_TYPE_FLICKER, PARTICLE_COLOR_WHITE, 30 + rand() % 25,
						mines[index].varsI[INDEX_WORLD_X] + RAND_RANGE(-4, 4),
						mines[index].varsI[INDEX_WORLD_Y] + RAND_RANGE(-4, 4),
						(mines[index].xv * .125), (mines[index].yv * .125));

					Start_Particle(PARTICLE_TYPE_FADE, PARTICLE_COLOR_RED, 5,
						mines[index].varsI[INDEX_WORLD_X] + RAND_RANGE(-4, 4),
						mines[index].varsI[INDEX_WORLD_Y] + RAND_RANGE(-4, 4),
						(mines[index].xv * .125), (mines[index].yv * .125));

				} // end if

				// tracking algorithm

				// compute vector toward player
				float vx = player_x - mines[index].varsI[INDEX_WORLD_X];
				float vy = player_y - mines[index].varsI[INDEX_WORLD_Y];

				// normalize vector (sorta :)
				float length = Fast_Distance_2D(vx, vy);

				// only track if reasonable close
				if (length < MIN_MINE_TRACKING_DIST) {
					vx = mine_tracking_rate * vx / length;
					vy = mine_tracking_rate * vy / length;

					// add velocity vector to current velocity
					mines[index].xv += vx;
					mines[index].yv += vy;

					// add a little noise
					if ((rand() % 10) == 1) {
						vx = RAND_RANGE(-1, 1);
						vy = RAND_RANGE(-1, 1);
						mines[index].xv += vx;
						mines[index].yv += vy;
					} // end if

					// test velocity vector of mines
					length = Fast_Distance_2D(mines[index].xv, mines[index].yv);

					// test for velocity overflow and slow
					if (length > MAX_MINE_VELOCITY) {
						// slow down
						mines[index].xv *= 0.75;
						mines[index].yv *= 0.75;

					} // end if

				} // end if
				else {
					// add a random velocity component
					if ((rand() % 30) == 1) {
						vx = RAND_RANGE(-2, 2);
						vy = RAND_RANGE(-2, 2);

						// add velocity vector to current velocity
						mines[index].xv += vx;
						mines[index].yv += vy;

						// test velocity vector of mines
						length = Fast_Distance_2D(mines[index].xv, mines[index].yv);

						// test for velocity overflow and slow
						if (length > MAX_MINE_VELOCITY) {
							// slow down
							mines[index].xv *= 0.75;
							mines[index].yv *= 0.75;

						} // end if

					} // end if

				} // end else

			} // end if activated
			else {
				// compute dist from player
				float vx = player_x - mines[index].varsI[INDEX_WORLD_X];
				float vy = player_y - mines[index].varsI[INDEX_WORLD_Y];

				float length = Fast_Distance_2D(vx, vy);

				if (length < MIN_MINE_ACTIVATION_DIST) {
					mines[index].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_ACTIVATED;

				} // end if

			} // end else

			// test for boundaries
			if (mines[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X) {
				mines[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			} // end if
			else
				if (mines[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X) {
					mines[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;
				} // end if	    

			if (mines[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y) {
				mines[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_Y;
			} // end if
			else
				if (mines[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y) {
					mines[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;
				} // end if

			// check for collision with player
			if (player_state == PLAYER_STATE_ALIVE && player_state == PLAYER_STATE_ALIVE &&
				Collision_Test(player_x - (wraith.width * .5),
					player_y - (wraith.height * .5),
					wraith.width, wraith.height,
					mines[index].varsI[INDEX_WORLD_X] - (mines[index].width * .5),
					mines[index].varsI[INDEX_WORLD_Y] - (mines[index].height * .5),
					mines[index].width, mines[index].height)) {

				// test for contact count
				if (++mines[index].varsI[INDEX_MINE_CONTACT_COUNT] > MAX_MINE_CONTACT_COUNT) {

					// add to players score
					player_score += 250;

					// damage player
					player_damage += 30;

					// engage shields
					player_shield_count = 3;

					int width = 30 + rand() % 40;

					// start a burst
					Start_Burst(mines[index].varsI[INDEX_WORLD_X],
						mines[index].varsI[INDEX_WORLD_Y],
						width, (width * .5) + (width * .25),
						int(mines[index].xv) * .5, int(mines[index].yv) * .5);


					// kill the mine
					mines[index].state = MINE_STATE_DEAD;

					// start a new mine
					Start_Mine();

					// process next mine
					continue;

				} // end if contact count

			} // end if
			else // no collision or other problem, so reset
			{
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;
			} // end else

		} // end if alive

	} // end for index

} // end Move_Mines

///////////////////////////////////////////////////////////

void Draw_Mines(void)
{
	// this function draws all the mines

	for (int index = 0; index < MAX_MINES; index++) {
		// test if mine is alive
		if (mines[index].state == MINE_STATE_ALIVE) {
			// transform to screen coords
			mines[index].x = mines[index].varsI[INDEX_WORLD_X] - (mines[index].width >> 1) - player_x + (screen_width / 2);
			mines[index].y = mines[index].varsI[INDEX_WORLD_Y] - (mines[index].height >> 1) - player_y + (screen_height / 2);

			// draw the station
			Draw_BOB(&mines[index], framebuffer, framebuffer_pitch);

			// animate the mine
			Animate_BOB(&mines[index]);

			// draw info text above mine
			sprintf(buffer, "VELOCITY[%f, %f]", mines[index].xv, mines[index].yv);
			Draw_Text_GDI(buffer, mines[index].x - 32, mines[index].y - 16, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

		} // end if

	} // end for index

} // end Draw_Mines

////////////////////////////////////////////////////////////

void Delete_Mines(void)
{
	// this function simply deletes all memory and surfaces
	// related to the mines

	for (int index = 0; index < MAX_MINES; index++)
		Destroy_BOB(&mines[index]);

} // end Delete_Mines

//////////////////////////////////////////////////////

void Reset_Stations(void)
{
	// this function resets all the stations in preparation for another run

	for (int index = 0; index < MAX_STATIONS; index++) {
		// set state to off
		stations[index].state = STATION_STATE_DEAD;

		// set anim state
		stations[index].anim_state = STATION_SHIELDS_ANIM_ON;

		// set damage to 0
		stations[index].varsI[INDEX_STATION_DAMAGE] = 0;

	} // end for

} // end Reset_Stations

///////////////////////////////////////////////////////////

void Start_Station(int override = 0, int x = 0, int y = 0)
{
	// this functions starts a station, note that if override = 1
	// then the function uses the sent data otherwise it's random

	// first find an available stations
	for (int index = 0; index < MAX_STATIONS; index++) {
		// is this one dead
		if (stations[index].state == STATION_STATE_DEAD) {
			// position the station

			int xpos = RAND_RANGE((UNIVERSE_MIN_X + 256), (UNIVERSE_MAX_X - 256));
			int ypos = RAND_RANGE((UNIVERSE_MIN_Y + 256), (UNIVERSE_MAX_Y - 256));

			// set position
			stations[index].varsI[INDEX_WORLD_X] = xpos;
			stations[index].varsI[INDEX_WORLD_Y] = ypos;

			// start a mine up in the vicinity
			int ang = rand() % 16;
			float mine_x = xpos + STATION_RANGE_RING * cos_look16[ang];
			float mine_y = ypos + STATION_RANGE_RING * sin_look16[ang];

			// start a deactivated mine
			Start_Mine(1, mine_x, mine_y, MINE_STATE_AI_SLEEP);

			// set velocity
			stations[index].xv = 0;
			stations[index].yv = 0;

			// set remaining state variables
			stations[index].state = STATION_STATE_ALIVE;

			// set animation to on
			Set_Animation_BOB(&stations[index], STATION_SHIELDS_ANIM_ON);

			// set anim state
			stations[index].anim_state = STATION_SHIELDS_ANIM_ON;

			// set damage to 0
			stations[index].varsI[INDEX_STATION_DAMAGE] = 0;

			// done so exit
			return;

		} // end if

	} // end for index

} // end Start_Station

////////////////////////////////////////////////////////////

void Move_Stations(void)
{
	// this function moves/animates all the stations

	for (int index = 0; index < MAX_STATIONS; index++) {
		// test if station is alive
		if (stations[index].state == STATION_STATE_ALIVE) {
			// move the stations
			stations[index].varsI[INDEX_WORLD_X] += stations[index].xv;
			stations[index].varsI[INDEX_WORLD_Y] += stations[index].yv;

			// test for boundaries
			if (stations[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X) {
				stations[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			} // end if
			else
				if (stations[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X) {
					stations[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;
				} // end if	    

			if (stations[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y) {
				stations[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_Y;
			} // end if
			else
				if (stations[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y) {
					stations[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;
				} // end if

			 // test for damage level
			if (stations[index].varsI[INDEX_STATION_DAMAGE] > (MAX_STATION_DAMAGE / 4) &&
				(rand() % (20 - (stations[index].varsI[INDEX_STATION_DAMAGE] >> 3))) == 1) {
				int width = 20 + rand() % 60;

				// start a burst
				Start_Burst(stations[index].varsI[INDEX_WORLD_X] - (stations[index].width * .5) + RAND_RANGE(0, stations[index].width),
					stations[index].varsI[INDEX_WORLD_Y] - (stations[index].height * .5) + RAND_RANGE(0, stations[index].height),
					width, (width >> 2) + (width >> 1),
					int(stations[index].xv) * .5, int(stations[index].yv) * .5);

				// add some particles

			} // end if

		} // end if alive

	} // end for index

} // end Move_Stations

///////////////////////////////////////////////////////////

void Draw_Stations(void)
{
	// this function draws all the stations

	for (int index = 0; index < MAX_STATIONS; index++) {
		// test if station is alive
		if (stations[index].state == STATION_STATE_ALIVE) {
			// transform to screen coords
			stations[index].x = stations[index].varsI[INDEX_WORLD_X] - (stations[index].width >> 1) - player_x + (screen_width / 2);
			stations[index].y = stations[index].varsI[INDEX_WORLD_Y] - (stations[index].height >> 1) - player_y + (screen_height / 2);

			// draw the station
			Draw_BOB(&stations[index], framebuffer, framebuffer_pitch);

			// animate the station
			Animate_BOB(&stations[index]);

		} // end if

	} // end for index

} // end Draw_Stations

////////////////////////////////////////////////////////////

void Delete_Stations(void)
{
	// this function simply deletes all memory and surfaces
	// related to the gunships

	for (int index = 0; index < MAX_STATIONS; index++)
		Destroy_BOB(&stations[index]);

} // end Delete_Stations

///////////////////////////////////////////////////////////

void Init_Rocks(void)
{
	// this function initializes and loads all the rocks 

	int frame; // used as loop index

	// create the large rock
	Create_BOB(&rock_l, 0, 0, 96, 96, 16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		0);

	// load animation frames
	for (frame = 0; frame <= 15; frame++) {
		// load the rocks imagery 
		Pad_Name("assets/rockl", "bmp", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load the actual .BMP
		Load_Frame_BOB(&rock_l, &bitmap8bit, frame, 0, 0, BITMAP_EXTRACT_MODE_ABS);

		// unload data infile
		Unload_Bitmap_File(&bitmap8bit);

	} // end if

	// set animation rate
	Set_Anim_Speed_BOB(&rock_l, 1 + rand() % 5);
	Set_Vel_BOB(&rock_l, -4 + rand() % 8, 4 + rand() % 4);
	Set_Pos_BOB(&rock_l, rand() % screen_width, rand() % screen_height);

	// set size of rock
	rock_l.varsI[0] = ROCK_LARGE;
	rock_l.varsI[1] = rock_sizes[rock_l.varsI[0]];

	// set state to off
	rock_l.state = ROCK_STATE_OFF;

	// create the medium rock
	Create_BOB(&rock_m, 0, 0, 56, 56, 16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		0);

	// load animation frames
	for (frame = 0; frame <= 15; frame++) {
		// load the rocks imagery 
		Pad_Name("assets/rockm", "bmp", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load the actual .BMP
		Load_Frame_BOB(&rock_m, &bitmap8bit, frame, 0, 0, BITMAP_EXTRACT_MODE_ABS);

		// unload data infile
		Unload_Bitmap_File(&bitmap8bit);

	} // end if

	// set animation rate
	Set_Anim_Speed_BOB(&rock_m, 1 + rand() % 5);
	Set_Vel_BOB(&rock_m, -4 + rand() % 8, 4 + rand() % 4);
	Set_Pos_BOB(&rock_m, rand() % screen_width, rand() % screen_height);

	// set size of rock
	rock_m.varsI[0] = ROCK_MEDIUM;
	rock_m.varsI[1] = rock_sizes[rock_m.varsI[0]];

	// set state to off
	rock_m.state = ROCK_STATE_OFF;

	// create the small rock
	Create_BOB(&rock_s, 0, 0, 32, 32, 16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		0);

	// load animation frames
	for (frame = 0; frame <= 15; frame++) {
		// load the rocks imagery 
		Pad_Name("assets/rocks", "bmp", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load the actual .BMP
		Load_Frame_BOB(&rock_s, &bitmap8bit, frame, 0, 0, BITMAP_EXTRACT_MODE_ABS);

		// unload data infile
		Unload_Bitmap_File(&bitmap8bit);

	} // end if

	// set animation rate
	Set_Anim_Speed_BOB(&rock_s, 1 + rand() % 5);
	Set_Vel_BOB(&rock_s, -4 + rand() % 8, -4 + rand() % 8);
	Set_Pos_BOB(&rock_s, rand() % screen_width, rand() % screen_height);

	// set size of rock
	rock_s.varsI[0] = ROCK_SMALL;
	rock_s.varsI[1] = rock_sizes[rock_s.varsI[0]];

	// set state to off
	rock_s.state = ROCK_STATE_OFF;

	// make semi copies
	for (int rock = 0; rock < MAX_ROCKS; rock++) {
		Clone_BOB(&rock_l, &rocks[rock]);

		// set animation rate
		Set_Anim_Speed_BOB(&rocks[rock], 1 + rand() % 5);

		// set velocity
		Set_Vel_BOB(&rocks[rock], -4 + rand() % 8, -4 + rand() % 8);

		// set position
		Set_Pos_BOB(&rocks[rock], 0, 0);
		rocks[rock].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
		rocks[rock].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

		// set size of rock
		rocks[rock].varsI[0] = rand() % 3;
		rocks[rock].varsI[1] = rock_sizes[rocks[rock].varsI[0]];

		// based on size of rock switch in correct bitmaps and sizes
		switch (rocks[rock].varsI[1]) {
		case ROCK_LARGE:
		{
			// copy dd bitmap surfaces
			memcpy(rocks[rock].images, rock_l.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

			// set width and height (incase)
			rocks[rock].width = rock_l.width;
			rocks[rock].height = rock_l.height;

		} break;

		case ROCK_MEDIUM:
		{
			// copy dd bitmap surfaces
			memcpy(rocks[rock].images, rock_m.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

			// set width and height (incase)
			rocks[rock].width = rock_m.width;
			rocks[rock].height = rock_m.height;

		} break;

		case ROCK_SMALL:
		{
			// copy dd bitmap surfaces
			memcpy(rocks[rock].images, rock_s.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

			// set width and height (incase)
			rocks[rock].width = rock_s.width;
			rocks[rock].height = rock_s.height;

		} break;

		default: break;

		} // end switch

	} // end for rock

} // end Init_Rocks

//////////////////////////////////////////////////////////////////////////////

void Reset_Rocks(void)
{
	// this function resets all the rocks

	for (int rock = 0; rock < MAX_ROCKS; rock++) {
		// reset state
		rocks[rock].state = ROCK_STATE_OFF;

	} // end for rocks

} // end Reset_Rocks

//////////////////////////////////////////////////////////////////////////////

void Delete_Rocks(void)
{
	// this function simply deletes all memory and surfaces
	// related to the rocks pulses

	for (int index = 0; index < MAX_ROCKS; index++)
		Destroy_BOB(&rocks[index]);

} // end Delete_Rocks

//////////////////////////////////////////////////////////////////////////////

void Seed_Rocks(void)
{
	// this function seeds the universe with rocks

	int index; // looping var

	// scan for a rock to initialize
	for (index = 0; index < (MAX_ROCKS * .75); index++) {
		// is this rock available?
		if (rocks[index].state == ROCK_STATE_OFF) {
			// set animation rate
			Set_Anim_Speed_BOB(&rocks[index], 1 + rand() % 5);

			// set velocity
			Set_Vel_BOB(&rocks[index], -6 + rand() % 12, -6 + rand() % 12);

			// set position
			Set_Pos_BOB(&rocks[index], 0, 0);
			rocks[index].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
			rocks[index].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

			// set size of rock
			rocks[index].varsI[0] = rand() % 3;
			rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

			// set strength of rock
			switch (rocks[index].varsI[0]) {
			case ROCK_LARGE:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 100 + rand() % 100;

				// copy dd bitmap surfaces
				memcpy(rocks[index].images, rock_l.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

				// set width and height (incase)
				rocks[index].width = rock_l.width;
				rocks[index].height = rock_l.height;

			} break;

			case ROCK_MEDIUM:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 40 + rand() % 30;

				// copy dd bitmap surfaces
				memcpy(rocks[index].images, rock_m.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

				// set width and height (incase)
				rocks[index].width = rock_m.width;
				rocks[index].height = rock_m.height;

			} break;

			case ROCK_SMALL:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 10;

				// copy dd bitmap surfaces
				memcpy(rocks[index].images, rock_s.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

				// set width and height (incase)
				rocks[index].width = rock_s.width;
				rocks[index].height = rock_s.height;
			} break;

			default:break;

			} // end switch

			// turn rock on
			rocks[index].state = ROCK_STATE_ON;

		} // end if

	} // end for index

} // end Seed_Rocks

//////////////////////////////////////////////////////////

void Move_Rocks(void)
{
	// this function moves all the rocks pulses and checks for
	// collision with the rocks

	for (int index = 0; index < MAX_ROCKS; index++) {
		// test if rocks pulse is in flight
		if (rocks[index].state == ROCK_STATE_ON) {
			// move the rock
			rocks[index].varsI[INDEX_WORLD_X] += rocks[index].xv;
			rocks[index].varsI[INDEX_WORLD_Y] += rocks[index].yv;

			// test if rock is out of universe, but persist object
			if (rocks[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X)
				rocks[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			else
				if (rocks[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X)
					rocks[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;

			if (rocks[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_X)
				rocks[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_X;
			else
				if (rocks[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y)
					rocks[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;

			// test for collision with rocks
			if (player_state == PLAYER_STATE_ALIVE && Collision_Test(player_x - (wraith.width * .5), player_y - (wraith.height * .5),
				wraith.width, wraith.height,
				rocks[index].varsI[INDEX_WORLD_X] - (rocks[index].width * .5),
				rocks[index].varsI[INDEX_WORLD_Y] - (rocks[index].height * .5),
				rocks[index].width, rocks[index].height)) {
				// what size rock did we hit?

				switch (rocks[index].varsI[0]) {
				case ROCK_LARGE:
				{
					// start explosion
					Start_Burst(rocks[index].varsI[INDEX_WORLD_X], rocks[index].varsI[INDEX_WORLD_Y],
						68 + rand() % 12, 54 + rand() % 10,
						rocks[index].xv * .5, rocks[index].yv * .5);

					// update players damage
					player_damage += 35;

					// update velocity vector
					player_xv += (rocks[index].xv);
					player_yv += (rocks[index].yv);

					// rotate ship a bit
					wraith.varsI[WRAITH_INDEX_DIR] += (RAND_RANGE(-4, 4));
					if (wraith.varsI[WRAITH_INDEX_DIR] > 15)
						wraith.varsI[WRAITH_INDEX_DIR] -= 15;
					else
						if (wraith.varsI[WRAITH_INDEX_DIR] < 0)
							wraith.varsI[WRAITH_INDEX_DIR] += 15;

				} break;

				case ROCK_MEDIUM:
				{
					// start explosion
					Start_Burst(rocks[index].varsI[INDEX_WORLD_X], rocks[index].varsI[INDEX_WORLD_Y],
						52 + rand() % 10, 44 + rand() % 8,
						rocks[index].xv * .5, rocks[index].yv * .5);

					// update players damage
					player_damage += 15;

					// update velocity vector
					player_xv += (rocks[index].xv * .5);
					player_yv += (rocks[index].yv * .5);

					// rotate ship a bit
					wraith.varsI[WRAITH_INDEX_DIR] += (RAND_RANGE(-4, 4));
					if (wraith.varsI[WRAITH_INDEX_DIR] > 15)
						wraith.varsI[WRAITH_INDEX_DIR] -= 15;
					else
						if (wraith.varsI[WRAITH_INDEX_DIR] < 0)
							wraith.varsI[WRAITH_INDEX_DIR] += 15;

				} break;

				case ROCK_SMALL:
				{
					// start explosion
					Start_Burst(rocks[index].varsI[INDEX_WORLD_X], rocks[index].varsI[INDEX_WORLD_Y],
						34 - 4 + rand() % 8, 30 - 3 + rand() % 6,
						rocks[index].xv * .5, rocks[index].yv * .5);

					// update players damage
					player_damage += 5;

					// update velocity vector
					player_xv += (rocks[index].xv * .25);
					player_yv += (rocks[index].yv * .25);

					// rotate ship a bit
					wraith.varsI[WRAITH_INDEX_DIR] += (RAND_RANGE(-4, 4));
					if (wraith.varsI[WRAITH_INDEX_DIR] > 15)
						wraith.varsI[WRAITH_INDEX_DIR] -= 15;
					else
						if (wraith.varsI[WRAITH_INDEX_DIR] < 0)
							wraith.varsI[WRAITH_INDEX_DIR] += 15;

				} break;

				} // end switch

				// update score
				player_score += rocks[index].varsI[2];

				// engage shields
				player_shield_count = 3;

				// kill the original
				rocks[index].state = ROCK_STATE_OFF;

			} // end if collision

		} // end if rock

	} // end for index

	// now test if it's time to add a new rock to the list
	if ((rand() % 100) == 50) {
		// scan for a rock to initialize
		for (int index = 0; index < MAX_ROCKS; index++) {
			// is this rock available?
			if (rocks[index].state == ROCK_STATE_OFF) {
				// set animation rate
				Set_Anim_Speed_BOB(&rocks[index], 1 + rand() % 5);

				// set velocity
				Set_Vel_BOB(&rocks[index], -6 + rand() % 12, -6 + rand() % 12);

				// set position
				Set_Pos_BOB(&rocks[index], 0, 0);
				rocks[index].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
				rocks[index].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

				// set size of rock
				rocks[index].varsI[0] = rand() % 3;
				rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

				// set strength of rock
				switch (rocks[index].varsI[0]) {
				case ROCK_LARGE:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 100 + rand() % 100;

					// copy dd bitmap surfaces
					memcpy(rocks[index].images, rock_l.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

					// set width and height (incase)
					rocks[index].width = rock_l.width;
					rocks[index].height = rock_l.height;

				} break;

				case ROCK_MEDIUM:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 40 + rand() % 30;

					// copy dd bitmap surfaces
					memcpy(rocks[index].images, rock_m.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

					// set width and height (incase)
					rocks[index].width = rock_m.width;
					rocks[index].height = rock_m.height;

				} break;

				case ROCK_SMALL:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 10;

					// copy dd bitmap surfaces
					memcpy(rocks[index].images, rock_s.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

					// set width and height (incase)
					rocks[index].width = rock_s.width;
					rocks[index].height = rock_s.height;
				} break;

				default:break;

				} // end switch

				// turn rock on
				rocks[index].state = ROCK_STATE_ON;

				// later
				return;

			} // end if

		} // end for index

	} // end if

} // end Move_Rocks

///////////////////////////////////////////////////////////

void Start_Rock(int x, int y, int size, int xv, int yv)
{
	// this functions starts a rock up with the sent parms, considering
	// there is one left

	// scan for a rock to initialize
	for (int index = 0; index < MAX_ROCKS; index++) {
		// is this rock available?
		if (rocks[index].state == ROCK_STATE_OFF) {
			// set animation rate
			Set_Anim_Speed_BOB(&rocks[index], 1 + rand() % 5);

			// set velocity
			Set_Vel_BOB(&rocks[index], xv, yv);

			// set position
			Set_Pos_BOB(&rocks[index], 0, 0);
			rocks[index].varsI[INDEX_WORLD_X] = x;
			rocks[index].varsI[INDEX_WORLD_Y] = y;

			// set size of rock
			rocks[index].varsI[0] = size;
			rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

			// set strength of rock
			switch (rocks[index].varsI[0]) {
			case ROCK_LARGE:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 100 + rand() % 100;

				// copy dd bitmap surfaces
				memcpy(rocks[index].images, rock_l.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

				// set width and height (incase)
				rocks[index].width = rock_l.width;
				rocks[index].height = rock_l.height;

			} break;

			case ROCK_MEDIUM:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 40 + rand() % 30;

				// copy dd bitmap surfaces
				memcpy(rocks[index].images, rock_m.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

				// set width and height (incase)
				rocks[index].width = rock_m.width;
				rocks[index].height = rock_m.height;
			} break;

			case ROCK_SMALL:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 10;

				// copy dd bitmap surfaces
				memcpy(rocks[index].images, rock_s.images, sizeof(UCHAR *) * MAX_BOB_FRAMES);

				// set width and height (incase)
				rocks[index].width = rock_s.width;
				rocks[index].height = rock_s.height;
			} break;

			default:break;

			} // end switch

			// turn rock on
			rocks[index].state = ROCK_STATE_ON;

			// later
			return;

		} // end if

	} // end for index

} // end Start_Rock

///////////////////////////////////////////////////////////

void Draw_Rocks(void)
{
	// this function draws all the rocks 

	for (int index = 0; index < MAX_ROCKS; index++) {
		// test if rocks pulse is in flight
		if (rocks[index].state == ROCK_STATE_ON) {
			// transform to screen coords
			rocks[index].x = rocks[index].varsI[INDEX_WORLD_X] - (rocks[index].width * .5) - player_x + (screen_width / 2);
			rocks[index].y = rocks[index].varsI[INDEX_WORLD_Y] - (rocks[index].height * .5) - player_y + (screen_height / 2);

			// draw the rock
			Draw_BOB(&rocks[index], framebuffer, framebuffer_pitch);

			// animate the pulse
			Animate_BOB(&rocks[index]);

		} // end if

	} // end for index

} // end Draw_Rocks

///////////////////////////////////////////////////////////

void Init_Plasma(void)
{
	// this function initializes and loads all the plasma 
	// weapon pulses

	// the plasma animations
	static int plasma_anim_player[] = { 0,1,2,3,4,5,6,7 },
		plasma_anim_enemy[] = { 8,9,10,11,12,13,14,15 };

	// load the plasma imagery 
	Load_Bitmap_File(&bitmap8bit, "assets/energy8.bmp");

	// create the first bob
	Create_BOB(&plasma[0], 0, 0, 8, 8, 16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		0);

	// load animation frames
	for (int frame = 0; frame < 16; frame++)
		Load_Frame_BOB(&plasma[0], &bitmap8bit, frame, frame % 8, frame / 8, BITMAP_EXTRACT_MODE_CELL);

	// set animation rate
	Set_Anim_Speed_BOB(&plasma[0], 1);

	// load animations
	Load_Animation_BOB(&plasma[0], PLASMA_ANIM_PLAYER, 8, plasma_anim_player);
	Load_Animation_BOB(&plasma[0], PLASMA_ANIM_ENEMY, 8, plasma_anim_enemy);

	// set state to off
	plasma[0].state = PLASMA_STATE_OFF;

	for (int pulse = 1; pulse < MAX_PLASMA; pulse++) {
		Clone_BOB(&plasma[0], &plasma[pulse]);
	} // end for pulse

	// unload data infile
	Unload_Bitmap_File(&bitmap8bit);

} // end Init_Plasma

/////////////////////////////////////////////////////////

void Reset_Plasma(void)
{
	// this function resets all the plasma pulses
	for (int pulse = 0; pulse < MAX_PLASMA; pulse++) {
		plasma[pulse].state = PLASMA_STATE_OFF;
	} // end for pulse

} // end Reset_Plasma

///////////////////////////////////////////////////////////

void Delete_Plasma(void)
{
	// this function simply deletes all memory and surfaces
	// related to the plasma pulses

	for (int index = 0; index < MAX_PLASMA; index++)
		Destroy_BOB(&plasma[index]);

} // end Delete_Plasma

///////////////////////////////////////////////////////////

void Move_Plasma(void)
{
	// this function moves all the plasma pulses and checks for
	// collision with the rocks

	for (int index = 0; index < MAX_PLASMA; index++) {
		// test if plasma pulse is in flight
		if (plasma[index].state == PLASMA_STATE_ON) {
			// move the pulse 
			plasma[index].varsI[INDEX_WORLD_X] += plasma[index].xv;
			plasma[index].varsI[INDEX_WORLD_Y] += plasma[index].yv;

			// test for boundaries
			if ((++plasma[index].counter_1 > PLASMA_RANGE_1) ||
				(plasma[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X) ||
				(plasma[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X) ||
				(plasma[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y) ||
				(plasma[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y)) {
				// kill the pulse
				plasma[index].state = PLASMA_STATE_OFF;
				plasma[index].counter_1 = 0;

				// move to next pulse
				continue;
			} // end if

			// test for mines
			for (int mine = 0; mine < MAX_MINES; mine++) {
				if (mines[mine].state == MINE_STATE_ALIVE &&
					plasma[index].anim_state == PLASMA_ANIM_PLAYER) {
					// test for collision 
					if (Collision_Test(plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].width * .5),
						plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].height * .5),
						plasma[index].width, plasma[index].height,
						mines[mine].varsI[INDEX_WORLD_X] - (mines[mine].width * .5),
						mines[mine].varsI[INDEX_WORLD_Y] - (mines[mine].height * .5),
						mines[mine].width, mines[mine].height)) {
						// kill pulse
						plasma[index].state = PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						// do damage to mine
						mines[mine].varsI[INDEX_MINE_DAMAGE] += (1 + rand() % 3);

						// increase the damage on the mine and test for death
						if (mines[mine].varsI[INDEX_MINE_DAMAGE] > MAX_MINE_DAMAGE) {
							// kill the ship    
							mines[mine].state = MINE_STATE_DEAD;

							// start a new mine
							Start_Mine();

							// add to players score
							player_score += 250;

						} // end if

						int width = 30 + rand() % 40;

						// start a burst
						Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
							plasma[index].varsI[INDEX_WORLD_Y],
							width, (width * .5) + (width * .25),
							int(mines[mine].xv) * .5, int(mines[mine].yv) * .5);
						break;
					} // end if

				} // end if alive

			} // end for mines

	   ///////////////////////////////////////////////////////

	   // test for stations
			for (int station = 0; station < MAX_STATIONS; station++) {
				if (stations[station].state == STATION_STATE_ALIVE &&
					plasma[index].anim_state == PLASMA_ANIM_PLAYER) {
					// test for collision 
					if ( //stations[station].anim_state == STATION_SHIELDS_ANIM_OFF &&
						Collision_Test(plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].width * .5),
							plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].height * .5),
							plasma[index].width, plasma[index].height,
							stations[station].varsI[INDEX_WORLD_X] - (stations[station].width * .5),
							stations[station].varsI[INDEX_WORLD_Y] - (stations[station].height * .5),
							stations[station].width, stations[station].height)) {
						// kill pulse
						plasma[index].state = PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						// do damage to station
						stations[station].varsI[INDEX_STATION_DAMAGE] += (1 + rand() % 3);

						// increase the damage on the station and test for death
						if (stations[station].varsI[INDEX_STATION_DAMAGE] > MAX_STATION_DAMAGE) {
							// kill the station
							stations[station].state = STATION_STATE_DEAD;

							// add to players score
							player_score += 10000;

							// has played won
							if (++num_stations_destroyed >= NUM_ACTIVE_STATIONS) {
								win_counter = 0;
								player_won = 1;
							} // end if

						} // end if

						// start a burst
						Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
							plasma[index].varsI[INDEX_WORLD_Y],
							40 + rand() % 20, 30 + rand() % 16,
							int(stations[station].xv) * .5, int(stations[station].yv) * .5);
						break;
					} // end if

				} // end if alive

			} // end for stations

			///////////////////////////////////////////////////////

			// test for collision with player
			if (plasma[index].anim_state == PLASMA_ANIM_ENEMY && player_state == PLAYER_STATE_ALIVE &&
				Collision_Test(plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].width * .5),
					plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].height * .5),
					plasma[index].width, plasma[index].height,
					player_x - (wraith.width * .5),
					player_y - (wraith.height * .5),
					wraith.width, wraith.height)) {

				Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
					plasma[index].varsI[INDEX_WORLD_Y],
					40 + rand() % 20, 30 + rand() % 16,
					int(player_xv) * .5, int(player_yv) * .5);

				// update players damage
				player_damage += 2;

				// engage shields
				player_shield_count = 3;

				// kill the pulse
				plasma[index].state = PLASMA_STATE_OFF;
				plasma[index].counter_1 = 0;

				// plasma is dead
				continue;
			} // end if

			////////////////////////////////////////////////////////

			// test for collision with rocks
			for (int rock = 0; rock < MAX_ROCKS; rock++) {
				if (rocks[rock].state == ROCK_STATE_ON) {
					// test for collision 
					if (Collision_Test(plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].width * .5),
						plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].height * .5),
						plasma[index].width, plasma[index].height,
						rocks[rock].varsI[INDEX_WORLD_X] - (rocks[rock].width * .5),
						rocks[rock].varsI[INDEX_WORLD_Y] - (rocks[rock].height * .5),
						rocks[rock].width, rocks[rock].height)) {
						// kill pulse
						plasma[index].state = PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						switch (rocks[rock].varsI[0]) {
						case ROCK_LARGE:
						{
							// start explosion
							Start_Burst(plasma[index].varsI[INDEX_WORLD_X], plasma[index].varsI[INDEX_WORLD_Y],
								68 + rand() % 12, 54 + rand() % 10,
								rocks[rock].xv * .5, rocks[rock].yv * .5);

						} break;

						case ROCK_MEDIUM:
						{
							// start explosion
							Start_Burst(plasma[index].varsI[INDEX_WORLD_X], plasma[index].varsI[INDEX_WORLD_Y],
								52 + rand() % 10, 44 + rand() % 8,
								rocks[rock].xv * .5, rocks[rock].yv * .5);

						} break;

						case ROCK_SMALL:
						{

							// start explosion
							Start_Burst(plasma[index].varsI[INDEX_WORLD_X], plasma[index].varsI[INDEX_WORLD_Y],
								34 - 4 + rand() % 8, 30 - 3 + rand() % 6,
								rocks[rock].xv * .5, rocks[rock].yv * .5);

						} break;

						} // end switch

						// update score
						player_score += rocks[rock].varsI[2];

						// test strength of rock, cause damage
						rocks[rock].varsI[2] -= 50;

						// split test
						if (rocks[rock].varsI[2] > 0 && rocks[rock].varsI[2] < 50) {
							// test the size of rock
							switch (rocks[rock].varsI[0]) {
							case ROCK_LARGE:
							{
								// split into two medium
								Start_Rock(rocks[rock].varsI[INDEX_WORLD_X] + rand() % 16, rocks[rock].varsI[INDEX_WORLD_Y] + rand() % 16,
									ROCK_MEDIUM,
									rocks[rock].xv - 2 + rand() % 4, rocks[rock].yv - 2 + rand() % 4);

								Start_Rock(rocks[rock].varsI[INDEX_WORLD_X] + rand() % 16, rocks[rock].varsI[INDEX_WORLD_Y] + rand() % 16,
									ROCK_MEDIUM,
									rocks[rock].xv - 2 + rand() % 4, rocks[rock].yv - 2 + rand() % 4);

								// throw in a small?
								if ((rand() % 3) == 1)
									Start_Rock(rocks[rock].varsI[INDEX_WORLD_X] + rand() % 16, rocks[rock].varsI[INDEX_WORLD_Y] + rand() % 16,
										ROCK_SMALL,
										rocks[rock].xv - 2 + rand() % 4, rocks[rock].yv - 2 + rand() % 4);

								// kill the original
								rocks[rock].state = ROCK_STATE_OFF;

							} break;

							case ROCK_MEDIUM:
							{
								// split into 1 - 3 small
								int num_rocks = 1 + rand() % 3;

								for (; num_rocks >= 1; num_rocks--) {
									Start_Rock(rocks[rock].varsI[INDEX_WORLD_X] + rand() % 8, rocks[rock].varsI[INDEX_WORLD_Y] + rand() % 8,
										ROCK_SMALL,
										rocks[rock].xv - 2 + rand() % 4, rocks[rock].yv - 2 + rand() % 4);

								} // end for num_rocks

							// kill the original
								rocks[rock].state = ROCK_STATE_OFF;

							} break;

							case ROCK_SMALL:
							{
								// just kill it
								rocks[rock].state = ROCK_STATE_OFF;

							} break;

							default:break;


							} // end switch

						} // end if split
						else
							if (rocks[rock].varsI[2] <= 0) {
								// kill rock
								rocks[rock].state = ROCK_STATE_OFF;
							} // end else

						// break out of loop
						break;

					} // end if collision

				} // end if rock alive

			} // end for rock

		} // end if

	} // end for index

} // end Move_Plasma

///////////////////////////////////////////////////////////

void Draw_Plasma(void)
{
	// this function draws all the plasma pulses

	for (int index = 0; index < MAX_PLASMA; index++) {
		// test if plasma pulse is in flight
		if (plasma[index].state == PLASMA_STATE_ON) {
			// transform to screen coords
			plasma[index].x = plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].width >> 1) - player_x + (screen_width / 2);
			plasma[index].y = plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].height >> 1) - player_y + (screen_height / 2);

			// draw the pulse
			Draw_BOB(&plasma[index], framebuffer, framebuffer_pitch);

			// animate the pulse
			Animate_BOB(&plasma[index]);

		} // end if

	} // end for index

} // end Draw_Plasma

///////////////////////////////////////////////////////////

void Fire_Plasma(int x, int y, int xv, int yv, int source = PLASMA_ANIM_PLAYER)
{
	// this function fires a plasma pulse at the given starting
	// position and velocity, of course, one must be free for 
	// this to work

	// scan for a pulse that is available
	for (int index = 0; index < MAX_PLASMA; index++) {
		// is this one available
		  // test if plasma pulse is in flight
		if (plasma[index].state == PLASMA_STATE_OFF) {
			// start this one up, note the use of world coords
			plasma[index].varsI[INDEX_WORLD_X] = x - (plasma[0].width * .5);
			plasma[index].varsI[INDEX_WORLD_Y] = y - (plasma[0].height * .5);

			plasma[index].xv = xv;
			plasma[index].yv = yv;
			plasma[index].curr_frame = 0;
			plasma[index].state = PLASMA_STATE_ON;
			plasma[index].counter_1 = 0; // used to track distance

			// set animation and typing info for collision engine
			plasma[index].anim_state = source;
			Set_Animation_BOB(&plasma[index], source);

			// later
			return;

		} // end if

	} // end for

} // end Fire_Plasma

///////////////////////////////////////////////////////////

void Init_Bursts(void)
{
	// this function initializes and loads all the bursts 

	// load the bursts imagery 
	Load_Bitmap_File(&bitmap8bit, "assets/expl8.bmp");

	// create the first bob
	Create_BOB(&bursts[0], 0, 0, 42, 36, 14,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		0);

	// load animation frames
	for (int frame = 0; frame < 14; frame++)
		Load_Frame_BOB(&bursts[0], &bitmap8bit, frame, frame % 6, frame / 6, BITMAP_EXTRACT_MODE_CELL);

	// set animation rate
	Set_Anim_Speed_BOB(&bursts[0], 1);

	// set size of burst
	bursts[0].varsI[0] = bursts[0].width;
	bursts[0].varsI[1] = bursts[0].height;

	// set state to off
	bursts[0].state = BURST_STATE_OFF;

	for (int burst = 1; burst < MAX_BURSTS; burst++) {
		Clone_BOB(&bursts[0], &bursts[burst]);
	} // end for burst

	// unload data infile
	Unload_Bitmap_File(&bitmap8bit);

} // end Init_Bursts

///////////////////////////////////////////////////////////

void Reset_Bursts(void)
{
	// this function resets all the bursts

	for (int burst = 0; burst < MAX_BURSTS; burst++) {
		// set state to off
		bursts[burst].state = BURST_STATE_OFF;
	} // end for burst

} // end Reset_Bursts

////////////////////////////////////////////////////////////

void Delete_Bursts(void)
{
	// this function simply deletes all memory and surfaces
	// related to the bursts pulses

	for (int index = 0; index < MAX_BURSTS; index++)
		Destroy_BOB(&bursts[index]);

} // end Delete_Bursts

///////////////////////////////////////////////////////////

void Move_Bursts(void)
{
	// this function moves all the bursts 

	for (int index = 0; index < MAX_BURSTS; index++) {
		// test if bursts pulse is in moving
		if (bursts[index].state == BURST_STATE_ON) {
			// move the burst
			// Move_BOB(&bursts[index]);
			bursts[index].varsI[INDEX_WORLD_X] += bursts[index].xv;
			bursts[index].varsI[INDEX_WORLD_Y] += bursts[index].yv;

			// test if burst is off screen or done with animation
			if (bursts[index].curr_frame >= bursts[index].num_frames - 1) {
				// kill burst and put back on available list
				bursts[index].state = BURST_STATE_OFF;
			} // end if

		} // end if

	} // end for index

} // end Move_Bursts

///////////////////////////////////////////////////////////

void Draw_Bursts(void)
{
	// this function draws all the bursts 
	for (int index = 0; index < MAX_BURSTS; index++) {
		// test if bursts pulse is in flight
		if (bursts[index].state == BURST_STATE_ON) {
			// transform bursts to screen space
			bursts[index].x = bursts[index].varsI[INDEX_WORLD_X] - (bursts[index].width * .5) - player_x + (screen_width / 2);
			bursts[index].y = bursts[index].varsI[INDEX_WORLD_Y] - (bursts[index].height * .5) - player_y + (screen_height / 2);

			// is scaling needed
			if (bursts[index].varsI[0] != bursts[index].width ||
				bursts[index].varsI[1] != bursts[index].height) {
				// draw the burst scaled
				Draw_Scaled_BOB(&bursts[index],
					bursts[index].varsI[0], bursts[index].varsI[1],
					framebuffer, framebuffer_pitch);
			} else // draw normal
				Draw_BOB(&bursts[index], framebuffer, framebuffer_pitch);

			// animate the explosion
			Animate_BOB(&bursts[index]);

		} // end if

	} // end for index

} // end Draw_Bursts

///////////////////////////////////////////////////////////

void Draw_Info(void)
{
	// this function draws all the information at the top of the screen

	char score[16]; // hold score
	static int red_glow = 0; // used for damage display

	// build up scrore string
	sprintf(score, "0000000%d", player_score);

	// build up final string
	sprintf(buffer, "SCORE %s", &score[strlen(score) - 8]);
	Draw_Text_GDI(buffer, 10, 10, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	// draw damage
	sprintf(buffer, "DAMAGE %d%%", player_damage);

	if (player_damage < 90)
		Draw_Text_GDI(buffer, 350 - 8 * strlen(buffer), 10, RGB(0, 255, 0), framebuffer, framebuffer_pitch);
	else
		Draw_Text_GDI(buffer, 350 - 8 * strlen(buffer), 10, RGB(red_glow, 0, 0), framebuffer, framebuffer_pitch);

	// update red glow
	if ((red_glow += 15) > 255)
		red_glow = 0;

	// draw ships
	sprintf(buffer, "SHIPS %d", player_ships);
	Draw_Text_GDI(buffer, 520, 10, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "VEL %.2f Kps", vel * (100 / MAX_PLAYER_SPEED));
	Draw_Text_GDI(buffer, 10, 460, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "POS [%.2f,%.2f]", player_x, player_y);
	Draw_Text_GDI(buffer, 480, 460, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

	sprintf(buffer, "TRACKING RATE = %.2f", mine_tracking_rate);
	Draw_Text_GDI(buffer, 320 - 120, 460, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

} // end Draw_Info

///////////////////////////////////////////////////////////

int Copy_Screen(UCHAR *source_bitmap, UCHAR *dest_buffer, int lpitch, int transparent)
{
	// this function draws the bitmap onto the destination memory surface
	// if transparent is 1 then color 0 will be transparent
	// note this function does NOT clip, so be carefull!!!

	UCHAR *dest_addr,   // starting address of bitmap in destination
		*source_addr; // starting adddress of bitmap data in source

	UCHAR pixel;        // used to hold pixel value

	int index,          // looping vars
		pixel_x;

	// test if this bitmap is loaded

	   // compute starting destination address
	dest_addr = dest_buffer;

	// compute the starting source address
	source_addr = source_bitmap;

	// is this bitmap transparent
	if (transparent) {
		// copy each line of bitmap into destination with transparency
		for (index = 0; index < screen_height; index++) {
			// copy the memory
			for (pixel_x = 0; pixel_x < screen_width; pixel_x++) {
				if ((pixel = source_addr[pixel_x]) != 0)
					dest_addr[pixel_x] = pixel;

			} // end if

			// advance all the pointers
			dest_addr += lpitch;
			source_addr += screen_width;

		} // end for index
	} // end if
	else {
		// non-transparent version
		// copy each line of bitmap into destination

		for (index = 0; index < screen_height; index++) {
			// copy the memory
			memcpy(dest_addr, source_addr, screen_width);

			// advance all the pointers
			dest_addr += lpitch;
			source_addr += screen_width;

		} // end for index

	} // end else

	// return success
	return(1);

} // end Copy_Screen

/////////////////////////////////////////////////////////////////////

void Do_Intro(void)
{
	// the worlds simples intro

	// draw in logo screen
	Load_Bitmap_File(&bitmap8bit, "assets/trackintro.bmp");

	// set the palette to background image palette
	Set_Palette(bitmap8bit.palette);

	// copy the bitmap to primary buffer
	Copy_Screen(bitmap8bit.buffer,framebuffer, framebuffer_pitch, 0);

	// unload bitmap file
	Unload_Bitmap_File(&bitmap8bit);

	// transition to black
	Screen_Transitions(SCREEN_DARKNESS, NULL, 0);

} // end Do_Intro

///////////////////////////////////////////////////////////

void Load_HUD(void)
{
	// this function loads the animation for the hud

	int index; // looping variable

	// load the wraith ship
	Load_Bitmap_File(&bitmap8bit, "assets/hudart8.bmp");

	// now create the wraith
	Create_BOB(&hud, 0, 0, 24, 26, 4,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		0);

	// load hud frames
	for (index = 0; index < 4; index++)
		Load_Frame_BOB(&hud, &bitmap8bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// unload data infile
	Unload_Bitmap_File(&bitmap8bit);

} // end Load_HUD

///////////////////////////////////////////////////////////////////////////////

int Load_Player(void)
{
	// this function loads the animation for the player

	int index; // looping variable

	// load the wraith ship
	Load_Bitmap_File(&bitmap8bit, "assets/wraithb8.bmp");

	// set the palette to background image palette
	Set_Palette(bitmap8bit.palette);

	// save this palette
	Save_Palette(game_palette);

	// now create the wraith
	Create_BOB(&wraith, 0, 0, 64, 64, 35,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		0);

	// load wraith frames
	for (index = 0; index < 35; index++)
		Load_Frame_BOB(&wraith, &bitmap8bit, index, index % 8, index / 8, BITMAP_EXTRACT_MODE_CELL);

	// set position
	Set_Pos_BOB(&wraith, (screen_width / 2) - wraith.width / 2, (screen_height / 2) - wraith.height / 2);

	// set starting direction
	wraith.varsI[WRAITH_INDEX_DIR] = 0;

	// unload data infile
	Unload_Bitmap_File(&bitmap8bit);

	// return success
	return(1);

} // end Load_Player

////////////////////////////////////////////////////////////////////

void Create_Tables(void)
{
	// this function creates all the lookup tables for the game

	int ang = 0; // looping var to track angle

	// create the 16 sector sin, cos lookup
	for (ang = 0; ang < 16; ang++) {
		float fang = PI * (ang * 22.5) / 180;

		cos_look16[ang] = -cos(fang + PI / 2);
		sin_look16[ang] = -sin(fang + PI / 2);

	} // end for ang

} // end Create_Tables

/////////////////////////////////////////////////////////////////////////////////////

float Fast_Distance_2D(float x, float y)
{
	// this function computes the distance from 0,0 to x,y with 3.5% error

	// first compute the absolute value of x,y
	x = fabs(x);
	y = fabs(y);

	// compute the minimum of x,y
	float mn = MIN(x, y);

	// return the distance
	return(x + y - (mn / 2) - (mn / 4) + (mn / 8));

} // end Fast_Distance_2D

///////////////////////////////////////////////////////////

void Game_Reset(void)
{
	// this function resets all the game related systems and vars for another run

	int index; // looping var

	// player state variables
	player_state = PLAYER_STATE_ALIVE;
	player_score = 0;   // the score
	player_ships = 3;   // ships left
	player_regen_counter = 0; // used to track when to regen
	player_damage = 0;   // damage of player
	player_counter = 0;   // used for state transition tracking
	player_regen_count = 0;   // used to regenerate player
	player_shield_count = 0;  // used to display shields
	win_counter = 0;
	player_won = 0;
	num_stations_destroyed = 0; // number of stations destroyed thus far

	// menu_up = 0;             // flags if the main menu is up
	// intro_done = 0;          // flags if intro has played already

	// positional and state info for player
	player_x = 0;
	player_y = 0;
	player_dx = 0;
	player_dy = 0;
	player_xv = 0;
	player_yv = 0;
	vel = 0;
	wraith.varsI[WRAITH_INDEX_DIR] = 0;
	game_state = GAME_STATE_RESTART;   // initial game state
	station_id = -1;

	ready_counter = 0,        // used to draw a little "get ready"
		ready_state = 0;

	// call all the reset funcs
	Init_Reset_Particles();

	Reset_Mines();
	Reset_Rocks();
	Reset_Plasma();
	Reset_Bursts();
	Reset_Stations();

	// start the rocks
	Seed_Rocks();

	// start the mines
	for (index = 0; index < NUM_ACTIVE_MINES; index++)
		Start_Mine();

	// start the stations
	for (index = 0; index < NUM_ACTIVE_STATIONS; index++)
		Start_Station();

} // end Game_Reset

////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	float xv = 0, yv = 0;            // used to compute velocity 
	static int engines_on = 0;   // tracks state of engines each frame 
	static int andre_up = 0;

	// what state is the main loop in?
	if (game_state == GAME_STATE_INIT) {
		// reset everything

		// transition into menu state
		// game_state = GAME_STATE_MENU;

		// transition into run state
		game_state = GAME_STATE_RESTART;
	} // end game_state is game init
	else
		if (game_state == GAME_STATE_MENU) {

			game_state = GAME_STATE_RESTART;

		} // end if in menu state
		else
			if (game_state == GAME_STATE_RESTART) {

				// restart the game
				Game_Reset();

				// set to run state
				game_state = GAME_STATE_RUNNING;

			} // end if restart
			else
				if (game_state == GAME_STATE_RUNNING) {

					// start the timing clock
					Start_Clock();

					// only process player if alive
					if (player_state == PLAYER_STATE_ALIVE) {

						// test for tracking rate change
						if (RETRO_KeyState(SDL_SCANCODE_COMMA)) {
							if ((mine_tracking_rate += 0.1) > 4.0)
								mine_tracking_rate = 4.0;
						} // end if
						else
							if (RETRO_KeyState(SDL_SCANCODE_PERIOD)) {
								if ((mine_tracking_rate -= 0.1) < 0)
									mine_tracking_rate = 0;
							} // end if

						// test if player is moving
						if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
							// rotate player to right
							if (++wraith.varsI[WRAITH_INDEX_DIR] > 15)
								wraith.varsI[WRAITH_INDEX_DIR] = 0;

						} // end if
						else
							if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
								// rotate player to left
								if (--wraith.varsI[WRAITH_INDEX_DIR] < 0)
									wraith.varsI[WRAITH_INDEX_DIR] = 15;

							} // end if

						// vertical/speed motion
						if (RETRO_KeyState(SDL_SCANCODE_UP)) {
							// move player forward
							xv = cos_look16[wraith.varsI[WRAITH_INDEX_DIR]];
							yv = sin_look16[wraith.varsI[WRAITH_INDEX_DIR]];

							// set engines to on
							engines_on = 1;

							Start_Particle(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 3,
								player_x + RAND_RANGE(-2, 2), player_y + RAND_RANGE(-2, 2), (-int(player_xv) * .125), (-int(player_yv) * .125));

						} // end if
						else
							if (engines_on) {
								// reset the engine on flag and turn off sound
								engines_on = 0;

							} // end if

						// toggle huds
						if (RETRO_KeyPressed(SDL_SCANCODE_H) && !huds_debounce) {
							huds_debounce = 1;
							huds_on = (huds_on) ? 0 : 1;

						} // end if

						if (!RETRO_KeyPressed(SDL_SCANCODE_H))
							huds_debounce = 0;

						// toggle scanner
						if (RETRO_KeyPressed(SDL_SCANCODE_S) && !scanner_debounce) {
							scanner_debounce = 1;
							scanner_on = (scanner_on) ? 0 : 1;

						} // end if

						if (!RETRO_KeyPressed(SDL_SCANCODE_S))
							scanner_debounce = 0;

						// add velocity change to player's velocity
						player_xv += xv;
						player_yv += yv;

						// test for maximum velocity
						vel = Fast_Distance_2D(player_xv, player_yv);

						if (vel >= MAX_PLAYER_SPEED) {
							// re-compute velocity vector by normalizing then re-scaling
							player_xv = (MAX_PLAYER_SPEED - 1) * player_xv / vel;
							player_yv = (MAX_PLAYER_SPEED - 1) * player_yv / vel;
						} // end if

						// add in frictional coefficient

						// move player, note that these are in world coords
						player_x += player_xv;
						player_y += player_yv;

						// wrap player to universe
						if (player_x > UNIVERSE_MAX_X) {
							// reset pos
							player_x = UNIVERSE_MIN_X;

							// set warp flag
						} // end if
						else
							if (player_x < UNIVERSE_MIN_X) {
								// reset pos
								player_x = UNIVERSE_MAX_X;

								// set warp flag
							} // end if

						 // y coords
						if (player_y > UNIVERSE_MAX_Y) {
							// reset pos
							player_y = UNIVERSE_MIN_Y;

							// set warp flag
						} // end if
						else
							if (player_y < UNIVERSE_MIN_Y) {
								// reset pos
								player_y = UNIVERSE_MAX_Y;

								// set warp flag
							} // end if

						// test if player is firing
						if (RETRO_KeyPressed(SDL_SCANCODE_RETURN)) {
							// compute plasma velocity vector
							float plasma_xv = cos_look16[wraith.varsI[WRAITH_INDEX_DIR]] * PLASMA_SPEED;
							float plasma_yv = sin_look16[wraith.varsI[WRAITH_INDEX_DIR]] * PLASMA_SPEED;

							// fire the plasma taking into consideration ships's velocity
							Fire_Plasma(player_x, player_y,
								player_xv + plasma_xv,
								player_yv + plasma_yv, PLASMA_ANIM_PLAYER);

						} // end if

						// do bounds check

						// regenerate player
						if (++player_regen_counter >= PLAYER_REGEN_COUNT) {
							// regenerate the player a bit
							if (player_damage > 0)
								player_damage--;

							// reset counter
							player_regen_counter = 0;

						} // end if

						// test for dying state transition
						if (player_damage >= 100 && player_state == PLAYER_STATE_ALIVE) {
							// set damage to 100
							player_damage = 100;

							// kill player
							player_state = PLAYER_STATE_DYING;
							player_ships--;

							// set counter to 0
							player_counter = 0;

							// turn engines off
							engines_on = 0;

						} // end if

					} // end if player alive
					else
						if (player_state == PLAYER_STATE_DYING) {
							// player is dying

							// start random bursts
							int bwidth = 16 + rand() % 64;

							if ((rand() % 4) == 1)
								Start_Burst(player_x + rand() % 40, player_y + rand() % 8,
									bwidth, (bwidth >> 2) + (bwidth >> 1),
									-4 + rand() % 8, 2 + rand() % 4);

							// get jiggie with it
							wraith.varsI[WRAITH_INDEX_DIR] += (RAND_RANGE(-1, 1));

							if (wraith.varsI[WRAITH_INDEX_DIR] > 15)
								wraith.varsI[WRAITH_INDEX_DIR] = 0;
							else
								if (wraith.varsI[WRAITH_INDEX_DIR] < 0)
									wraith.varsI[WRAITH_INDEX_DIR] = 15;

							// update state counter
							if (++player_counter > 150) {
								// set state to invincible  
								player_state = PLAYER_STATE_INVINCIBLE;

								// reset counter
								player_counter = 0;

								// reset damage
								player_damage = 0;
							} // end if

						} // end if
						else
							if (player_state == PLAYER_STATE_INVINCIBLE) {
								// player is invincible

								// flicker shields and play energizing sound

								// update state counter
								if (++player_counter > 70) {
									// set state to invincible  
									player_state = PLAYER_STATE_ALIVE;

									// reset counter
									player_counter = 0;

									// reset player position
									player_x = 0;
									player_y = 0;
									player_dx = 0;
									player_dy = 0;
									player_xv = 0;
									player_yv = 0;
									wraith.varsI[WRAITH_INDEX_DIR] = 0;

									// decrease player ships
									if (player_ships == 0) {
										// change state to dead, reset all vars
										player_state = PLAYER_STATE_DEAD;
										player_counter = 0;
										ready_state = 1;
										ready_counter = 0;

									} // end if       

								} // end if

								// start ready again
								if ((player_counter == 20) && (player_ships > 0)) {
									// reset to ready
									ready_counter = 0,
										ready_state = 0;
								} // end if

							} // end if

					// end player sub states

					// move the stations
					Move_Stations();

					// move the mines
					Move_Mines();

					// move the asteroids
					Move_Rocks();

					// move the stars
					Move_Stars();

					// move the particles
					Move_Particles();

					// move the explosions
					Move_Bursts();

					// draw the stars
					Draw_Stars();

					// draw the stations
					Draw_Stations();

					// draw the tocks
					Draw_Rocks();

					// draw the mines
					Draw_Mines();

					// draw the particles
					Draw_Particles();

					// draw the plasma
					Draw_Plasma();

					// move the plasma
					Move_Plasma();

					// draw the player if alive
					if (player_state == PLAYER_STATE_ALIVE || player_state == PLAYER_STATE_DYING) {
						// set the current frame
						if (engines_on)
							wraith.curr_frame = wraith.varsI[WRAITH_INDEX_DIR] + 16 * (rand() % 2);
						else
							wraith.curr_frame = wraith.varsI[WRAITH_INDEX_DIR];

						// draw the bob
						Draw_BOB(&wraith, framebuffer, framebuffer_pitch);

						// draw the shields
						if (player_shield_count > 0) {
							player_shield_count--;

							// select shield frame
							wraith.curr_frame = 34 - player_shield_count;

							// draw the bob
							Draw_BOB(&wraith, framebuffer, framebuffer_pitch);

						} // end if

					} // end if

					// draw explosions last
					Draw_Bursts();

					// draw the score and ships left
					if (huds_on)
						Draw_Info();

					if (scanner_on)
						Draw_Scanner();

					// last state here so it will overlay
					if (player_state == PLAYER_STATE_DEAD) {
						// player is dead
						ready_state = 1;
						ready_counter = 0;

						// player is done!
						Draw_Text_GDI("G A M E   O V E R", (screen_width / 2) - 8 * (strlen("G A M E   O V E R") >> 1), screen_height / 2, RGB(0, 255, 0), framebuffer, framebuffer_pitch);

					} // end if

				 // draw get ready?
					if (ready_state == 0) {

						// draw text
						Draw_Text_GDI("E N T E R I N G   S E C T O R - ALPHA 11", 320 - 8 * strlen("E N T E R I N G   S E C T O R!") / 2, 200, RGB(0, rand() % 255, 0), framebuffer, framebuffer_pitch);

						// increment counter
						if (++ready_counter > 60) {
							// set state to ready
							ready_state = 1;
							ready_counter = 0;

						} // end if

					} // end if

					// test for me!
					if (RETRO_KeyPressed(SDL_SCANCODE_A)) {
						Draw_Bitmap(&andre, framebuffer, framebuffer_pitch, 0);
						andre_up = 1;
					} // end if
					else {
						andre_up = 0;
						Set_Palette(game_palette);
					} // end else

					if (andre_up == 1) {
						Set_Palette(andre_palette);
						andre_up = 2;
					} // end if

					// sync to 30ish fps
					Wait_Clock(30);

					// check of user is trying to exit
					if (++player_counter > 10)
						if (RETRO_KeyPressed(SDL_SCANCODE_BACKSPACE)) {
							// send game back to menu state
							game_state = GAME_STATE_EXIT;

						} // end if

				} // end if game running
				else
					if (game_state == GAME_STATE_PAUSED) {
						// pause game
						if (RETRO_KeyPressed(SDL_SCANCODE_P) && !pause_debounce) {
							pause_debounce = 1;
							game_paused = (game_paused) ? 0 : 1;

						} // end if

						if (!RETRO_KeyPressed(SDL_SCANCODE_P))
							game_paused = 0;

						if (game_paused) {
							// draw text
							Draw_Text_GDI("G A M E  P A U S E D  -- P R E S S  <P>", 320 - 8 * strlen("G A M E  P A U S E D  -- P R E S S  <P>") / 2, 200, RGB(255, 0, 0), framebuffer, framebuffer_pitch);
						} // end if
						else
							game_state = GAME_STATE_RUNNING;

					} // end if
					else
						if (game_state == GAME_STATE_EXIT) {
							game_state = GAME_STATE_WAITING_FOR_EXIT;

						} // end if
						else
							if (game_state == GAME_STATE_WAITING_FOR_EXIT) {
								// wait here in safe state

							} // end wait

						 // return success
}

///////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// seed random number generate
	srand(Start_Clock());

	// create all lookup tables
	Create_Tables();

	// initialize the plasma pulses
	Init_Plasma();

	// initialize all the stars
	Init_Stars();

	// init rocks
	Init_Rocks();
	Seed_Rocks();

	// init all the explosions
	Init_Bursts();

	// initialize all the Mines
	Init_Mines();

	// initialize all the stations, must be after mines!
	Init_Stations();

	// intialize particle system
	Init_Reset_Particles();

	Load_Andre();

	// do the introdcution
	Do_Intro();

	// load the player
	Load_Player();

	// load the hud art
	Load_HUD();

	// create lookup for lighting engine
	RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
		palette,             // source palette
		rgblookup);          // lookup table
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// delete all the explosions
	Delete_Bursts();

	// delete the player
	Destroy_BOB(&wraith);

	// delete all the rocks
	Delete_Rocks();

	// delete all the plasma pulses
	Delete_Plasma();

	// delete all the stations
	Delete_Stations();

	// delete the mines
	Delete_Mines();
}

///////////////////////////////////////////////////////////
