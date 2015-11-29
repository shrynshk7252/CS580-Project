/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

void	swap3(int i, int j, float triArray[][3]);
void	swap2(int i, int j, float coordArray[][2]);
short	ctoi(float color);
void	matrixMult(GzMatrix mat1, GzMatrix mat2, GzMatrix matOut);
int		normalizeVector(GzCoord v);
float	fisqrt(float num);
float	dotProduct(GzCoord v1, GzCoord v2);
int		shadingEq(GzRender* render, GzColor color, GzCoord norm, bool gouraudTexture);
float	triArea(GzCoord A, GzCoord B, GzCoord C);

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	//	1		0		0		0	
	//	0		cos		-sin	0
	//	0		sin		cos		0
	//	0		0		0		1
	float rads = degree * (PI / 180.0);

	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = cos(rads);
	mat[1][2] = -sin(rads);
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = sin(rads);
	mat[2][2] = cos(rads);
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	//	cos		0		sin		0
	//	0		1		0		0
	//	-sin	0		cos		0
	//	0		0		0		1
	float rads = degree * (PI / 180.0);

	mat[0][0] = cos(rads);
	mat[0][1] = 0;
	mat[0][2] = sin(rads);
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = -sin(rads);
	mat[2][1] = 0;
	mat[2][2] = cos(rads);
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	//	cos		-sin	0		0
	//	sin		cos		0		0
	//	0		0		1		0
	//	0		0		0		1
	float rads = degree * (PI / 180.0);

	mat[0][0] = cos(rads);
	mat[0][1] = -sin(rads);
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = sin(rads);
	mat[1][1] = cos(rads);
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	//	1	0	0	tx
	//	0	1	0	ty
	//	0	0	1	tz
	//	0	0	0	1
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = translate[X];

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = translate[Y];

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = translate[Z];

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	//	sx	0	0	0
	//	0	sy	0	0
	//	0	0	sz	0
	//	0	0	0	1
	mat[0][0] = scale[X];
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = scale[Y];
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = scale[Z];
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- setup Xsp and anything only done once 
- save the pointer to display 
- init default camera 
*/ 

	GzRender* newRender;

	newRender = new GzRender;
	newRender->display = display;

	//initialize the mat stack to empty
	newRender->matlevel = 0;

	//set the transform matrix
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			newRender->Xsp[i][j] = 0;
			newRender->camera.Xiw[i][j] = 0;
			newRender->camera.Xpi[i][j] = 0;
		}
	}

	//Set Xsp
	newRender->Xsp[0][0] = newRender->display->xres / 2;
	newRender->Xsp[0][3] = newRender->display->xres / 2;

	newRender->Xsp[1][1] = -(newRender->display->yres / 2);
	newRender->Xsp[1][3] = newRender->display->yres / 2;

	newRender->Xsp[2][2] = MAXINT;

	newRender->Xsp[3][3] = 1.0;

	//set the default camera
	newRender->camera.FOV = DEFAULT_FOV;
	newRender->camera.lookat[X] = 0;
	newRender->camera.lookat[Y] = 0;
	newRender->camera.lookat[Z] = 0;

	newRender->camera.position[X] = DEFAULT_IM_X;
	newRender->camera.position[Y] = DEFAULT_IM_Y;
	newRender->camera.position[Z] = DEFAULT_IM_Z;

	newRender->camera.worldup[X] = 0;
	newRender->camera.worldup[Y] = 1;
	newRender->camera.worldup[Z] = 0;

	newRender->interp_mode = GZ_RGB_COLOR;
	newRender->numlights = 0;
	GzColor Ka = DEFAULT_AMBIENT;
	GzColor Kd = DEFAULT_DIFFUSE;
	GzColor Ks = DEFAULT_SPECULAR;

	//initialize Ka
	memcpy(newRender->Ka, Ka, sizeof(GzColor));
	//initialize Kd
	memcpy(newRender->Kd, Kd, sizeof(GzColor));
	//initialize Ks
	memcpy(newRender->Ks, Ks, sizeof(GzColor));

	newRender->aaXOffset = 0;
	newRender->aaYOffset = 0;

	*render = newRender;

	if (*render == NULL){
		return GZ_FAILURE;
	}

	return GZ_SUCCESS;

}

int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	delete render;
	return GZ_SUCCESS;
}

