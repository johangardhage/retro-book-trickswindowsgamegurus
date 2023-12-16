//
// DEMO13_7_16b.CPP - 2D collision off irregular shaped object demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

#define NUM_BALLS       10   // number of pool balls
#define BALL_RADIUS     12   // radius of ball

// center position of shape
#define SHAPE_CENTER_X         320
#define SHAPE_CENTER_Y         240

// variable lookup indices
#define INDEX_X               0 
#define INDEX_Y               1  
#define INDEX_XV              2 
#define INDEX_YV              3  

// MACROS ///////////////////////////////////////////////

#define RAND_RANGE(x,y) ( (x) + (rand()%((y)-(x)+1)))
#define DOT_PRODUCT(ux,uy,vx,vy) ((ux)*(vx) + (uy)*(vy))

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                 // used to print text

BITMAP_IMAGE background_bmp;      // holds the background
BOB          balls[NUM_BALLS];    // the balls

int ball_ids[8];                  // sound ids for balls

POLYGON2D shape;                  // the shape to bounce off

// FUNCTIONS //////////////////////////////////////////////

void Compute_Collisions(void)
{
	// this function computes if any ball has hit one of the edges of the polygon
	// if so the ball is bounced

	float length, s, t, s1x, s1y, s2x, s2y, p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y, npx, npy, Nx, Ny, Fx, Fy;

	for (int index = 0; index < NUM_BALLS; index++) {
		// first move ball
		balls[index].varsF[INDEX_X] += balls[index].varsF[INDEX_XV];
		balls[index].varsF[INDEX_Y] += balls[index].varsF[INDEX_YV];

		// now project velocity vector forward and test for intersection with all lines of polygon shape

		// build up vector in direction of trajectory
		p0x = balls[index].varsF[INDEX_X];
		p0y = balls[index].varsF[INDEX_Y];

#if 1 // this is the velocity vector used as segment 1
		p1x = balls[index].varsF[INDEX_X] + balls[index].varsF[INDEX_XV];
		p1y = balls[index].varsF[INDEX_Y] + balls[index].varsF[INDEX_YV];
		s1x = p1x - p0x;
		s1y = p1y - p0y;

		// normalize and scale to 1.25*radius
		length = sqrt(s1x * s1x + s1y * s1y);
		s1x = 1.25 * BALL_RADIUS * s1x / length;
		s1y = 1.25 * BALL_RADIUS * s1y / length;
		p1x = p0x + s1x;
		p1y = p0y + s1y;

#endif

		// for each line try and intersect
		for (int line = 0; line < shape.num_verts; line++) {
			// now build up vector based on line
			p2x = shape.vlist[line].x + shape.x0;
			p2y = shape.vlist[line].y + shape.y0;

			p3x = shape.vlist[(line + 1) % (shape.num_verts)].x + shape.x0;
			p3y = shape.vlist[(line + 1) % (shape.num_verts)].y + shape.y0;

			s2x = p3x - p2x;
			s2y = p3y - p2y;

#if 0 // this is the perp vector used as segment 1
			// normalize s2x, s2y to create a perpendicular collision vector from the ball center
			length = sqrt(s2x * s2x + s2y * s2y);
			s1x = BALL_RADIUS * s2y / length;
			s1y = -BALL_RADIUS * s2x / length;
			p1x = p0x + s1x;
			p1y = p0y + s1y;
#endif
			// compute s and t, the parameters
			s = (-s1y * (p0x - p2x) + s1x * (p0y - p2y)) / (-s2x * s1y + s1x * s2y);
			t = (s2x * (p0y - p2y) - s2y * (p0x - p2x)) / (-s2x * s1y + s1x * s2y);

			// test for valid range (0..1)
			if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
				// find collision point based on s
//				xi = p0x + s * s1x;
//				yi = p0y + s * s1y;

				// now we know point of intersection, reflect ball at current location

				// N = (-I . N')*N'
				// F = 2*N + I
				npx = -s2y;
				npy = s2x;

				// normalize p
				length = sqrt(npx * npx + npy * npy);
				npx /= length;
				npy /= length;

				// compute N = (-I . N')*N'
				Nx = -(balls[index].varsF[INDEX_XV] * npx + balls[index].varsF[INDEX_YV] * npy) * npx;
				Ny = -(balls[index].varsF[INDEX_XV] * npx + balls[index].varsF[INDEX_YV] * npy) * npy;

				// compute F = 2*N + I
				Fx = 2 * Nx + balls[index].varsF[INDEX_XV];
				Fy = 2 * Ny + balls[index].varsF[INDEX_YV];

				// update velocity with results
				balls[index].varsF[INDEX_XV] = Fx;
				balls[index].varsF[INDEX_YV] = Fy;

				balls[index].varsF[INDEX_X] += balls[index].varsF[INDEX_XV];
				balls[index].varsF[INDEX_Y] += balls[index].varsF[INDEX_YV];

				// break out of for line
				//break;

			} // end if

		} // end for line

	} // end for ball index

} // end Collision_Collisions

