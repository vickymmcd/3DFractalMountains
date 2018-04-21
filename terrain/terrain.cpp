#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include "tga.h"
#include "terrain.h"
#include "finaltry.h"

static int terrainGridWidth ,terrainGridLength;
static float *terrainHeights = NULL;
static float *terrainColors = NULL;
static float *terrainNormals = NULL;

int terrainLoadFromImage(char *filename, int normals) {

	tgaInfo *info;
	int mode;
	float pointHeight;

// if a terrain already exists, destroy it.
	if (terrainHeights != NULL)
		terrainDestroy();
		
// load the image, using the tgalib
	info = tgaLoad(filename);
// check to see if the image was properly loaded
// remember: only greyscale, RGB or RGBA noncompressed images
	if (info->status != TGA_OK)
		return(TERRAIN_ERROR_LOADING_IMAGE);

//really hoping the image is just grayscale to begin with
// if the image is RGB, convert it to grayscale
// mode will store the image's number of components
	mode = info->pixelDepth / 8;
	if (mode == 3) {
		tgaRGBtoGreyscale(info);
		mode = 1;
	}
	
// set the width and height of the terrain
	terrainGridWidth = info->width;
	terrainGridLength = info->height;

// alocate memory for the terrain, and check for errors
	terrainHeights = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float));
	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);

//don't know what normals are so none of that...
	terrainNormals = NULL;

//only grayscale images allowed for now
	terrainColors = NULL;

// fill arrays
	for (int i = 0 ; i < terrainGridLength; i++)
		for (int j = 0;j < terrainGridWidth; j++) {
// compute the height as a value between 0.0 and 1.0
			pointHeight = info->heightMapData[mode*(i*terrainGridWidth + j)+(mode-1)] / 256.0;
			terrainHeights[i*terrainGridWidth + j] = pointHeight;
		}

// free the image's memory 
	tgaDestroy(info);
	
	return(TERRAIN_OK); 
}


int terrainLoadFromHeightmap(int size) {

	tgaInfo *info;
	Matrix *heightmap;
	int mode;
	float pointHeight;

	//define heightmap
	heightmap = make_mountain(size);
	printf("matrix");
	print_matrix(heightmap);

// set the width and height of the terrain
	terrainGridWidth = heightmap->num_cols;
	terrainGridLength = heightmap->num_rows;

// alocate memory for the terrain, and check for errors
	terrainHeights = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float));
	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);

//don't know what normals are so none of that...
	terrainNormals = NULL;

//only grayscale images allowed for now
	terrainColors = NULL;

// fill arrays
	for (int i = 0 ; i < terrainGridLength; i++)
		for (int j = 0;j < terrainGridWidth; j++) {
// compute the height as a value between 0.0 and 1.0
			pointHeight = heightmap->rows[i][j]/500;    
			terrainHeights[i*terrainGridWidth + j] = pointHeight;
		}

	for (int i = (terrainGridLength*terrainGridWidth) - 1; i >= 0; i--) 
  	printf("%d ", terrainHeights[i]);
// free the image's memory 
	tgaDestroy(info);
	
	return(TERRAIN_OK); 
}


int terrainScale(float min,float max) {

	float amp,aux,min1,max1,height;
	int total,i;

	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_NOT_INITIALISED);

	if (min > max) {
		aux = min;
		min = max;
		max = aux;
	}

	amp = max - min;
	total = terrainGridWidth * terrainGridLength;

	min1 = terrainHeights[0];
	max1 = terrainHeights[0];
	for(i=1;i < total ; i++) {
		if (terrainHeights[i] > max1)
			max1 = terrainHeights[i];
		if (terrainHeights[i] < min1)
			min1 = terrainHeights[i];
	}
	for(i=0;i < total; i++) {
		height = (terrainHeights[i] - min1) / (max1-min1);
		terrainHeights[i] = height * amp - min;
	}
	printf("bla %f %f %f %f %f %f\n",min,max,min1,max1,amp,height);
	return(TERRAIN_OK);
}


int terrainCreateDL(float xOffset, float yOffset, float zOffset) {

	GLuint terrainDL;
	float startW,startL;
	int i,j;

	startW = terrainGridWidth / 2.0 - terrainGridWidth;
	startL = - terrainGridLength / 2.0 + terrainGridLength;

	terrainDL = glGenLists(1);

	glNewList(terrainDL,GL_COMPILE);


	for (i = 0 ; i < terrainGridLength-1; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0;j < terrainGridWidth; j++) {
		
			glVertex3f(
				startW + j + xOffset,
				terrainHeights[(i+1)*terrainGridWidth + (j)] + yOffset, startL - (i+1) + zOffset);

			glVertex3f(
				startW + j + xOffset, 
				terrainHeights[(i)*terrainGridWidth + j] + yOffset, startL - i + zOffset);					
		}
		glEnd();
	}
	glEndList();
	return(terrainDL);
}

float terrainGetHeight(int x, int z) {

	int xt,zt;

	if (terrainHeights == NULL) 
			return(0.0);

	xt = x + terrainGridWidth /2;
	zt = terrainGridWidth - (z + terrainGridLength /2);

	if ((xt > terrainGridWidth) || (zt > terrainGridLength) || (xt < 0) || (zt < 0))
		return(0.0);

	return(terrainHeights[zt * terrainGridWidth + xt]);
}


void terrainDestroy() {

	if (terrainHeights != NULL)
		free(terrainHeights);

	if (terrainColors != NULL)
		free(terrainColors);

	if (terrainNormals != NULL)
		free(terrainNormals);
}