int GzBeginRender(GzRender *render)
{
/*  
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	if (render == NULL){
		return GZ_FAILURE;
	}

	GzInitDisplay(render->display);

	if (render->display == NULL){
		return GZ_FAILURE;
	}

	//-----------------------------------
	// Build Xpi
	float radians = render->camera.FOV * (PI / 180.0);
	float d = 1 / tan(radians/2);
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (i == j){
				render->camera.Xpi[i][j] = 1.0;
			}
			else{
				render->camera.Xpi[i][j] = 0.0;
			}
		}
	}

	render->camera.Xpi[3][2] = 1 / d;
	render->camera.Xpi[2][2] = 1 / d;
	
	//Compute Xiw
	//Camera z axis
	//Take the vector going from the camera to the lookat point
	GzCoord cl, camZ;
	camZ[X] = render->camera.lookat[X] - render->camera.position[X];
	camZ[Y] = render->camera.lookat[Y] - render->camera.position[Y];
	camZ[Z] = render->camera.lookat[Z] - render->camera.position[Z];
	normalizeVector(camZ);

	//Camera y axis
	//up' = up - (upDotZ)Z
	GzCoord camY;
	float upDotZ = render->camera.worldup[X] * camZ[X] + render->camera.worldup[Y] * camZ[Y] + render->camera.worldup[Z] * camZ[Z];
	camY[X] = render->camera.worldup[X] - upDotZ * camZ[X];
	camY[Y] = render->camera.worldup[Y] - upDotZ * camZ[Y];
	camY[Z] = render->camera.worldup[Z] - upDotZ * camZ[Z];
	normalizeVector(camY);

	//Camera x axis
	//Cross Y with Z;
	GzCoord camX;
	camX[X] = camY[Y] * camZ[Z] - camY[Z] * camZ[Y];
	camX[Y] = camY[Z] * camZ[X] - camY[X] * camZ[Z];
	camX[Z] = camY[X] * camZ[Y] - camY[Y] * camZ[X];
	normalizeVector(camX);

	// Build Xiw
	render->camera.Xiw[0][0] = camX[X];
	render->camera.Xiw[0][1] = camX[Y];
	render->camera.Xiw[0][2] = camX[Z];
	render->camera.Xiw[0][3] = -dotProduct(camX, render->camera.position);

	render->camera.Xiw[1][0] = camY[X];
	render->camera.Xiw[1][1] = camY[Y];
	render->camera.Xiw[1][2] = camY[Z];
	render->camera.Xiw[1][3] = -dotProduct(camY, render->camera.position);

	render->camera.Xiw[2][0] = camZ[X];
	render->camera.Xiw[2][1] = camZ[Y];
	render->camera.Xiw[2][2] = camZ[Z];
	render->camera.Xiw[2][3] = -dotProduct(camZ, render->camera.position);

	render->camera.Xiw[3][0] = 0.0;
	render->camera.Xiw[3][1] = 0.0;
	render->camera.Xiw[3][2] = 0.0;
	render->camera.Xiw[3][3] = 1.0;

	// init Ximage
	// push Xsp
	render->matlevel = -1;
	GzPushMatrix(render, render->Xsp);
	// push Xpi
	GzPushMatrix(render, render->camera.Xpi);
	// push Xiw
	GzPushMatrix(render, render->camera.Xiw);
	//-----------------------------------

	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	if (render == NULL){
		return GZ_FAILURE;
	}
	render->camera.FOV = camera->FOV;

	render->camera.position[X] = camera->position[X];
	render->camera.position[Y] = camera->position[Y];
	render->camera.position[Z] = camera->position[Z];

	render->camera.lookat[X] = camera->lookat[X];
	render->camera.lookat[Y] = camera->lookat[Y];
	render->camera.lookat[Z] = camera->lookat[Z];

	render->camera.worldup[X] = camera->worldup[X];
	render->camera.worldup[Y] = camera->worldup[Y];
	render->camera.worldup[Z] = camera->worldup[Z];
	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if (render == NULL){
		return GZ_FAILURE;
	}
	if (render->matlevel >= MATLEVELS){
		return GZ_FAILURE;
	}

	//Normalize the matrix
	float lenSquared = matrix[0][X]*matrix[0][X] + matrix[0][Y]*matrix[0][Y] + matrix[0][Z]*matrix[0][Z] + matrix[0][3]*matrix[0][3];
	float kInv = fisqrt(lenSquared);
	GzMatrix normMatrix, identityMatrix;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			//Remove any translations from the normal matrix
			normMatrix[i][j] = (i == 3 || j == 3) ? ((i == j) ? 1 : 0) : matrix[i][j] * kInv;
			identityMatrix[i][j] = (i == j) ? 1 : 0;
		}
	}
	
	render->matlevel++;
	if (render->matlevel == 0){
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				render->Ximage[render->matlevel][i][j] = matrix[i][j];
				render->Xnorm[render->matlevel][i][j] = identityMatrix[i][j];
			}
		}
	}

	else{
		matrixMult(render->Ximage[render->matlevel - 1], matrix, render->Ximage[render->matlevel]);
		//Skip pushing Xsp and Xpi matrices onto Xnorm (The first two that get pushed onto the stack.  Push identity instead.)
		if (render->matlevel < 2){
			matrixMult(render->Xnorm[render->matlevel - 1], identityMatrix, render->Xnorm[render->matlevel]);
		}
		else{
			matrixMult(render->Xnorm[render->matlevel - 1], normMatrix, render->Xnorm[render->matlevel]);
		}
	}

	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render == NULL){
		return GZ_FAILURE;
	}
	if (render->matlevel <= 0){
		return GZ_FAILURE;
	}
	render->matlevel--;
	return GZ_SUCCESS;
}

int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList,
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	if (render == NULL)  {
		return GZ_FAILURE;
	}
	if (nameList == NULL) {
		return GZ_FAILURE;
	}
	if (valueList == NULL) {
		return GZ_FAILURE;
	}

	for (int i = 0; i < numAttributes; i++){
		if (nameList[i] == GZ_RGB_COLOR){
			GzColor* cp = (GzColor*)valueList[i];

			render->flatcolor[RED] = (*cp)[RED];
			render->flatcolor[GREEN] = (*cp)[GREEN];
			render->flatcolor[BLUE] = (*cp)[BLUE];
		}
		else if (nameList[i] == GZ_INTERPOLATE){
			int *pInterp = (int*)valueList[i];
			render->interp_mode = (*pInterp);
		}
		else if (nameList[i] == GZ_AMBIENT_LIGHT){
			GzLight *aLight = (GzLight*)valueList[i];
			memcpy(render->ambientlight.color, aLight->color, sizeof(GzColor));
		}
		else if (nameList[i] == GZ_DIRECTIONAL_LIGHT){
			if (render->numlights >= MAX_LIGHTS){
				return GZ_FAILURE;
			}
			GzLight *dLight = (GzLight*)valueList[i];
			memcpy(render->lights[render->numlights].direction, dLight->direction, sizeof(GzCoord));
			memcpy(render->lights[render->numlights].color, dLight->color, sizeof(GzColor));
			render->numlights++;
		}
		else if (nameList[i] == GZ_AMBIENT_COEFFICIENT){
			GzColor *pColor = (GzColor*)valueList[i];
			memcpy(render->Ka, pColor, sizeof(GzColor));
		}
		else if (nameList[i] == GZ_DIFFUSE_COEFFICIENT){
			GzColor *pColor = (GzColor*)valueList[i];
			memcpy(render->Ks, pColor, sizeof(GzColor));
		}
		else if (nameList[i] == GZ_SPECULAR_COEFFICIENT){
			GzColor *pColor = (GzColor*)valueList[i];
			memcpy(render->Ks, pColor, sizeof(GzColor));
		}
		else if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT){
			float *pSpec = (float*)valueList[i];
			render->spec = (*pSpec);
		}
		else if (nameList[i] == GZ_TEXTURE_MAP){
			GzTexture texture = (GzTexture)valueList[i];
			render->tex_fun = texture;
		}
		else if (nameList[i] == GZ_AASHIFTX){
			float* pShiftX = (float*)valueList[i];
			render->aaXOffset = (*pShiftX);
		}
		else if (nameList[i] == GZ_AASHIFTY){
			float* pShiftY = (float*)valueList[i];
			render->aaYOffset = (*pShiftY);
		}
	}
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
	GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
	  GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
	   - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 
	if (render == NULL) {
		return GZ_FAILURE;
	}
	if (nameList == NULL) {
		return GZ_FAILURE;
	}
	if (valueList == NULL) {
		return GZ_FAILURE;
	}

	/*
	Given a triangle's vertices:
	Determine Clockwise or CounterClockwise orientation of the vectors formed from A->B, B->C, and C->A:
	Sort the vertices by Y coordinate
	Use the middle vertex's Y and find what x value on the vector from first vertex -> last vertex gets that Y value.
	Compare that x to the mid vertex's X: x < mid X means edges with the mid vertex are R
	Special case: Vertices with the same Y:
	Top - First and Middle vertex:
	Edges with the vertex with the largest X are R
	*/

	//Transform each vertex from world space to screen space
	GzCoord* xformTri = new GzCoord[3];
	GzCoord* xformNs = new GzCoord[3];
	GzTextureIndex* uvTexCoord = new GzTextureIndex[3];
	bool behindViewPlane = false;
	GzMatrix topMat, topNMat;
	memcpy(topMat, render->Ximage[render->matlevel], sizeof(GzMatrix));
	memcpy(topNMat, render->Xnorm[render->matlevel], sizeof(GzMatrix));

	for (int i = 0; i < numParts; i++){
		//Get the points
		GzCoord* tri = (GzCoord*)valueList[i];
		float W;

		if (nameList[i] == GZ_NULL_TOKEN){
			continue;
		}
		else if (nameList[i] == GZ_POSITION){
			for (int j = 0; j < 3; j++) {
				// xform vertices

				xformTri[j][X] = topMat[0][0] * tri[j][X] + topMat[0][1] * tri[j][Y] + topMat[0][2] * tri[j][Z] + topMat[0][3] * 1.0;
				xformTri[j][Y] = topMat[1][0] * tri[j][X] + topMat[1][1] * tri[j][Y] + topMat[1][2] * tri[j][Z] + topMat[1][3] * 1.0;
				xformTri[j][Z] = topMat[2][0] * tri[j][X] + topMat[2][1] * tri[j][Y] + topMat[2][2] * tri[j][Z] + topMat[2][3] * 1.0;
							 W = topMat[3][0] * tri[j][X] + topMat[3][1] * tri[j][Y] + topMat[3][2] * tri[j][Z] + topMat[3][3] * 1.0;
				xformTri[j][X] /= W;
				xformTri[j][Y] /= W;
				xformTri[j][Z] /= W;

				//Shift x and y by the offset
				xformTri[j][X] += render->aaXOffset;
				xformTri[j][Y] += render->aaYOffset;

				// check if any verts are behind view plane
				if (xformTri[j][Z] < render->camera.position[Z]) {
					behindViewPlane = true;
					continue;
				}
			}
			if (behindViewPlane) {
				continue;
			}
		}
		else if (nameList[i] == GZ_NORMAL){
			GzCoord* pNorm = (GzCoord*)valueList[i];

			for (int j = 0; j < 3; j++){
				// xform normals
				xformNs[j][X] = dotProduct(topNMat[0], pNorm[j]);
				xformNs[j][Y] = dotProduct(topNMat[1], pNorm[j]);
				xformNs[j][Z] = dotProduct(topNMat[2], pNorm[j]);
			}

			// re-normalize the vectors
			normalizeVector(xformNs[0]);
			normalizeVector(xformNs[1]);
			normalizeVector(xformNs[2]);
		}
		else if (nameList[i] == GZ_TEXTURE_INDEX){
			GzTextureIndex* pTexCoord = (GzTextureIndex*)valueList[i];

			for (int j = 0; j < 3; j++){
				// xform uv coords into perspective space
				// do this to avoid divide by 0
				if (xformTri[j][Z] == INT_MAX){
					xformTri[j][Z]--;
				}
				float Vz = xformTri[j][Z] / (INT_MAX - xformTri[j][Z]);
				uvTexCoord[j][U] = pTexCoord[j][U] / (Vz + 1);
				uvTexCoord[j][V] = pTexCoord[j][V] / (Vz + 1);
			}
		}
	}

	if (!behindViewPlane){
		//Sort vertices by decreasing Y value
		int maxY = 0;
		for (int i = 0; i < 2; i++){
			maxY = i;
			for (int j = i + 1; j < 3; j++){
				if (xformTri[maxY][Y] < xformTri[j][Y]){
					maxY = j;
				}
			}
			//Swap
			if (maxY != i){
				swap3(maxY, i, xformTri);
				//NEED TO SWAP NORMALS TOO
				swap3(maxY, i, xformNs);
				//AND THOSE HONERY UV COORDS AS WELL
				swap2(maxY, i, uvTexCoord);
			}
		}

		//Used to determine L/R ownership later
		float midX;
		float v02X;
		//Determine if there is a top or bottom edge
		//Top
		if (xformTri[0][Y] == xformTri[1][Y]){

			//Going Clockwise, so swap if xformTri[0][X] > xformTri[1][X]
			if (xformTri[0][X] < xformTri[1][X]){
				swap3(0, 1, xformTri);
				swap3(0, 1, xformNs);
				swap2(0, 1, uvTexCoord);
			}
		}
		//Bottom
		else if (xformTri[1][Y] == xformTri[2][Y]){
			//Going Clockwise, so swap if xformTri[1][X] < xformTri[2][X]
			if (xformTri[1][X] > xformTri[2][X]){
				swap3(1, 2, xformTri);
				swap3(1, 2, xformNs);
				swap2(1, 2, uvTexCoord);
			}
		}
		//If it's not a top or bottom edge, determine L/R edges
		else{
			//Rearrange vertices to ensure clockwise orientation:
			//Look at the mid vertex
			midX = xformTri[1][X];
			//Construct a line equation for a line from vertex 0 -> 2
			//  Ax + By + C = 0
			//	A = y0 - y2
			//	B = x2 - x0
			//  C = -Ax - By
			//  x = (-C - By) / A
			float A = xformTri[0][Y] - xformTri[2][Y];
			float B = xformTri[2][X] - xformTri[0][X];
			float C = -A * xformTri[2][X] - B * xformTri[2][Y];

			//Plug in the Y from the mid vertex and solve for X
			v02X = (-C - B * xformTri[1][Y]) / A;
			
			//Compare the x of the mid vertex to this X
			//If mid vertex x < this X, swap
			if (midX > v02X){
				swap3(1, 2, xformTri);
				swap3(1, 2, xformNs);
				swap2(1, 2, uvTexCoord);
			}
		}

		int boxTop = 0, boxBottom = 0, boxLeft = 0, boxRight = 0;

		//Find the bounding box
		boxTop = (int)ceil(max(max(xformTri[0][Y], xformTri[1][Y]), xformTri[2][Y]));
		boxBottom = (int)floor(min(min(xformTri[0][Y], xformTri[1][Y]), xformTri[2][Y]));
		boxLeft = (int)floor(min(min(xformTri[0][X], xformTri[1][X]), xformTri[2][X]));
		boxRight = (int)ceil(max(max(xformTri[0][X], xformTri[1][X]), xformTri[2][X]));


		//mipmap
		float ydist = xformTri[0][Y] - xformTri[2][Y];
		
		float UYMax = uvTexCoord[0][U];
		float VYMax = uvTexCoord[0][V];

		float UYMin = uvTexCoord[2][U];
		float VYMin = uvTexCoord[2][V];

		float maxX, minX, maxZ,minZ;
		maxX = xformTri[0][X];
		minX = xformTri[0][X];
		maxZ = xformTri[0][Z];
		minZ = xformTri[0][Z];
		int min = 0;
		int max = 0;

		for (int p = 1; p < 3; p++)
		{
			if (maxX < xformTri[p][X])
			{
				maxX = xformTri[p][X];
				max = p;
			}
			if (minX > xformTri[p][X])
			{
				minX = xformTri[p][X];
				min = p;
			}
			if (maxZ < xformTri[p][Z])
				maxZ = xformTri[p][Z];
			if (minZ > xformTri[p][Z])
				minZ = xformTri[p][Z];
		}

		float xdist = maxX-minX;

		float UXMax = uvTexCoord[max][U];
		float VXMax = uvTexCoord[max][V];

		float UXMin = uvTexCoord[min][U];
		float VXMin = uvTexCoord[min][V];

		float dudx = (UXMax - UXMin) / xdist;
		float dvdy = (VYMax - VYMin) / ydist;

		float final = max((dvdy*dvdy), (dudx*dudx));

		
		
		//end


		//For interpolating Z later
		//Ax + By + Cz + D = 0
		//xformTri[0] cross xformTri[1] = (A, B, C)
		//Get the vectors centered at the origin
		GzCoord e0to1;
		e0to1[X] = xformTri[1][X] - xformTri[0][X];
		e0to1[Y] = xformTri[1][Y] - xformTri[0][Y];
		e0to1[Z] = xformTri[1][Z] - xformTri[0][Z];

		GzCoord e2to0;
		e2to0[X] = xformTri[0][X] - xformTri[2][X];
		e2to0[Y] = xformTri[0][Y] - xformTri[2][Y];
		e2to0[Z] = xformTri[0][Z] - xformTri[2][Z];

		float A = e0to1[Y] * e2to0[Z] - e2to0[Y] * e0to1[Z];
		float B = -(e0to1[X] * e2to0[Z] - e2to0[X] * e0to1[Z]);
		float C = e0to1[X] * e2to0[Y] - e2to0[X] * e0to1[Y];

		float D = -(A*xformTri[0][X]) - (B*xformTri[0][Y]) - (C*xformTri[0][Z]);

		//Get the shaded color at each vertex
		GzColor v0Color, v1Color, v2Color;
		if (render->interp_mode == GZ_COLOR && render->tex_fun != NULL){
			shadingEq(render, v0Color, xformNs[0], true);
			shadingEq(render, v1Color, xformNs[1], true);
			shadingEq(render, v2Color, xformNs[2], true);
		}
		else{
			shadingEq(render, v0Color, xformNs[0], false);
			shadingEq(render, v1Color, xformNs[1], false);
			shadingEq(render, v2Color, xformNs[2], false);
		}

		//Get the area of the triangle formed by the vertices on the screen
		float areaOfTri = triArea(xformTri[0], xformTri[1], xformTri[2]);

		float iZ;

		//Check the pixels within the bounding box

		for (int i = boxLeft; i < boxRight; i++){
			//Check that we're still within the bounds
			if (i < 0 || i > render->display->xres){
				continue;
			}
			for (int j = boxBottom; j < boxTop; j++){
				//Check that we're still within the bounds
				if (j < 0 || j > render->display->yres){
					continue;
				}
				//Interpolate Z at this coordinate
				//Ax + By + Cz + D = 0
				//z = -(Ax + By + D) / C
				iZ = -(A*i + B*j + D) / C;

				//Get current Z and color information at this pixel 
				GzIntensity r, g, b, a;
				GzCoord pix = { i, j, 0 };

				GzDepth z;
				GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);

				//If iZ <= current z and not negative, the point could potentially be drawn
				if (iZ <= z && iZ > 0){
					//Compute LEE
					//E(x,y) = dY(x-X) - dX(y-Y)
					float dY, dX;
					//E 0 -> 2
					dY = xformTri[2][Y] - xformTri[0][Y];
					dX = xformTri[2][X] - xformTri[0][X];
					float e02 = dY * ((float)i - xformTri[2][X]) - dX * ((float)j - xformTri[2][Y]);

					//E 2 -> 1
					dY = xformTri[1][Y] - xformTri[2][Y];
					dX = xformTri[1][X] - xformTri[2][X];
					float e21 = dY * ((float)i - xformTri[1][X]) - dX * ((float)j - xformTri[1][Y]);

					//E 1 -> 0
					dY = xformTri[0][Y] - xformTri[1][Y];
					dX = xformTri[0][X] - xformTri[1][X];
					float e10 = dY * ((float)i - xformTri[0][X]) - dX * ((float)j - xformTri[0][Y]);

					//If all have the same sign, then draw the pixel
					//  Also only draw edge pixels that are on the Left and Bottom edges
					if ((
						(e02 == 0) || (e21 == 0 && midX < v02X)				//Left edge pixels
						|| (e10 == 0 && xformTri[0][Y] == xformTri[1][Y])	//Bottom edge pixels
						|| (e02 > 0 && e21 > 0 && e10 > 0)	)				//All pixels in the middle have the same sign
						){

						float a0 = triArea(pix, xformTri[1], xformTri[2]);
						float a1 = triArea(xformTri[0], pix, xformTri[2]);
						float a2 = triArea(xformTri[0], xformTri[1], pix);

						//Interpolate uv
						GzTextureIndex uv;
						if (iZ == INT_MAX){
							iZ--;
						}
						float Vz = iZ / (INT_MAX - iZ);
						uv[U] = ((a0*uvTexCoord[0][U] + a1*uvTexCoord[1][U] + a2*uvTexCoord[2][U]) / areaOfTri) * (Vz + 1);
						uv[V] = ((a0*uvTexCoord[0][V] + a1*uvTexCoord[1][V] + a2*uvTexCoord[2][V]) / areaOfTri) * (Vz + 1);

						//mipmap trilinear
						float level = log2(final);
						if (level<-21 && level >-30)
							level = -21;
						level += 21;
						int level1 = floor(level);
						if (level1 == 5)
							int check = 0;
						int level2 = ceil(level);
						level -= level1;
						float interp = iZ - minZ;
						level = interp / (maxZ - minZ);
						if (level1 < 0 || level2 > 9){
							level1 = 8;
							level2 = 9;
						}

						//mipmap anisotropic
						GzCoord pix1 = { i - .5, j - .5, 0 };
						GzCoord pix2 = { i - .5, j + .5, 0 };
						GzCoord pix3 = { i + .5, j - .5, 0 };
						GzCoord pix4 = { i + .5, j + .5, 0 };

						//xy
						float a11 = triArea(pix1, xformTri[1], xformTri[2]);
						float a12 = triArea(xformTri[0], pix1, xformTri[2]);
						float a13 = triArea(xformTri[0], xformTri[1], pix1);
						
						float z1 = (a11*xformTri[0][Z] + a12*xformTri[1][Z] + a13*xformTri[2][Z]) / areaOfTri;

						if (z1 == INT_MAX){
							z1--;
						}
						float Vz1 = z1 / (INT_MAX - z1);
						GzTextureIndex uv1;

						uv1[U] = ((a11*uvTexCoord[0][U] + a12*uvTexCoord[1][U] + a13*uvTexCoord[2][U]) / areaOfTri) * (Vz1 + 1);
						uv1[V] = ((a11*uvTexCoord[0][V] + a12*uvTexCoord[1][V] + a13*uvTexCoord[2][V]) / areaOfTri) * (Vz1 + 1);

						//x,y+1
						float a21 = triArea(pix2, xformTri[1], xformTri[2]);
						float a22 = triArea(xformTri[0], pix2, xformTri[2]);
						float a23 = triArea(xformTri[0], xformTri[1], pix2);

						float z2 = (a21*xformTri[0][Z] + a22*xformTri[1][Z] + a23*xformTri[2][Z]) / areaOfTri;

						if (z2 == INT_MAX){
							z2--;
						}
						float Vz2 = z2 / (INT_MAX - z2);
						GzTextureIndex uv2;

						uv2[U] = ((a21*uvTexCoord[0][U] + a22*uvTexCoord[1][U] + a23*uvTexCoord[2][U]) / areaOfTri) * (Vz2 + 1);
						uv2[V] = ((a21*uvTexCoord[0][V] + a22*uvTexCoord[1][V] + a23*uvTexCoord[2][V]) / areaOfTri) * (Vz2 + 1);

						//x+1,y
						float a31 = triArea(pix3, xformTri[1], xformTri[2]);
						float a32 = triArea(xformTri[0], pix3, xformTri[2]);
						float a33 = triArea(xformTri[0], xformTri[1], pix3);

						float z3 = (a31*xformTri[0][Z] + a32*xformTri[1][Z] + a33*xformTri[2][Z]) / areaOfTri;

						if (z3 == INT_MAX){
							z3--;
						}
						float Vz3 = z3 / (INT_MAX - z3);
						GzTextureIndex uv3;

						uv3[U] = ((a31*uvTexCoord[0][U] + a32*uvTexCoord[1][U] + a33*uvTexCoord[2][U]) / areaOfTri) * (Vz3 + 1);
						uv3[V] = ((a31*uvTexCoord[0][V] + a32*uvTexCoord[1][V] + a33*uvTexCoord[2][V]) / areaOfTri) * (Vz3 + 1);

						//x+1,y+1
						float a41 = triArea(pix4, xformTri[1], xformTri[2]);
						float a42 = triArea(xformTri[0], pix4, xformTri[2]);
						float a43 = triArea(xformTri[0], xformTri[1], pix4);

						float z4 = (a41*xformTri[0][Z] + a42*xformTri[1][Z] + a43*xformTri[2][Z]) / areaOfTri;

						if (z4 == INT_MAX){
							z4--;
						}
						float Vz4 = z4 / (INT_MAX - z4);
						GzTextureIndex uv4;

						uv4[U] = ((a41*uvTexCoord[0][U] + a42*uvTexCoord[1][U] + a43*uvTexCoord[2][U]) / areaOfTri) * (Vz4 + 1);
						uv4[V] = ((a41*uvTexCoord[0][V] + a42*uvTexCoord[1][V] + a43*uvTexCoord[2][V]) / areaOfTri) * (Vz4 + 1);


						float r1 = sqrt(pow(uv3[U] - uv1[U], 2) + pow(uv3[V] - uv1[V], 2));
						float r2 = sqrt(pow(uv2[U] - uv1[U], 2) + pow(uv2[V] - uv1[V], 2));

						float d1 = sqrt(pow(uv4[U] - uv1[U], 2) + pow(uv4[V] - uv1[V], 2));
						float d2 = sqrt(pow(uv3[U] - uv2[U], 2) + pow(uv3[V] - uv2[V], 2));

						float minaxis = min(r1, r2, d1, d2);

						float levelai = floor(log2(minaxis));

						//end



						//Get the texture color at this uv
						GzColor texColor;
						if (render->tex_fun != NULL){
							render->tex_fun(uv[U], uv[V], texColor, level1, level2, level);
						}

						//Gouraud Shading - Interpolate Color
						if (render->interp_mode == GZ_COLOR){
							//Interpolate the color
							float rGouraud = (a0*v0Color[RED] + a1*v1Color[RED] + a2*v2Color[RED]) / areaOfTri;
							float gGouraud = (a0*v0Color[GREEN] + a1*v1Color[GREEN] + a2*v2Color[GREEN]) / areaOfTri;
							float bGouraud = (a0*v0Color[BLUE] + a1*v1Color[BLUE] + a2*v2Color[BLUE]) / areaOfTri;

							//Texture code
							if (render->tex_fun != NULL){
								rGouraud *= texColor[RED];
								gGouraud *= texColor[GREEN];
								bGouraud *= texColor[BLUE];
							}

							//Gate the color to 1.0
							r = (rGouraud > 1.0f) ? (GzIntensity)ctoi(1.0f) : (GzIntensity)ctoi(rGouraud);
							g = (gGouraud > 1.0f) ? (GzIntensity)ctoi(1.0f) : (GzIntensity)ctoi(gGouraud);
							b = (bGouraud > 1.0f) ? (GzIntensity)ctoi(1.0f) : (GzIntensity)ctoi(bGouraud);
						}
						//Phong Shading - Interpolate Normals
						else if (render->interp_mode == GZ_NORMALS){
							GzCoord pN;
							pN[X] = (a0*xformNs[0][X] + a1*xformNs[1][X] + a2*xformNs[2][X]) / areaOfTri;
							pN[Y] = (a0*xformNs[0][Y] + a1*xformNs[1][Y] + a2*xformNs[2][Y]) / areaOfTri;
							pN[Z] = (a0*xformNs[0][Z] + a1*xformNs[1][Z] + a2*xformNs[2][Z]) / areaOfTri;
							normalizeVector(pN);

							//Texture code
							if (render->tex_fun != NULL){
								render->Ka[RED] = render->Kd[RED] = texColor[RED];
								render->Ka[GREEN] = render->Kd[GREEN] = texColor[GREEN];
								render->Ka[BLUE] = render->Kd[BLUE] = texColor[BLUE];
							}

							//Calculate color
							GzColor cPhong;
							shadingEq(render, cPhong, pN, false);

							r = (GzIntensity)ctoi(cPhong[RED]);
							g = (GzIntensity)ctoi(cPhong[GREEN]);
							b = (GzIntensity)ctoi(cPhong[BLUE]);
						}
						//Flat Shading
						else if (render->interp_mode == GZ_FLAT){
							r = (GzIntensity)ctoi((float)((v0Color[RED] + v1Color[RED] + v2Color[RED]) / 3));
							g = (GzIntensity)ctoi((float)((v0Color[GREEN] + v1Color[GREEN] + v2Color[GREEN]) / 3));
							b = (GzIntensity)ctoi((float)((v0Color[BLUE] + v1Color[BLUE] + v2Color[BLUE]) / 3));
						}
						else {
							r = (GzIntensity)ctoi((float)render->flatcolor[RED]);
							g = (GzIntensity)ctoi((float)render->flatcolor[GREEN]);
							b = (GzIntensity)ctoi((float)render->flatcolor[BLUE]);
						}
						z = (int)iZ;
						GzPutDisplay(render->display, i, j, r, g, b, a, z);
					}
				}
			}
		}
	}
	//Free up memory
	delete[] xformTri;
	delete[] xformNs;
	delete[] uvTexCoord;

	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

