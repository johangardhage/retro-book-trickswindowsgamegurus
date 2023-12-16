//
// DEMO13_9_16b.CPP - 2D Forward Kinematic demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/t3dlib1.h"

// DEFINES ////////////////////////////////////////////////

#define ARM_ANG         2   // arm rotation angle increment

// MACROS ///////////////////////////////////////////////

#define RAND_RANGE(x,y) ( (x) + (rand()%((y)-(x)+1)))

// GLOBALS ////////////////////////////////////////////////

char buffer[256];                 // used to print text

BITMAP_IMAGE background_bmp;      // holds the background

POLYGON2D arm1[2], arm2[2];       // the arms of the linkage
// since the transform function modify
// the local coordites, I need two versions, 
// so I can make a copy of the original object and always
// transform from there, this is just because normally
// you have a set of local coords and the transformed temp
// coords, but I saved space and made them the same, so
// this is a fix to make it work
// hence index 0 is original and 1 will be transformed

// FUNCTIONS //////////////////////////////////////////////

void Copy_Polygon2D(POLYGON2D_PTR dest, POLYGON2D_PTR source)
{
	// copies one poly to another
	dest->state = source->state;
	dest->num_verts = source->num_verts;
	dest->x0 = source->x0;
	dest->y0 = source->y0;
	dest->xv = source->xv;
	dest->yv = source->yv;
	dest->color = source->color;

	// now copy vertex list
	if (dest->vlist)
		free(dest->vlist);

	// allocate memory for new list
	dest->vlist = new VERTEX2DF[dest->num_verts];

	// now copy the vertex list
	memcpy(dest->vlist, source->vlist, sizeof(VERTEX2DF) * dest->num_verts);

} // end Copy_Polygon2D

//////////////////////////////////////////////////////////////////////

int Translate_Polygon2D_Verts_Mat(POLYGON2D_PTR poly, float dx, float dy)
{
	// this function translates the local coordinates of the polygon at the vertex
	// level rather than the center point

	// test for valid pointer
	if (!poly)
		return(0);

	MATRIX3X2 mt; // used to hold translation transform matrix

	// initialize the matrix with translation values dx dy
	Mat_Init_3X2(&mt, 1, 0, 0, 1, dx, dy);

	// loop and translate each point
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++) {
		// scale and store result back

		// create a 1x2 matrix to do the transform
		MATRIX1X2 p0 = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y };
		MATRIX1X2 p1 = { 0,0 }; // this will hold result

		// now translate via a matrix multiply
		Mat_Mul_1X2_3X2(&p0, &mt, &p1);

		// now copy the result back into vertex
		poly->vlist[curr_vert].x = p1.M[0];
		poly->vlist[curr_vert].y = p1.M[1];

	} // end for curr_vert

	// return success
	return(1);

} // end Translate_Polygon2D_Verts_Mat

//////////////////////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	static int   curr_angle1 = -90,
		curr_angle2 = 45; // current link angles

	// start the timing clock
	Start_Clock();

	// draw background
	Draw_Bitmap16(&background_bmp, framebuffer, framebuffer_pitch, 0);

	// test for arm1 rotation
	if (RETRO_KeyState(SDL_SCANCODE_A)) {
		curr_angle1 -= ARM_ANG;
	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_S)) {
			curr_angle1 += ARM_ANG;
		} // end if

	// test for arm2 rotation
	if (RETRO_KeyState(SDL_SCANCODE_D)) {
		curr_angle2 -= ARM_ANG;
	} // end if
	else
		if (RETRO_KeyState(SDL_SCANCODE_F)) {
			curr_angle2 += ARM_ANG;
		} // end if

	// test for overflow
	if (curr_angle1 >= 360)
		curr_angle1 -= 360;
	else
		if (curr_angle1 < 0)
			curr_angle1 += 360;

	if (curr_angle2 >= 360)
		curr_angle2 -= 360;
	else
		if (curr_angle2 < 0)
			curr_angle2 += 360;

	// first arm1

	// first copy arms to transform polygon
	Copy_Polygon2D(&arm1[1], &arm1[0]);

	// rotate
	Rotate_Polygon2D_Mat(&arm1[1], curr_angle1);

	// now translate
	Translate_Polygon2D_Verts_Mat(&arm1[1], 324, 400);

	// now arm2

	// first copy arms to transform polygon
	Copy_Polygon2D(&arm2[1], &arm2[0]);

	// rotate
	Rotate_Polygon2D_Mat(&arm2[1], curr_angle2);

	// and rotate around link 1
	Rotate_Polygon2D_Mat(&arm2[1], curr_angle1);

	// now translate to link 1
	Translate_Polygon2D_Verts_Mat(&arm2[1], (arm1[1].vlist[1].x + arm1[1].vlist[2].x) / 2,
		(arm1[1].vlist[1].y + arm1[1].vlist[2].y) / 2);

	// draw arms
	Draw_Polygon2D16(&arm1[1], framebuffer, framebuffer_pitch);
	Draw_Polygon2D16(&arm2[1], framebuffer, framebuffer_pitch);

	// draw the title
	Draw_Text_GDI16("(16-Bit Version) Forward Kinematic DEMO, Press <ESC> to Exit.", 10, 10, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);
	Draw_Text_GDI16("<A>, <S> to adjust Arm 1, <D>, <F> to adjust Arm 2", 10, 25, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

	sprintf(buffer, "Arm 1 Angle=%d, Arm 2 Angle=%d", 360 - curr_angle1, 360 - curr_angle2);
	Draw_Text_GDI16(buffer, 10, 60, RGB16Bit(255, 255, 255), framebuffer, framebuffer_pitch);

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
	Load_Bitmap_File(&bitmap8bit, "assets/armgrid24.bmp");
	Create_Bitmap(&background_bmp, 0, 0, 640, 480, 16);
	Load_Image_Bitmap16(&background_bmp, &bitmap8bit, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap8bit);
	
	// define points of arm1
	VERTEX2DF arm1_vertices[4] = { -10,-10, 100,-10, 100,10, -10,10, };

	// initialize arm1
	arm1[0].state = 1;   // turn it on
	arm1[0].num_verts = 4;
	arm1[0].x0 = 0; // position it
	arm1[0].y0 = 0;
	arm1[0].xv = 0;
	arm1[0].yv = 0;
	arm1[0].color = RGB16Bit(255, 0, 0); // red
	arm1[0].vlist = new VERTEX2DF[arm1[0].num_verts];

	for (int index = 0; index < arm1[0].num_verts; index++)
		arm1[0].vlist[index] = arm1_vertices[index];

	// define points of arm2
	VERTEX2DF arm2_vertices[4] = { -10,-10, 100,-10, 100,10, -10,10, };

	// initialize arm2
	arm2[0].state = 1;   // turn it on
	arm2[0].num_verts = 4;
	arm2[0].x0 = 0; // position it
	arm2[0].y0 = 0;
	arm2[0].xv = 0;
	arm2[0].yv = 0;
	arm2[0].color = RGB16Bit(0, 0, 255); // blue
	arm2[0].vlist = new VERTEX2DF[arm2[0].num_verts];

	for (int index = 0; index < arm2[0].num_verts; index++)
		arm2[0].vlist[index] = arm2_vertices[index];

	// build the 360 degree look ups
	Build_Sin_Cos_Tables();
}

///////////////////////////////////////////////////////////
