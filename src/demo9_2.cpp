//
// DEMO9_2.CPP - DirectInput Mouse Demo
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrofont.h"
#include "lib/retromouse.h"
#include "lib/t3dlib1.h"
#include "lib/t3dlib2.h"
#include "lib/t3dlib6.h"

// DEFINES ////////////////////////////////////////////////

#define BUTTON_SPRAY    0    // defines for each button
#define BUTTON_PENCIL   1
#define BUTTON_ERASE    2
#define BUTTON_EXIT     3

// GLOBALS ////////////////////////////////////////////////

// windows vars
char buffer[80];                // used to print text

// demo globals
BOB          buttons,           // a bob with all the buttons
pointer;           // a pointer bob
BITMAP_IMAGE cpanel;            // the control panel
BITMAP_IMAGE canvas;            // off screen drawing canvas

int mouse_x,                    // used to track mouse
mouse_y;

UCHAR mouse_color = 100;          // color of mouse brush

int command_state = 0;            // state of user command

// position of control buttons
int buttons_x[] = { 509, 559, 509, 559 };
int buttons_y[] = { 344, 344, 383, 383 };

// on/off state of buttons
int buttons_state[] = { 0,1,0,0 };

// FUNCTIONS ///////////////////////////////////////////////

void DEMO_Render(double deltatime)
{
	int index; // looping var

	// start the timing clock
	Start_Clock();

	// read mouse
	DInput_Read_Mouse();

	// move the mouse cursor
	mouse_x = (mouse_state.lX);
	mouse_y = (mouse_state.lY);

	// test bounds

	// first x boundaries
	if (mouse_x >= screen_width)
		mouse_x = screen_width - 1;
	else
		if (mouse_x < 0)
			mouse_x = 0;

	// now the y boundaries
	if (mouse_y >= screen_height)
		mouse_y = screen_height - 1;
	else
		if (mouse_y < 0)
			mouse_y = 0;

	// position the pointer bob to the mouse coords
	pointer.x = mouse_x - 16;
	pointer.y = mouse_y - 16;

	// test what the user is doing with the mouse
	if ((mouse_x > 3) && (mouse_x < 500 - 3) &&
		(mouse_y > 3) && (mouse_y < screen_height - 3)) {
		// mouse is within canvas region

		// if left button is down then draw
		if (mouse_state.rgbButtons[0]) {
			// test drawing mode
			if (buttons_state[BUTTON_PENCIL]) {
				// draw a pixel 
				Draw_Pixel(mouse_x, mouse_y, mouse_color, canvas.buffer, canvas.width);
				Draw_Pixel(mouse_x + 1, mouse_y, mouse_color, canvas.buffer, canvas.width);
				Draw_Pixel(mouse_x, mouse_y + 1, mouse_color, canvas.buffer, canvas.width);
				Draw_Pixel(mouse_x + 1, mouse_y + 1, mouse_color, canvas.buffer, canvas.width);
			} else {
				// draw spray
				for (index = 0; index < 10; index++) {
					// get next particle
					int sx = mouse_x - 8 + rand() % 16;
					int sy = mouse_y - 8 + rand() % 16;

					// make sure particle is in bounds
					if (sx > 0 && sx < 500 && sy > 0 && sy < screen_height)
						Draw_Pixel(sx, sy, mouse_color, canvas.buffer, canvas.width);
				} // end for index

			} // end else

		} // end if left button
		else // right button is eraser
			if (mouse_state.rgbButtons[1]) {
				// test drawing mode
				if (buttons_state[BUTTON_PENCIL]) {
					// erase a pixel 
					Draw_Pixel(mouse_x, mouse_y, 0, canvas.buffer, canvas.width);
					Draw_Pixel(mouse_x + 1, mouse_y, 0, canvas.buffer, canvas.width);
					Draw_Pixel(mouse_x, mouse_y + 1, 0, canvas.buffer, canvas.width);
					Draw_Pixel(mouse_x + 1, mouse_y + 1, 0, canvas.buffer, canvas.width);
				} // end if
				else {
					// erase spray
					for (index = 0; index < 20; index++) {
						// get next particle
						int sx = mouse_x - 8 + rand() % 16;
						int sy = mouse_y - 8 + rand() % 16;

						// make sure particle is in bounds
						if (sx > 0 && sx < 500 && sy > 0 && sy < screen_height)
							Draw_Pixel(sx, sy, 0, canvas.buffer, canvas.width);
					} // end for index

				} // end else

			} // end if left button

	} // end if
	else
		if ((mouse_x > 500 + 16) && (mouse_x < 500 + 16 + 8 * 9) &&
			(mouse_y > 8) && (mouse_y < 8 + 32 * 9)) {
			// within palette

			// test if button left button is down
			if (mouse_state.rgbButtons[0]) {
				// see what color cell user is pointing to
				int cell_x = (mouse_x - (500 + 16)) / 9;
				int cell_y = (mouse_y - (8)) / 9;

				// change color
				mouse_color = cell_x + cell_y * 8;

			} // end if
		} // end if
		else
			if ((mouse_x > 500) && (mouse_x < (500 + 100)) &&
				(mouse_y > 344) && (mouse_y < (383 + 34))) {
				// within button area
				// test for each button
				for (index = 0; index < 4; index++) {
					if ((mouse_x > buttons_x[index]) && (mouse_x < (buttons_x[index] + 32)) &&
						(mouse_y > buttons_y[index]) && (mouse_y < (buttons_y[index] + 34)))
						break;

				} // end for

			// at this point we know where the user is, now determine what he
			// is doing with the buttons
				switch (index) {
				case BUTTON_SPRAY:
				{
					// if left button is down simply activate spray mode
					if (mouse_state.rgbButtons[0]) {
						// depress button
						buttons_state[index] = 1;

						// de-activate pencil mode
						buttons_state[BUTTON_PENCIL] = 0;
					} // end if
					else {
						// make sure button is up
						// buttons_state[index] = 0;
					} // end else

				} break;

				case BUTTON_PENCIL:
				{
					// if left button is down activate spray mode
					if (mouse_state.rgbButtons[0]) {
						// depress button
						buttons_state[index] = 1;

						// de-activate spray mode
						buttons_state[BUTTON_SPRAY] = 0;

					} // end if
					else {
						// make sure button is up
						// buttons_state[index] = 0;
					} // end else

				} break;

				case BUTTON_ERASE:
				{
					// test if left button is down, if so clear screen
					if (mouse_state.rgbButtons[0]) {
						// clear memory
						memset(canvas.buffer, 0, canvas.width * canvas.height);

						// depress button
						buttons_state[index] = 1;
					} // end if
					else {
						// make sure button is up
						buttons_state[index] = 0;
					} // end else
				} break;

				case BUTTON_EXIT:
				{
					// test if left button down, if so bail
					if (mouse_state.rgbButtons[0])
						RETRO_Quit();

				} break;

				} // end switch

			} // end if
			else {
				// no mans land

			} // end else

	// draw the canvas
	Draw_Bitmap(&canvas, framebuffer, framebuffer_pitch, 0);

	// draw control panel
	Draw_Bitmap(&cpanel, framebuffer, framebuffer_pitch, 0);

	// draw the color palette
	for (int col = 0; col < 256; col++) {
		Draw_Rectangle(500 + 16 + (col % 8) * 9, 8 + (col / 8) * 9,
			500 + 16 + (col % 8) * 9 + 8, 8 + (col / 8) * 9 + 8,
			col, framebuffer, framebuffer_pitch);

	} // end for col

	// draw the current color selected
	Draw_Rectangle(533, 306, 533 + 34, 306 + 34, mouse_color, framebuffer, framebuffer_pitch);

	// draw the buttons
	for (index = 0; index < 4; index++) {
		// set position of button bob
		buttons.x = buttons_x[index];
		buttons.y = buttons_y[index];

		// now select the on/off frame based on if the
		// button is off
		if (buttons_state[index] == 0)
			buttons.curr_frame = index;
		else // button is on
			buttons.curr_frame = index + 4;

		// draw the button
		Draw_BOB(&buttons, framebuffer, framebuffer_pitch);

	} // end for index

	static int green = 0;

	// display coords
	sprintf(buffer, "Pointer (%d,%d)", mouse_x, mouse_y);
	Draw_Text_GDI(buffer, 8, screen_height - 16, RGB(0, 255, 0), framebuffer, framebuffer_pitch);
	Draw_Text_GDI("T3D Paint Version 2.0 - Press <ESC> to Exit.", 0, 0, RGB(0, (green & 255), 0), framebuffer, framebuffer_pitch);

	// a little animation
	++green;

	// draw the cursor last
	Draw_BOB(&pointer, framebuffer, framebuffer_pitch);

	// sync to 30 fps
	Wait_Clock(30);
}