void swap3(int i, int j, float triArray[][3]){
	float tempX = triArray[j][X];
	float tempY = triArray[j][Y];
	float tempZ = triArray[j][Z];

	triArray[j][X] = triArray[i][X];
	triArray[j][Y] = triArray[i][Y];
	triArray[j][Z] = triArray[i][Z];

	triArray[i][X] = tempX;
	triArray[i][Y] = tempY;
	triArray[i][Z] = tempZ;
}

void swap2(int i, int j, float coordArray[][2]){
	float tempU = coordArray[j][U];
	float tempV = coordArray[j][V];

	coordArray[j][U] = coordArray[i][U];
	coordArray[j][V] = coordArray[i][V];

	coordArray[i][U] = tempU;
	coordArray[i][V] = tempV;
}

void matrixMult(GzMatrix mat1, GzMatrix mat2, GzMatrix matOut){
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matOut[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				matOut[i][j] += mat1[i][k] * mat2[k][j];
			}
		}
	}
}

int normalizeVector(GzCoord v) {
	//Get the length of the vector squared
	float lenSquared = (v[X] * v[X]) + (v[Y] * v[Y]) + (v[Z] * v[Z]);
	//Find the fast inverse square root (close approximation w/o using sqrt)
	float invsqrt = fisqrt(lenSquared);

	v[X] *= invsqrt;
	v[Y] *= invsqrt;
	v[Z] *= invsqrt;

	return GZ_SUCCESS;
}

