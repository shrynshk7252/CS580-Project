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
short ctoi(float color);
float length(GzCoord a);
float fisqrt(float num);
void GzFlushMipMap2File(FILE* outfile, GzColor *mipmap[10], int level, int xres, int yres);

void tri_mm(GzColor *Image, int xs, int ys, int level)
{

	if (level == 0)
	{
		MipMap[level] = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		MipMap[level] = Image;
		char file[100] = "mipmapLevel";

		char  buffer[10];
		strcat(file, itoa(level, buffer, 10));
		strcat(file, ".ppm");
		FILE* outfile;
		if ((outfile = fopen(file, "wb")) == NULL)
		{
			AfxMessageBox("The output file was not opened\n");
		}
		GzFlushMipMap2File(outfile, MipMap, level, xs, ys);
		fclose(outfile);
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

				/*if (level == 1)
				{
					MipMap[level][i + (j*xs)][RED] = 1;
					MipMap[level][i + (j*xs)][GREEN] = 0;
					MipMap[level][i + (j*xs)][BLUE] = 0;
				}
				else if (level == 2)
				{
					MipMap[level][i + (j*xs)][RED] = 0;
					MipMap[level][i + (j*xs)][GREEN] = 1;
					MipMap[level][i + (j*xs)][BLUE] = 0;
				}
				else if (level == 3)
				{
					MipMap[level][i + (j*xs)][RED] = 0;
					MipMap[level][i + (j*xs)][GREEN] = 0;
					MipMap[level][i + (j*xs)][BLUE] = 1;
				}
				else if (level == 4)
				{
					MipMap[level][i + (j*xs)][RED] = 1;
					MipMap[level][i + (j*xs)][GREEN] = 0;
					MipMap[level][i + (j*xs)][BLUE] = 1;
				}
				else if (level == 5)
				{
					MipMap[level][i + (j*xs)][RED] = 0;
					MipMap[level][i + (j*xs)][GREEN] = 1;
					MipMap[level][i + (j*xs)][BLUE] = 1;
				}
				else if (level == 6)
				{
					MipMap[level][i + (j*xs)][RED] = 1;
					MipMap[level][i + (j*xs)][GREEN] = 1;
					MipMap[level][i + (j*xs)][BLUE] = 0;
				}
				else
				{
					MipMap[level][i + (j*xs)][RED] = 0;
					MipMap[level][i + (j*xs)][GREEN] = 0;
					MipMap[level][i + (j*xs)][BLUE] = 0;
				}*/

				//fprintf(fd, "%f %f %f", MipMap[level][i + (j*xs)][RED], MipMap[level][i + (j*xs)][GREEN], MipMap[level][i + (j*xs)][BLUE]);			
			}
		}

		char file[100] = "mipmapLevel";

		char  buffer[10];
		strcat(file, itoa(level, buffer, 10));
		strcat(file, ".ppm");
		FILE* outfile;
		if ((outfile = fopen(file, "wb")) == NULL)
		{
			AfxMessageBox("The output file was not opened\n");
		}
		GzFlushMipMap2File(outfile, MipMap, level, xs, ys);
		fclose(outfile);

		//fclose(fd);
		if (xs == 1)
			return;
		tri_mm(MipMap[level], xs / 2, ys / 2, ++level);
	}
}

