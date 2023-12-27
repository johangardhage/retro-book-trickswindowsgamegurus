//
// FREAKOUT.CPP - break game demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////////

// states for game loop
#define GAME_STATE_INIT         0
#define GAME_STATE_START_LEVEL  1
#define GAME_STATE_RUN          2
#define GAME_STATE_SHUTDOWN     3
#define GAME_STATE_EXIT         4 

// block defines
#define NUM_BLOCK_ROWS          6
#define NUM_BLOCK_COLUMNS       8

#define BLOCK_WIDTH             64
#define BLOCK_HEIGHT            16
#define BLOCK_ORIGIN_X          8
#define BLOCK_ORIGIN_Y          8
#define BLOCK_X_GAP             80
#define BLOCK_Y_GAP             32

// paddle defines
#define PADDLE_START_X          (screen_width/2 - 16)
#define PADDLE_START_Y          (screen_height - 32);
#define PADDLE_WIDTH            32
#define PADDLE_HEIGHT           8
#define PADDLE_COLOR            191

// ball defines
#define BALL_START_Y            (screen_height/2)
#define BALL_SIZE                4

// GLOBALS ////////////////////////////////////////////////////

int game_state = GAME_STATE_INIT; // starting state

int paddle_x = 0, paddle_y = 0; // tracks position of paddle
int ball_x = 0, ball_y = 0; // tracks position of ball
int ball_dx = 0, ball_dy = 0; // velocity of ball
int score = 0;               // the score
int level = 1;               // the current level
int blocks_hit = 0;             // tracks number of blocks hit

// this contains the game grid data   

UCHAR blocks[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS];

// FUNCTIONS //////////////////////////////////////////////////

void Init_Blocks(void)
{
	// initialize the block field
	for (int row = 0; row < NUM_BLOCK_ROWS; row++)
		for (int col = 0; col < NUM_BLOCK_COLUMNS; col++)
			blocks[row][col] = row * 16 + col * 3 + 16;

} // end Init_Blocks

///////////////////////////////////////////////////////////////

void Draw_Blocks(void)
{
	// this function draws all the blocks in row major form
	int x1 = BLOCK_ORIGIN_X, // used to track current position
		y1 = BLOCK_ORIGIN_Y;

	// draw all the blocks
	for (int row = 0; row < NUM_BLOCK_ROWS; row++) {
		// reset column position
		x1 = BLOCK_ORIGIN_X;

		// draw this row of blocks
		for (int col = 0; col < NUM_BLOCK_COLUMNS; col++) {
			// draw next block (if there is one)
			if (blocks[row][col] != 0) {
				// draw block     
				Draw_Rectangle(x1 - 4, y1 + 4,
					x1 + BLOCK_WIDTH - 4, y1 + BLOCK_HEIGHT + 4, 0, framebuffer, framebuffer_pitch);

				Draw_Rectangle(x1, y1, x1 + BLOCK_WIDTH,
					y1 + BLOCK_HEIGHT, blocks[row][col], framebuffer, framebuffer_pitch);
			} // end if

			// advance column position
			x1 += BLOCK_X_GAP;
		} // end for col

		// advance to next row position
		y1 += BLOCK_Y_GAP;

	} // end for row

} // end Draw_Blocks

///////////////////////////////////////////////////////////////

void Process_Ball(void)
{
	// this function tests if the ball has hit a block or the paddle
	// if so, the ball is bounced and the block is removed from 
	// the playfield note: very cheesy collision algorithm :)

	// first test for ball block collisions

	// the algorithm basically tests the ball against each 
	// block's bounding box this is inefficient, but easy to 
	// implement, later we'll see a better way

	int x1 = BLOCK_ORIGIN_X, // current rendering position
		y1 = BLOCK_ORIGIN_Y;

	int ball_cx = ball_x + (BALL_SIZE / 2),  // computer center of ball
		ball_cy = ball_y + (BALL_SIZE / 2);

	// test of the ball has hit the paddle
	if (ball_y > (screen_height / 2) && ball_dy > 0) {
		// extract leading edge of ball
		int x = ball_x + (BALL_SIZE / 2);
		int y = ball_y + (BALL_SIZE / 2);

		// test for collision with paddle
		if ((x >= paddle_x && x <= paddle_x + PADDLE_WIDTH) &&
			(y >= paddle_y && y <= paddle_y + PADDLE_HEIGHT)) {
			// reflect ball
			ball_dy = -ball_dy;

			// push ball out of paddle since it made contact
			ball_y += ball_dy;

			// add a little english to ball based on motion of paddle
			if (RETRO_KeyState(SDL_SCANCODE_RIGHT))
				ball_dx -= (rand() % 3);
			else
				if (RETRO_KeyState(SDL_SCANCODE_LEFT))
					ball_dx += (rand() % 3);
				else
					ball_dx += (-1 + rand() % 3);

			// test if there are no blocks, if so send a message
			// to game loop to start another level
			if (blocks_hit >= (NUM_BLOCK_ROWS * NUM_BLOCK_COLUMNS)) {
				game_state = GAME_STATE_START_LEVEL;
				level++;
			} // end if

			// return
			return;

		} // end if

	} // end if

	// now scan thru all the blocks and see of ball hit blocks
	for (int row = 0; row < NUM_BLOCK_ROWS; row++) {
		// reset column position
		x1 = BLOCK_ORIGIN_X;

		// scan this row of blocks
		for (int col = 0; col < NUM_BLOCK_COLUMNS; col++) {
			// if there is a block here then test it against ball
			if (blocks[row][col] != 0) {
				// test ball against bounding box of block
				if ((ball_cx > x1) && (ball_cx < x1 + BLOCK_WIDTH) &&
					(ball_cy > y1) && (ball_cy < y1 + BLOCK_HEIGHT)) {
					// remove the block
					blocks[row][col] = 0;

					// increment global block counter, so we know 
					// when to start another level up
					blocks_hit++;

					// bounce the ball
					ball_dy = -ball_dy;

					// add a little english
					ball_dx += (-1 + rand() % 3);

					// add some points
					score += 5 * (level + (abs(ball_dx)));

					// that's it -- no more block
					return;

				} // end if  

			} // end if

			// advance column position
			x1 += BLOCK_X_GAP;
		} // end for col

		// advance to next row position
		y1 += BLOCK_Y_GAP;

	} // end for row

} // end Process_Ball

