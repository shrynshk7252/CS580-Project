/*   CS580 HW1 display functions to be completed   */

#include   "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	char* newFramebuffer;

	newFramebuffer = new char[3 * sizeof(char) * width * height];

	*framebuffer = newFramebuffer;

	if (*framebuffer == NULL){
		return GZ_FAILURE;
	}

	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/
	//allocate memory
	GzDisplay *newDisplay = new GzDisplay;

	//Check the resolution to fit within the boundaries
	if (xRes > MAXXRES){
		newDisplay->xres = MAXXRES;
	}
	else if (xRes < 0){
		newDisplay->xres = 0;
	}
	else{
		newDisplay->xres = xRes;
	}

	if (yRes > MAXYRES){
		newDisplay->yres = MAXYRES;
	}
	else if (yRes < 0){
		newDisplay->yres = 0;
	}
	else{
		newDisplay->yres = yRes;
	}

	newDisplay->fbuf = new GzPixel[(newDisplay->xres + (newDisplay->yres*newDisplay->xres))];

	//pass back the buffer
	*display = newDisplay;

	if ((*display)->fbuf == NULL){
		return GZ_FAILURE;
	}
	return GZ_SUCCESS;
}

int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */
	if (display == NULL){
		return GZ_FAILURE;
	}
	delete[] display->fbuf;
	delete display;
	return GZ_SUCCESS;
}

int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */
	if (display == NULL){
		return GZ_FAILURE;
	}

	*xRes = display->xres;
	*yRes = display->yres;
	return GZ_SUCCESS;
}

int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */
	if (display == NULL){
		return GZ_FAILURE;
	}

	for (int i = 0; i < display->xres; i++){
		for (int j = 0; j < display->yres; j++){
			display->fbuf[ARRAY(i,j)].red = 1000;
			display->fbuf[ARRAY(i,j)].blue = 3000;
			display->fbuf[ARRAY(i,j)].green = 2000;
			display->fbuf[ARRAY(i,j)].alpha = 1;
			display->fbuf[ARRAY(i,j)].z = MAXINT;
		}
	}


	return GZ_SUCCESS;
}

int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */
	//Make sure pixel is inside the frame, and color is valid too
	if (display == NULL){
		return GZ_FAILURE;
	}

	//Ignore pixels that are outside the resolution
	if (i >= 0 && j >= 0 && i < display->xres && j < display->yres){
		//Clamp the intensity values to 0-4095
		if (r < 0){
			r = 0;
		}
		else if (r > 4095){
			r = 4095;
		}

		if (g < 0){
			g = 0;
		}
		else if (g > 4095){
			g = 4095;
		}

		if (b < 0){
			b = 0;
		}
		else if (b > 4095){
			b = 4095;
		}

		if (a < 0){
			a = 0;
		}
		else if (a > 4095){
			a = 4095;
		}

		display->fbuf[ARRAY(i,j)].red = r;
		display->fbuf[ARRAY(i,j)].green = g;
		display->fbuf[ARRAY(i,j)].blue = b;
		display->fbuf[ARRAY(i,j)].alpha = a;
		display->fbuf[ARRAY(i,j)].z = z;
	}

	return GZ_SUCCESS;
}

int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */
	//Make sure i and j are within our resolution
	if (display == NULL){
		return GZ_FAILURE;
	}

	if (i < 0){
		i = 0;
	}
	else if (i > display->xres){
		i = display->xres;
	}
	if (j < 0){
		j = 0;
	}
	else if (j > display->yres){
		j = display->yres;
	}

	//Get the display values
	*r = display->fbuf[ARRAY(i,j)].red;
	*g = display->fbuf[ARRAY(i,j)].blue;
	*b = display->fbuf[ARRAY(i,j)].green;
	*a = display->fbuf[ARRAY(i,j)].alpha;
	*z = display->fbuf[ARRAY(i,j)].z;

	return GZ_SUCCESS;
}

int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\n" */
	fprintf(outfile, "P6 %d %d 255\n", display->xres, display->yres);

	//Convert each pixel's information to bits to write to ppm
	for (int j = 0; j < display->yres; j++){
		for (int i = 0; i < display->xres; i++){
			char bitRed =	display->fbuf[ARRAY(i,j)].red >> 4;
			char bitGreen = display->fbuf[ARRAY(i,j)].green >> 4;
			char bitBlue =	display->fbuf[ARRAY(i,j)].blue >> 4;
			fprintf(outfile, "%c%c%c", bitRed, bitGreen, bitBlue);
		}
	}
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	for (int i = 0; i < display->xres; i++){
		for (int j = 0; j < display->yres; j++){
			framebuffer[ARRAY(i,j) * 3] = display->fbuf[ARRAY(i,j)].blue >> 4;
			framebuffer[ARRAY(i,j) * 3 + 1] = display->fbuf[ARRAY(i,j)].green >> 4;
			framebuffer[ARRAY(i,j) * 3 + 2] = display->fbuf[ARRAY(i,j)].red >> 4;
		}
	}
	return GZ_SUCCESS;
}

/*
//Code for displaying debug message:
CString str;
str.Format(_T("%d"), sizeof(char));
AfxMessageBox(str);
*/
