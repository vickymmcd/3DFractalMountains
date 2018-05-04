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


float *terrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3) {

	float *auxNormal,v1[3],v2[3];
		
	v1[0] = x2-x1; 
	v1[1] = -terrainHeights[z1 * terrainGridWidth + x1] 
			+ terrainHeights[z2 * terrainGridWidth + x2];
	v1[2] = z2-z1; 


	v2[0] = x3-x1; 
	v2[1] = -terrainHeights[z1 * terrainGridWidth + x1] 
			+ terrainHeights[z3 * terrainGridWidth + x3];
	v2[2] = z3-z1; 

	auxNormal = (float *)malloc(sizeof(float)*3);

	auxNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];
	auxNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
	auxNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];

	return(auxNormal);
}

void terrainNormalize(float *v) {

	double d,x,y,z;

	x = v[0];
	y = v[1];
	z = v[2];
	
	d = sqrt((x*x) + (y*y) + (z*z));

	v[0] = x / d;
	v[1] = y / d;
	v[2] = z / d;
}

void terrainAddVector(float *a, float *b) {

	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void terrainComputeNormals() {

	float *norm1,*norm2,*norm3,*norm4; 
	int i,j,k;
	
	if (terrainNormals == NULL)
		return;

	for(i = 0; i < terrainGridLength; i++)
		for(j = 0; j < terrainGridWidth; j++) {
			norm1 = NULL;
			norm2 = NULL;
			norm3 = NULL;
			norm4 = NULL;

			// normals for the four corners
			if (i == 0 && j == 0) {
				norm1 = terrainCrossProduct(0,0, 0,1, 1,0);	
				terrainNormalize(norm1);				
			}
			else if (j == terrainGridWidth-1 && i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(i,j, j,i-1, j-1,i);	
				terrainNormalize(norm1);				
			}
			else if (j == 0 && i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(i,j, j,i-1, j+1,i);	
				terrainNormalize(norm1);				
			}
			else if (j == terrainGridWidth-1 && i == 0) {
				norm1 = terrainCrossProduct(i,j, j,i+1, j-1,i);	
				terrainNormalize(norm1);				
			}

			// normals for the borders
			else if (i == 0) {
				norm1 = terrainCrossProduct(j,0, j-1,0, j,1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,0,j,1,j+1,0);
				terrainNormalize(norm2);
			}
			else if (j == 0) {
				norm1 = terrainCrossProduct(0,i, 1,i, 0,i-1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(0,i, 0,i+1, 1,i);
				terrainNormalize(norm2);
			}
			else if (i == terrainGridLength) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j+1,i);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j+1,i, j,i-1);
				terrainNormalize(norm2);
			}
			else if (j == terrainGridWidth) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j-1,i);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j-1,i, j,i+1);
				terrainNormalize(norm2);
			}

			// normals for the interior
			else {
				norm1 = terrainCrossProduct(j,i, j-1,i, j,i+1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j,i+1, j+1,i);
				terrainNormalize(norm2);
				norm3 = terrainCrossProduct(j,i, j+1,i, j,i-1);
				terrainNormalize(norm3);
				norm4 = terrainCrossProduct(j,i, j,i-1, j-1,i);
				terrainNormalize(norm4);
			}
			if (norm2 != NULL) {
				terrainAddVector(norm1,norm2);
				free(norm2);
			}
			if (norm3 != NULL) {
				terrainAddVector(norm1,norm3);
				free(norm3);
			}
			if (norm4 != NULL) {
				terrainAddVector(norm1,norm4);
				free(norm4);
			}
			terrainNormalize(norm1);
			norm1[2] = - norm1[2];
			for (k = 0; k< 3; k++) 
				terrainNormals[3*(i*terrainGridWidth + j) + k] = norm1[k];
			free(norm1);
		}

}

int terrainLoadFromHeightmap(int size, int normals) {

	tgaInfo *info;
	Matrix *heightmap;
	int mode;
	float pointHeight;
	
// if a terrain already exists, destroy it.
	if (terrainHeights != NULL)
		terrainDestroy();

	//define heightmap
	heightmap = make_mountain(size,3);

	//set heightmap to grayscale
	mode = 1;

// set the width and height of the terrain
	terrainGridWidth = heightmap->num_cols;
	terrainGridLength = heightmap->num_rows;


// alocate memory for the terrain, and check for errors
	terrainHeights = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float));
	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);

// allocate memory for the normals, and check for errors
	if (normals) {
		terrainNormals = (float *)malloc(terrainGridWidth * 
						terrainGridLength * 
						sizeof(float) * 3);
		if (terrainNormals == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
			terrainNormals = NULL;

// fill arrays
	for (int i = 0 ; i < terrainGridLength; i++)
		for (int j = 0;j < terrainGridWidth; j++) {
// compute the height as a value between 0.0 and 1.0
			pointHeight = (float)(heightmap->rows[i][j]);  
			terrainHeights[i*terrainGridWidth + j] = (float)pointHeight;
		}

	for (int i = (terrainGridLength*terrainGridWidth) - 1; i >= 0; i--) 
  	printf("%lf ", terrainHeights[i]);
// free the image's memory 
	if (normals)
		terrainComputeNormals();
	tgaDestroy(info);
	
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
	if (terrainNormals != NULL && terrainColors != NULL) {
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
	}

	for (i = 0 ; i < terrainGridLength-1; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0;j < terrainGridWidth; j++) {
		
			if (terrainColors != NULL) 
				glColor3f(terrainColors[3*((i+1)*terrainGridWidth + j)],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+1],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+2]);
			if (terrainNormals != NULL)
				glNormal3f(terrainNormals[3*((i+1)*terrainGridWidth + j)],
						  terrainNormals[3*((i+1)*terrainGridWidth + j)+1],
						  terrainNormals[3*((i+1)*terrainGridWidth + j)+2]);
			glVertex3f(
				startW + j + xOffset,
				terrainHeights[(i+1)*terrainGridWidth + (j)] + yOffset,
				startL - (i+1) + zOffset);

			if (terrainColors != NULL) 
				glColor3f(terrainColors[3*(i*terrainGridWidth + j)],
						  terrainColors[3*(i*terrainGridWidth + j)+1],
						  terrainColors[3*(i*terrainGridWidth + j)+2]);
			if (terrainNormals != NULL)
				glNormal3f(terrainNormals[3*(i*terrainGridWidth + j)],
						  terrainNormals[3*(i*terrainGridWidth + j)+1],
						  terrainNormals[3*(i*terrainGridWidth + j)+2]);
			glVertex3f(
				startW + j + xOffset, 
				terrainHeights[(i)*terrainGridWidth + j] + yOffset,
				startL - i + zOffset);					
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