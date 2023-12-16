// T3DLIB6.H - header file for T3DLIB6.H

#ifndef T3DLIB6
#define T3DLIB6

// MACROS /////////////////////////////////////////////////

// lookup 8 bit color value from 16 bit format
#define RGB(r,g,b) (rgblookup[RGB16Bit(r, g, b)])

// GLOBALS //////////////////////////////////////////////////////////////////

// these look up tables are used by the 8-bit lighting engine
// the first one holds a color translation table in the form of each
// row is a color 0..255, and each row consists of 256 shades of that color
// the data in each row is the color/intensity indices and the resulting value
// is an 8-bit index into the real color lookup that should be used as the color

// the second table works by each index being a compressed 16bit RGB value
// the data indexed by that RGB value IS the index 0..255 of the real
// color lookup that matches the desired color the closest

UCHAR rgbilookup[256][256];         // intensity RGB 8-bit lookup storage
UCHAR rgblookup[65536];             // RGB 8-bit color lookup

// FUNCTIONS ////////////////////////////////////////////////////////////////

int RGB_16_8_IndexedRGB_Table_Builder(int rgb_format,             // format we want to build table for
	// 99/100 565
	LPPALETTEENTRY src_palette, // source palette
	UCHAR *rgblookup)           // lookup table
{
	// this function takes as input the rgb format that it should generate the lookup table
	// for;  dd_pixel_format = DD_PIXEL_FORMAT565, or DD_PIXEL_FORMAT555
	// notice in 5.5.5 format the input has only 32K possible colors and the high most
	// bit will be disregarded, thus the look up table will only need to be 32K
	// in either case, it's up to the caller to send in the rgblookup table pre-allocated
	// the function doesn't allocate memory for the caller
	// the function uses a simple least squares scan for all possible RGB colors in the
	// 16-bit color space that map to the discrete RGB space in the 8-bit palette 

	// first check the pointers
	if (!src_palette || !rgblookup)
		return(-1);

	// what is the color depth we are building a table for?
	if (rgb_format == DD_PIXEL_FORMAT565) {
		// there are a total of 64k entries, perform a loop and look them up, do the least 
		// amount of work, even with a pentium, there are 65536*256 interations here!
		for (int rgbindex = 0; rgbindex < 65536; rgbindex++) {
			int  curr_index = -1;        // current color index of best match
			long curr_error = INT_MAX;    // distance in color space to nearest match or "error"

			for (int color_index = 0; color_index < 256; color_index++) {
				// extract r,g,b from rgbindex, assuming an encoding of 5.6.5, then scale to 8.8.8 since 
				// palette is in that format always
				int r = (rgbindex >> 11) << 3;;
				int g = ((rgbindex >> 5) & 0x3f) << 2;
				int b = (rgbindex & 0x1f) << 3;

				// compute distance to color from target
				long delta_red = abs(src_palette[color_index].peRed - r);
				long delta_green = abs(src_palette[color_index].peGreen - g);
				long delta_blue = abs(src_palette[color_index].peBlue - b);
				long error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

				// is this color a better match?
				if (error < curr_error) {
					curr_index = color_index;
					curr_error = error;
				} // end if

			} // end for color_index

		 // best match has been found, enter it into table
			rgblookup[rgbindex] = curr_index;

		} // end for rgbindex

	} // end if
	else
		if (rgb_format == DD_PIXEL_FORMAT555) {
			// there are a total of 32k entries, perform a loop and look them up, do the least 
			// amount of work, even with a pentium, there are 32768*256 interations here!
			for (int rgbindex = 0; rgbindex < 32768; rgbindex++) {
				int  curr_index = -1;        // current color index of best match
				long curr_error = INT_MAX;    // distance in color space to nearest match or "error"

				for (int color_index = 0; color_index < 256; color_index++) {
					// extract r,g,b from rgbindex, assuming an encoding of 5.6.5, then scale to 8.8.8 since 
					// palette is in that format always
					int r = (rgbindex >> 10) << 3;;
					int g = ((rgbindex >> 5) & 0x1f) << 3;
					int b = (rgbindex & 0x1f) << 3;

					// compute distance to color from target
					long delta_red = abs(src_palette[color_index].peRed - r);
					long delta_green = abs(src_palette[color_index].peGreen - g);
					long delta_blue = abs(src_palette[color_index].peBlue - b);
					long error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

					// is this color a better match?
					if (error < curr_error) {
						curr_index = color_index;
						curr_error = error;
					} // end if
				} // end for color_index

			 // best match has been found, enter it into table
				rgblookup[rgbindex] = curr_index;

			} // end for rgbindex

		} // end if
		else
			return(-1); // serious problem! unsupported format, what are you doing to me!!!!

	// return success
	return(1);

} // end RGB_16_8_IndexedRGB_Table_Builder