/* Image texture function */
int tex_fun(float u, float v, GzColor color, int level1, int level2, float level)
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

	//color[RED] = Ci * image[C][RED] + Di * image[D][RED] + Bi * image[B][RED] + Ai * image[A][RED];
	//color[GREEN] = Ci * image[C][GREEN] + Di * image[D][GREEN] + Bi * image[B][GREEN] + Ai * image[A][GREEN];
	//color[BLUE] = Ci * image[C][BLUE] + Di * image[D][BLUE] + Bi * image[B][BLUE] + Ai * image[A][BLUE];

	xs = 512 / pow(2, (float)(level1));
	ys = xs;
	s = u * (xs - 1);
	t = v * (ys - 1);

	// Get the coordinate of the surrounding int texels 
	A = floor(s) + (floor(t) * xs);
	B = ceil(s) + (floor(t) * xs);
	C = ceil(s) + (ceil(t) * xs);
	D = floor(s) + (ceil(t) * xs);

	//The amount of influence each texel has on the given u, v
	Ai = (1 - u) * (1 - v);
	Bi = u * (1 - v);
	Ci = u * v;
	Di = (1 - u) * v;
	GzColor color1;
	/* set color to interpolated GzColor value and return */
	color1[RED] = Ci * MipMap[level1][C][RED] + Di * MipMap[level1][D][RED] + Bi * MipMap[level1][B][RED] + Ai * MipMap[level1][A][RED];
	color1[GREEN] = Ci * MipMap[level1][C][GREEN] + Di * MipMap[level1][D][GREEN] + Bi * MipMap[level1][B][GREEN] + Ai * MipMap[level1][A][GREEN];
	color1[BLUE] = Ci * MipMap[level1][C][BLUE] + Di * MipMap[level1][D][BLUE] + Bi * MipMap[level1][B][BLUE] + Ai * MipMap[level1][A][BLUE];

	xs = 512 / pow(2, (float)(level2) );
	ys = xs;
	s = u * (xs - 1);
	t = v * (ys - 1);

	// Get the coordinate of the surrounding int texels 
	A = floor(s) + (floor(t) * xs);
	B = ceil(s) + (floor(t) * xs);
	C = ceil(s) + (ceil(t) * xs);
	D = floor(s) + (ceil(t) * xs);

	//The amount of influence each texel has on the given u, v
	Ai = (1 - u) * (1 - v);
	Bi = u * (1 - v);
	Ci = u * v;
	Di = (1 - u) * v;
	GzColor color2;
	/* set color to interpolated GzColor value and return */
	color2[RED] = Ci * MipMap[level2][C][RED] + Di * MipMap[level2][D][RED] + Bi * MipMap[level2][B][RED] + Ai * MipMap[level2][A][RED];
	color2[GREEN] = Ci * MipMap[level2][C][GREEN] + Di * MipMap[level2][D][GREEN] + Bi * MipMap[level2][B][GREEN] + Ai * MipMap[level2][A][GREEN];
	color2[BLUE] = Ci * MipMap[level2][C][BLUE] + Di * MipMap[level2][D][BLUE] + Bi * MipMap[level2][B][BLUE] + Ai * MipMap[level2][A][BLUE];


	float red = (1 - level)*color1[RED] + level*color2[RED];
	float green = (1 - level)*color1[GREEN] + level*color2[GREEN];
	float blue = (1 - level)*color1[BLUE] + level*color2[BLUE];

	color[RED] = red;
	color[GREEN] = green;
	color[BLUE] = blue;
	
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

void GzFlushMipMap2File(FILE* outfile, GzColor *mipmap[10], int level, int xres, int yres)
{

	/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\n" */
	fprintf(outfile, "P6 %d %d 255\n", xres, yres);

	//Convert each pixel's information to bits to write to ppm
	for (int i = 0; i < xres; i++){
		for (int j = 0; j < yres; j++){
			short colorR = ctoi(mipmap[level][j + (i*yres)][RED]);
			short colorG = ctoi(mipmap[level][j + (i*yres)][GREEN]);
			short colorB = ctoi(mipmap[level][j + (i*yres)][BLUE]);
			char bitRed = colorR >> 4;
			char bitGreen = colorG >> 4;
			char bitBlue = colorB >> 4;
			fprintf(outfile, "%c%c%c", bitRed, bitGreen, bitBlue);
		}
	}
}

/* Free texture memory */
int GzFreeTexture()
{
	if (image != NULL)
		free(image);
	return GZ_SUCCESS;
}