//////////////////////////////////////////////////////////

void DEMO_Initialize(void)
{
	// Initialize T3DLIB
	T3DLIB_Init();

	// set the global mouse position
	mouse_x = screen_height / 2;
	mouse_y = screen_height / 2;

	// load the master bitmap in with all the graphics
	Load_Bitmap_File(&bitmap8bit, "assets/paint.bmp");
	Set_Palette(bitmap8bit.palette);

	// create the pointer bob
	Create_BOB(&pointer, mouse_x, mouse_y, 32, 34, 1,
		BOB_ATTR_VISIBLE | BOB_ATTR_SINGLE_FRAME, 0);

	// load the image for the pointer in
	Load_Frame_BOB(&pointer, &bitmap8bit, 0, 0, 2, BITMAP_EXTRACT_MODE_CELL);

	// create the button bob
	Create_BOB(&buttons, 0, 0, 32, 34, 8,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME, 0);

	// load buttons in, two banks of 4, all the off's, then all the on's
	for (int index = 0; index < 8; index++)
		Load_Frame_BOB(&buttons, &bitmap8bit, index, index % 4, index / 4, BITMAP_EXTRACT_MODE_CELL);

	// create the bitmap to hold the control panel
	Create_Bitmap(&cpanel, 500, 0, 104, 424);
	Load_Image_Bitmap(&cpanel, &bitmap8bit, 150, 0, BITMAP_EXTRACT_MODE_ABS);

	// create the drawing canvas bitmap
	Create_Bitmap(&canvas, 0, 0, 500, screen_height);
	memset(canvas.buffer, 0, canvas.width * canvas.height);
	canvas.attr = BITMAP_ATTR_LOADED;

	// create lookup for lighting engine
	RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,  // format we want to build table for
		palette,             // source palette
		rgblookup);          // lookup table
}

///////////////////////////////////////////////////////////

void DEMO_Deinitialize(void)
{
	// unload the bitmap file
	Unload_Bitmap_File(&bitmap8bit);

	// delete all bobs and bitmaps
	Destroy_BOB(&buttons);
	Destroy_BOB(&pointer);
	Destroy_Bitmap(&cpanel);
	Destroy_Bitmap(&canvas);
}

///////////////////////////////////////////////////////////