//////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	static int rotate = 0;

	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap16(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// draw shape
	Draw_Polygon2D16(&shape, framebuffer, framebuffer_pitch);

	// have a little fun
	if (++rotate > 10) {
		Rotate_Polygon2D(&shape, 1);
		rotate = 0;
	}

	// move the balls and compute collisions
	Compute_Collisions();

	// draw the balls
	for (int index = 0; index < NUM_BALLS; index++) {
		balls[index].x = balls[index].varsF[INDEX_X] + 0.5 - BALL_RADIUS;
		balls[index].y = balls[index].varsF[INDEX_Y] + 0.5 - BALL_RADIUS;

		Draw_BOB16(&balls[index], framebuffer, framebuffer_pitch);
	} // end for

	// draw the velocity vectors
	for (int index = 0; index < NUM_BALLS; index++) {
		Draw_Clip_Line16(balls[index].varsF[INDEX_X] + 0.5,
			balls[index].varsF[INDEX_Y] + 0.5,
			balls[index].varsF[INDEX_X] + 2 * balls[index].varsF[INDEX_XV] + 0.5,
			balls[index].varsF[INDEX_Y] + 2 * balls[index].varsF[INDEX_YV] + 0.5,
			RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);
	} // end for

	// draw the title
	Draw_Text_GDI16("(16-Bit Version) Object to Contour Collision DEMO, Press <ESC> to Exit.", 10, 10, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	// run collision algorithm here
	Compute_Collisions();

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
	Load_Bitmap_File(&bitmap8bit, "assets/greengrid24.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&background_bmp, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap8bit);

	// load the bitmaps
	Load_Bitmap_File(&bitmap8bit, "assets/poolballs24.bmp");

	// create master ball
	Create_BOB(&balls[0], 0, 0, 24, 24, 6, BOB_ATTR_MULTI_FRAME | BOB_ATTR_VISIBLE, 0, 0, 16);

	// load the imagery in
	for (int index = 0; index < 6; index++)
		Load_Frame_BOB16(&balls[0], &bitmap8bit, index, index, 0, BITMAP_EXTRACT_MODE_CELL);

	// create all the clones
	for (int index = 1; index < NUM_BALLS; index++)
		Clone_BOB(&balls[0], &balls[index]);

	// now set the initial conditions of all the balls
	for (int index = 0; index < NUM_BALLS; index++) {
		// set position in center of object
		balls[index].varsF[INDEX_X] = RAND_RANGE(SHAPE_CENTER_X - 50, SHAPE_CENTER_X + 50);
		balls[index].varsF[INDEX_Y] = RAND_RANGE(SHAPE_CENTER_Y - 50, SHAPE_CENTER_Y + 50);

		do {
			// set initial velocity
			balls[index].varsF[INDEX_XV] = RAND_RANGE(-100, 100) / 30;
			balls[index].varsF[INDEX_YV] = RAND_RANGE(-100, 100) / 30;
		} while (balls[index].varsF[INDEX_XV] == 0 && balls[index].varsF[INDEX_XV] == 0);

		// set ball color
		balls[index].curr_frame = rand() % 6;

	} // end for index

	// unload bitmap image
	Unload_Bitmap_File(&bitmap8bit);

	// define points of shape
	VERTEX2DF shape_vertices[10] =
	{ 328 - SHAPE_CENTER_X,60 - SHAPE_CENTER_Y,
	  574 - SHAPE_CENTER_X,162 - SHAPE_CENTER_Y,
	  493 - SHAPE_CENTER_X,278 - SHAPE_CENTER_Y,
	  605 - SHAPE_CENTER_X,384 - SHAPE_CENTER_Y,
	  484 - SHAPE_CENTER_X,433 - SHAPE_CENTER_Y,
	  306 - SHAPE_CENTER_X,349 - SHAPE_CENTER_Y,
	  150 - SHAPE_CENTER_X,413 - SHAPE_CENTER_Y,
	  28 - SHAPE_CENTER_X,326 - SHAPE_CENTER_Y,
	  152 - SHAPE_CENTER_X,281 - SHAPE_CENTER_Y,
	  73 - SHAPE_CENTER_X,138 - SHAPE_CENTER_Y };

	// initialize shape
	shape.state = 1;   // turn it on
	shape.num_verts = 10;
	shape.x0 = SHAPE_CENTER_X;
	shape.y0 = SHAPE_CENTER_Y;
	shape.xv = 0;
	shape.yv = 0;
	shape.color = RGB16Bit(0, 255, 0); // green
	shape.vlist = new VERTEX2DF[shape.num_verts];

	for (int index = 0; index < shape.num_verts; index++)
		shape.vlist[index] = shape_vertices[index];

	// build the 360 degree look ups
	Build_Sin_Cos_Tables();
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// kill all the bobs
	for (int index = 0; index < NUM_BALLS; index++)
		Destroy_BOB(&balls[index]);
}

//////////////////////////////////////////////////////////