//Method used by Quake 3 guys to quickly normalize vectors without using sqrt
float fisqrt(float num)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = num * 0.5F;
	y = num;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));

	return y;
}

float dotProduct(GzCoord v1, GzCoord v2){
	return v1[X] * v2[X]
		 + v1[Y] * v2[Y]
		 + v1[Z] * v2[Z];
}

float triArea(GzCoord A, GzCoord B, GzCoord C){
	return abs(.5 * (A[X] * (B[Y] - C[Y]) + B[X] * (C[Y] - A[Y]) + C[X] * (A[Y] - B[Y])));
}

int	shadingEq(GzRender* render, GzColor color, GzCoord norm, bool gouraudTexture){
	
	//Compute the color at a vertex
	//Make sure the normal is normalized
	normalizeVector(norm);

	GzCoord E = { 0, 0, -1 };

	//Compute R
	GzCoord* R = new GzCoord[render->numlights];
	//Compute N.L and N.E:
	//  Both positive, compute
	//  Both negative, flip normal and compute
	//  Different sign, light and eye are on opposite sides of surface. Skip it
	float NdotL, NdotE;
	NdotE = dotProduct(norm, E);

	GzColor specLightSum = { 0, 0, 0 };
	GzColor diffLightSum = { 0, 0, 0 };

	//Calculate the Specular and Diffuse components for each light source (not ambient)
	for (int i = 0; i < render->numlights; i++){
		NdotL = dotProduct(norm, render->lights[i].direction);
		//If they have different signs, skip it
		if (NdotL * NdotE < 0){
			continue;
		}
		else if (NdotL < 0 && NdotE < 0){
			//Flip the normal

			GzCoord negNorm = { -norm[X], -norm[Y], -norm[Z] };
			NdotL = dotProduct(negNorm, render->lights[i].direction);
		}

		//Compute R
		R[i][X] = 2 * NdotL * norm[X] - render->lights[i].direction[X];
		R[i][Y] = 2 * NdotL * norm[Y] - render->lights[i].direction[Y];
		R[i][Z] = 2 * NdotL * norm[Z] - render->lights[i].direction[Z];
		normalizeVector(R[i]);

		//Sum the specular lights
		float RdotE = dotProduct(R[i], E);
		RdotE = (RdotE < 0) ? 0 : ((RdotE > 1) ? 1 : RdotE);
		specLightSum[RED] += render->lights[i].color[RED] * pow(RdotE, render->spec);
		specLightSum[GREEN] += render->lights[i].color[GREEN] * pow(RdotE, render->spec);
		specLightSum[BLUE] += render->lights[i].color[BLUE] * pow(RdotE, render->spec);

		//Sum the diffuse lights
		diffLightSum[RED] += render->lights[i].color[RED] * NdotL;
		diffLightSum[GREEN] += render->lights[i].color[GREEN] * NdotL;
		diffLightSum[BLUE] += render->lights[i].color[BLUE] * NdotL;
	}

	//If this is Gouraud Shading with a texture, delay applying Ks, Kd, and Ka
	GzColor colorSum = { 0, 0, 0 };

	if (gouraudTexture){
		colorSum[RED] = specLightSum[RED] + diffLightSum[RED] + render->ambientlight.color[RED];
		colorSum[GREEN] = specLightSum[GREEN] + diffLightSum[GREEN] + render->ambientlight.color[GREEN];
		colorSum[BLUE] = specLightSum[BLUE] + diffLightSum[BLUE] + render->ambientlight.color[BLUE];
	}
	else{

		GzColor specularComponent, diffuseComponent, ambientComponent;

		specularComponent[RED] = render->Ks[RED] * specLightSum[RED];
		specularComponent[GREEN] = render->Ks[GREEN] * specLightSum[GREEN];
		specularComponent[BLUE] = render->Ks[BLUE] * specLightSum[BLUE];

		diffuseComponent[RED] = render->Kd[RED] * diffLightSum[RED];
		diffuseComponent[GREEN] = render->Kd[GREEN] * diffLightSum[GREEN];
		diffuseComponent[BLUE] = render->Kd[BLUE] * diffLightSum[BLUE];

		ambientComponent[RED] = render->Ka[RED] * render->ambientlight.color[RED];
		ambientComponent[GREEN] = render->Ka[GREEN] * render->ambientlight.color[GREEN];
		ambientComponent[BLUE] = render->Ka[BLUE] * render->ambientlight.color[BLUE];

		colorSum[RED] = specularComponent[RED] + diffuseComponent[RED] + ambientComponent[RED];
		colorSum[GREEN] = specularComponent[GREEN] + diffuseComponent[GREEN] + ambientComponent[GREEN];
		colorSum[BLUE] = specularComponent[BLUE] + diffuseComponent[BLUE] + ambientComponent[BLUE];
	}

	//Gate the color value
	color[RED] = (colorSum[RED] > 1.0f) ? 1.0f : ((colorSum[RED] < 0.0f) ? 0.0f : colorSum[RED]);
	color[GREEN] = (colorSum[GREEN] > 1.0f) ? 1.0f : ((colorSum[GREEN] < 0.0f) ? 0.0f : colorSum[GREEN]);
	color[BLUE] = (colorSum[BLUE] > 1.0f) ? 1.0f : ((colorSum[BLUE] < 0.0f) ? 0.0f : colorSum[BLUE]);

	//Free up memory
	delete[] R;
	return GZ_SUCCESS;
}

/*
CString str;
str.Format(_T("Before Sort:\n1: %f 2: %f 3: %f"), tri[0][Y], tri[1][Y], tri[2][Y]);
AfxMessageBox(str);
*/