///////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	char buffer[80]; // used to print text

	// what state is the game in? 
	if (game_state == GAME_STATE_INIT) {

		// seed the random number generator
		// so game is different each play
		srand(Start_Clock());

		// set the paddle position here to the middle bottom
		paddle_x = PADDLE_START_X;
		paddle_y = PADDLE_START_Y;

		// set ball position and velocity
		ball_x = 8 + rand() % (screen_width - 16);
		ball_y = BALL_START_Y;
		ball_dx = -4 + rand() % (8 + 1);
		ball_dy = 6 + rand() % 2;

		// transition to start level state
		game_state = GAME_STATE_START_LEVEL;

	} // end if 

	else
		if (game_state == GAME_STATE_START_LEVEL) {
			// get a new level ready to run

			// initialize the blocks
			Init_Blocks();

			// reset block counter
			blocks_hit = 0;

			// transition to run state
			game_state = GAME_STATE_RUN;

		} // end if

		else
			if (game_state == GAME_STATE_RUN) {
				// start the timing clock
				Start_Clock();

				// clear drawing surface for the next frame of animation
				Draw_Rectangle(0, 0, screen_width - 1, screen_height - 1, 200, framebuffer, framebuffer_pitch);

				// move the paddle
				if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
					// move paddle to right
					paddle_x += 8;

					// make sure paddle doesn't go off screen
					if (paddle_x > (screen_width - PADDLE_WIDTH))
						paddle_x = screen_width - PADDLE_WIDTH;

				} // end if
				else
					if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
						// move paddle to right
						paddle_x -= 8;

						// make sure paddle doesn't go off screen
						if (paddle_x < 0)
							paddle_x = 0;

					} // end if

				 // draw blocks
				Draw_Blocks();

				// move the ball
				ball_x += ball_dx;
				ball_y += ball_dy;

				// keep ball on screen, if the ball hits the edge of 
				// screen then bounce it by reflecting its velocity
				if (ball_x > (screen_width - BALL_SIZE) || ball_x < 0) {
					// reflect x-axis velocity
					ball_dx = -ball_dx;

					// update position 
					ball_x += ball_dx;
				} // end if

				// now y-axis
				if (ball_y < 0) {
					// reflect y-axis velocity
					ball_dy = -ball_dy;

					// update position 
					ball_y += ball_dy;
				} // end if
				else
					// penalize player for missing the ball
					if (ball_y > (screen_height - BALL_SIZE)) {
						// reflect y-axis velocity
						ball_dy = -ball_dy;

						// update position 
						ball_y += ball_dy;

						// minus the score
						score -= 100;

					} // end if

				 // next watch out for ball velocity getting out of hand
				if (ball_dx > 8) ball_dx = 8;
				else
					if (ball_dx < -8) ball_dx = -8;

				// test if ball hit any blocks or the paddle
				Process_Ball();

				// draw the paddle and shadow
				Draw_Rectangle(paddle_x - 8, paddle_y + 8,
					paddle_x + PADDLE_WIDTH - 8,
					paddle_y + PADDLE_HEIGHT + 8, 0, framebuffer, framebuffer_pitch);

				Draw_Rectangle(paddle_x, paddle_y,
					paddle_x + PADDLE_WIDTH,
					paddle_y + PADDLE_HEIGHT, PADDLE_COLOR, framebuffer, framebuffer_pitch);

				// draw the ball
				Draw_Rectangle(ball_x - 4, ball_y + 4, ball_x + BALL_SIZE - 4,
					ball_y + BALL_SIZE + 4, 0, framebuffer, framebuffer_pitch);
				Draw_Rectangle(ball_x, ball_y, ball_x + BALL_SIZE,
					ball_y + BALL_SIZE, 255, framebuffer, framebuffer_pitch);

				// draw the info
				sprintf(buffer, "F R E A K O U T           Score %d             Level %d", score, level);
				Draw_Text_GDI(buffer, 8, screen_height - 16, 127, framebuffer, framebuffer_pitch);

				// sync to 33ish fps
				Wait_Clock(30);

				// check of user is trying to exit
				if (RETRO_KeyState(SDL_SCANCODE_BACKSPACE)) {

					// set exit state
					game_state = GAME_STATE_SHUTDOWN;

				} // end if

			} // end if

			else
				if (game_state == GAME_STATE_SHUTDOWN) {

					// switch to exit state
					game_state = GAME_STATE_EXIT;

					RETRO_Quit();

				} // end if

}

///////////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();
}