//////////////////////////////////////////////////////////////////////////////

int RGB_16_8_Indexed_Intensity_Table_Builder(LPPALETTEENTRY src_palette,  // source palette
	UCHAR rgbilookup[256][256],  // lookup table
	int intensity_normalization = 1)
{
	// this function takes the source palette to compute the intensity shading table with
	// the table will be formatted such that each row is a color index, and each column
	// is the shade 0..255 desired, the output is a single byte index
	// in either case, it's up to the caller to send in the rgbilookup table pre-allocated
	// 64k buffer byte [256][256]the function doesn't allocate memory for the caller
	// the function builds the table by looping thru each color in the color palette and then
	// for each color, it scales the color to maximum intensity without overflow the RGB channels
	// and then uses this as the 100% intensity value of the color, then the algorithm computes
	// the 256 shades of the color, and then uses the standard least squares scan the find the 
	// colors in the palette and stores them in the row of the current color under intensity 
	// translation, sounds diabolical huh? Note: if you set intensity normalization to 0
	// the the maximization step isn't performed.

	int ri, gi, bi;        // initial color 
	int rw, gw, bw;        // current working color
	float ratio;         // scaling ratio
	float dl, dr, db, dg;   // intensity gradients for 256 shades

	// first check the pointers
	if (!src_palette || !rgbilookup)
		return(-1);

	// for each color in the palette, compute maximum intensity value then scan
	// for 256 shades of it
	for (int col_index = 0; col_index < 256; col_index++) {
		// extract color from palette
		ri = src_palette[col_index].peRed;
		gi = src_palette[col_index].peGreen;
		bi = src_palette[col_index].peBlue;

		// find largest channel then max it out and scale other
		// channels based on ratio
		if (intensity_normalization == 1) {
			// red largest?
			if (ri >= gi && ri >= bi) {
				// compute scaling ratio
				ratio = (float)255 / (float)ri;

				// max colors out
				ri = 255;
				gi = (int)((float)gi * ratio + 0.5);
				bi = (int)((float)bi * ratio + 0.5);

			} // end if
			else // green largest?
				if (gi >= ri && gi >= bi) {
					// compute scaling ratio
					ratio = (float)255 / (float)gi;

					// max colors out
					gi = 255;
					ri = (int)((float)ri * ratio + 0.5);
					bi = (int)((float)bi * ratio + 0.5);

				} // end if
				else // blue is largest
				{
					// compute scaling ratio
					ratio = (float)255 / (float)bi;

					// max colors out
					bi = 255;
					ri = (int)((float)ri * ratio + 0.5);
					gi = (int)((float)gi * ratio + 0.5);
				} // end if

		} // end if

		// at this point, we need to compute the intensity gradients for this color,
		// so we can compute the RGB values for 256 shades of the current color
		dl = sqrt(ri * ri + gi * gi + bi * bi) / (float)256;
		dr = ri / dl,
			db = gi / dl,
			dg = bi / dl;

		// initialize working color
		rw = 0;
		gw = 0;
		bw = 0;

		// at this point rw,gw,bw, is the color that we need to compute the 256 intensities for to 
		// enter into the col_index (th) row of the table
		for (int intensity_index = 0; intensity_index < 256; intensity_index++) {
			int  curr_index = -1;        // current color index of best match
			long curr_error = INT_MAX;    // distance in color space to nearest match or "error"

			for (int color_index = 0; color_index < 256; color_index++) {
				// compute distance to color from target
				long delta_red = abs(src_palette[color_index].peRed - rw);
				long delta_green = abs(src_palette[color_index].peGreen - gw);
				long delta_blue = abs(src_palette[color_index].peBlue - bw);
				long error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

				// is this color a better match?
				if (error < curr_error) {
					curr_index = color_index;
					curr_error = error;
				} // end if

			} // end for color_index

		   // best match has been found, enter it into table
			rgbilookup[col_index][intensity_index] = curr_index;

			// compute next intensity level (test for overflow, shouldn't happen, but never know)
			if (rw += dr > 255) rw = 255;
			if (gw += dg > 255) gw = 255;
			if (bw += db > 255) bw = 255;

		} // end for intensity_index

	} // end for c_index

	// return success
	return(1);

} // end RGB_16_8_Indexed_Intensity_Table_Builder

///////////////////////////////////////////////////////////////////////////////

#endif
