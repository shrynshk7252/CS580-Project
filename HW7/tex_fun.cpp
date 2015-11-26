/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

#define R 0
#define I 1

GzColor	*image = NULL;
GzColor *MipMap[10];
int xs, ys;
int reset = 1;
float length(GzCoord a);
float fisqrt(float num);

void tri_mm(GzColor *Image, int xs, int ys, int level)
{

	if (level == 0)
	{
		MipMap[level] = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		MipMap[level] = Image;
		tri_mm(MipMap[level], xs / 2, ys / 2, ++level);
	}
	else
	{
		MipMap[level] = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		//FILE			*fd;
		//fd = fopen("texture333333", "w+");
		for (int i = 0; i < xs; i++)
		{
			for (int j = 0; j < ys; j++)
			{
				MipMap[level][i + (j*xs)][RED] = (		MipMap[level - 1][2 * i + (2 * j * 2 * xs)][RED] +
														MipMap[level - 1][(2 * i + 1) + (2 * j * 2 * xs)][RED] +
														MipMap[level - 1][2 * i + ((2 * j + 1) * 2 * xs)][RED] +
														MipMap[level - 1][(2 * i + 1) + ((2 * j + 1) * 2 * xs)][RED]) / 4;

				MipMap[level][i + (j*xs)][GREEN] = (	MipMap[level - 1][2 * i + (2 * j * 2 * xs)][GREEN] +
														MipMap[level - 1][(2 * i + 1) + (2 * j * 2 * xs)][GREEN] +
														MipMap[level - 1][2 * i + ((2 * j + 1) * 2 * xs)][GREEN] +
														MipMap[level - 1][(2 * i + 1) + ((2 * j + 1) * 2 * xs)][GREEN]) / 4;

				MipMap[level][i + (j*xs)][BLUE] = (		MipMap[level - 1][2 * i + (2 * j * 2 * xs)][BLUE] +
														MipMap[level - 1][(2 * i + 1) + (2 * j * 2 * xs)][BLUE] +
														MipMap[level - 1][2 * i + ((2 * j + 1) * 2 * xs)][BLUE] +
														MipMap[level - 1][(2 * i + 1) + ((2 * j + 1) * 2 * xs)][BLUE]) / 4;
				//fprintf(fd, "%f %f %f", MipMap[level][i + (j*xs)][RED], MipMap[level][i + (j*xs)][GREEN], MipMap[level][i + (j*xs)][BLUE]);
				
			}
		}
		//fclose(fd);
		if (xs == 1)
			return;
		tri_mm(MipMap[level], xs / 2, ys / 2, ++level);
	}

}

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	unsigned char	pixel[3];
	unsigned char	dummy;
	char			foo[8];
	int				i, j;
	FILE			*fd;

	if (reset) {          /* open and load texture file */
		fd = fopen("texture", "rb");
		if (fd == NULL) {
			fprintf(stderr, "texture file not found\n");
			exit(-1);
		}
		fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		if (image == NULL) {
			fprintf(stderr, "malloc for texture image failed\n");
			exit(-1);
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset = 0;          /* init is done */
		tri_mm(image, xs, ys, 0);
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	u = (u < 0) ? 0 : ((u > 1) ? 1 : u);
	v = (v < 0) ? 0 : ((v > 1) ? 1 : v);

	/* determine texture cell corner values and perform bilinear interpolation */
	//Get the float value of the u, v coordinate of the texel mapped from
	//   u, v coordinate of the pixel
	float s = u * (xs - 1);
	float t = v * (ys - 1);

	// Get the coordinate of the surrounding int texels 
	int A = floor(s) + (floor(t) * xs);
	int B = ceil(s) + (floor(t) * xs);
	int C = ceil(s) + (ceil(t) * xs);
	int D = floor(s) + (ceil(t) * xs);

	//The amount of influence each texel has on the given u, v
	float Ai = (1 - u) * (1 - v);
	float Bi = u * (1 - v);
	float Ci = u * v;
	float Di = (1 - u) * v;

	/* set color to interpolated GzColor value and return */
	color[RED] = Ci * image[C][RED] + Di * image[D][RED] + Bi * image[B][RED] + Ai * image[A][RED];
	color[GREEN] = Ci * image[C][GREEN] + Di * image[D][GREEN] + Bi * image[B][GREEN] + Ai * image[A][GREEN];
	color[BLUE] = Ci * image[C][BLUE] + Di * image[D][BLUE] + Bi * image[B][BLUE] + Ai * image[A][BLUE];
}





/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	//Julia Set
	// Z = (Z_re + Z_im * i)
	int iterations = 100;
	float c[2] = { 0, 0 };
	c[R] = -0.601;
	c[I] = 0.479;

	float z[2] = { 0, 0 };
	z[R] = u;
	z[I] = v - 1.0;

	int i = 0;
	while (i < iterations){
		//Z^2
		float zr = z[R] * z[R] - z[I] * z[I] + c[R];
		float zi = 2 * z[R] * z[I] + c[I];
		if (z[R] * z[R] + z[I] * z[I] > 4){
			break;
		}
		z[R] = zr;
		z[I] = zi;

		i++;
	}

	float colr = (float)i / (float)iterations;

	color[RED] = colr * .571 * cos(colr);
	color[GREEN] = colr * 2 * cos(.80 * u);
	color[BLUE] = colr * cos(u) + sin(8.8 * colr);

	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if (image != NULL)
		free(image);
	return GZ_SUCCESS;
}