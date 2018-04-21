#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>


//void srand(int seed);

typedef struct {
    int num_rows, num_cols;
    double **rows;
} Matrix;

/* Make a matrix.
*/
Matrix *make_matrix(int num_rows, int num_cols) {
    Matrix *matrix = (Matrix*)malloc(sizeof(Matrix));
    matrix->num_rows = num_rows;
    matrix->num_cols = num_cols;
    matrix->rows = (double**)malloc(num_rows * sizeof(double*));
    for (int i=0; i<num_rows; i++) {
        matrix->rows[i] = (double*)calloc(num_cols, sizeof(double));
    }
    return matrix;
}

/* Free a matrix.
*/
void free_matrix(Matrix *matrix) {
    for (int i=0; i<matrix->num_rows; i++) {
        free(matrix->rows[i]);
    }
    free(matrix->rows);
    free(matrix);
}

/* Print a row of a matrix.
*/
void print_matrix_row(double *row, int num_cols) {
    for (int j=0; j<num_cols; j++) {
        printf("%f ", row[j]);
    }
}

/* Print a matrix.
*/
void print_matrix(Matrix *matrix) {
    for (int i=0; i<matrix->num_rows; i++) {

        print_matrix_row(matrix->rows[i], matrix->num_cols);
        printf("\n");
    }
}


int random_int(int min, int max)
{
   return (rand() + min) % (max + 1);
   //return min + rand() % (max+1 - min);
}

double indiv_smoothing(Matrix *matrix, int row, int col){
	int size = matrix->num_cols;
	double sum =0;
	double nAverage = 0;
	if (row+1<size){
		if (col+1<size){
			nAverage+=1;
			sum += matrix->rows[row+1][col+1];
		}
		if (col-1>=0){
			nAverage+=1;
			sum += matrix->rows[row+1][col-1];
		}
	}
	if (row-1>=0){
		if (col+1<size){
			nAverage+=1;
			sum += matrix->rows[row-1][col+1];
		}
		if (col-1>=0){
			nAverage+=1;
			sum += matrix->rows[row-1][col-1];
		}
	}
	return sum/nAverage;
}

void smoothing(Matrix *matrix){
	int size = matrix->num_cols;
	for(int row=0; row<size; row++){
		for (int col = 0; col<size; col++){
			if (matrix->rows[row][col] == 0){
				
				double smooth = indiv_smoothing(matrix, row, col);
				printf("Smoothing to: %f\n", smooth);
				matrix->rows[row][col] = smooth;
			}
		}
	}
}




void square_step(Matrix *matrix, int grid_split, int sideLength, int N){

	for (int i = 0; i<grid_split; i++){
		for (int j = 0; j<grid_split; j++){
			//Define indices
			int halfSide = sideLength/2;
			int i_min = i*sideLength;
			int j_min = j*sideLength;
			int i_max = (i+1)*sideLength;
			int j_max = (j+1)*sideLength;
			int i_mid = i_min+halfSide;
			int j_mid = j_min+halfSide;

			//Calculate average
			int north_west = matrix->rows[j_min][j_min];
            int north_east = matrix->rows[j_max][i_min];
            int south_west = matrix->rows[j_min][i_max];
            int south_east = matrix->rows[j_max][i_max];
            int sum = north_west+north_east+south_west+south_east;
            double avg = (double) sum/4.0;
            int total = (int) avg + random_int(-N,N);
            if (matrix->rows[j_mid][i_mid]==0){
            	matrix->rows[j_mid][i_mid] = total;
        	}
		}
	}
	int size = matrix->num_cols;
	matrix->rows[size/2][size/2] = 255;
}

void diamond_step(Matrix *matrix, int grid_split, int sideLength, int N, int max_index){
	for (int i = 0; i<grid_split; i++){
		for (int j = 0; j<grid_split; j++){
			//Define indices

			int halfSide = sideLength/2;
			int i_min = i*sideLength;
			int j_min = j*sideLength;
			int i_max = (i+1)*sideLength;
			int j_max = (j+1)*sideLength;
			int i_mid = i_min+halfSide;
			int j_mid = j_min+halfSide;
			int center = matrix->rows[j_mid][i_mid];
			;
			int north_west = matrix->rows[j_min][j_min];
            int north_east = matrix->rows[j_max][i_min];
            int south_west = matrix->rows[j_min][i_max];
            int south_east = matrix->rows[j_max][i_max];
            int sum = north_west+north_east+south_west+south_east;

            int temp = 0;
            double avg = 0;
            //Diamond Step
            // Top Diamond - wraps if at edge.
            if (i_min == 0){
                temp = max_index - halfSide;
            } else {
                temp = i_min - halfSide;
            }
            // If Top value exists then skip else compute.
            if (matrix->rows[j_mid][i_min] == 0){
            	sum = center+north_west+north_east+matrix->rows[j_mid][temp];
            	avg = (double) sum/4.0;
                matrix->rows[j_mid][i_min] = avg + random_int(-N,N);
            }
            //Left Diamond - wraps if at edge.
            if (j_min == 0){
                temp = max_index - halfSide;
            } else {
                temp = j_min - halfSide;
            }
            //If Left value exists then skip else compute.
            if (matrix->rows[j_min][i_mid] == 0){
            	sum = center+north_west+south_west+matrix->rows[temp][i_mid];
            	avg = (double) sum/4.0;
                matrix->rows[j_min][i_mid] = avg + random_int(-N,N);
            }
            
            //Right Diamond - wraps if at edge.
            if (j_max == max_index){
                temp = 0 + halfSide;
            } else {
                temp = j_max + halfSide;
            }

            sum = center + north_east + south_east + matrix->rows[temp][i_mid];
            avg = (double) sum/4.0;
            matrix->rows[j_max][i_mid] = avg + random_int(-N,N);

            //Bottom Diamond - wraps at edge.
            if (i_max == max_index){
                temp = 0 + halfSide;
            } else {
                temp = i_max + halfSide;
            }
            sum = center+south_west+south_east + matrix->rows[j_mid][temp];
            avg = (double) sum/4.0;
            matrix->rows[j_mid][i_max] = avg + random_int(-N,N);
		}

	}		
}

Matrix* init_matrix(int size){
	Matrix *matrix = make_matrix(size, size);
    
    for (int i=0; i<matrix->num_rows; i++) {
        for (int j=0; j<matrix->num_cols; j++) {
            matrix->rows[i][j] = 0;
        }
    }
    matrix->rows[0][0] = 10;
    matrix->rows[0][size-1] = 10;
    matrix->rows[size/2][size/2] = 255;
    matrix->rows[size-1][0] = 10;
    matrix->rows[size-1][size-1] = 10;
    return matrix;
}

Matrix* make_mountain(int sizeM){
	srand(time(NULL));
    
	Matrix *matrix = init_matrix(sizeM);
    int size = matrix->num_cols-1;
    int ds_steps = 7;
    int num_quad = 1;
    double roughness = 50;
    int grid_split = 1;

    int max_index = size;//(int) pow(2.0, ds_steps);

    int sideLength = size;
    for (int step = 0; step < ds_steps; step++){
    	int loRough = roughness/(step +1.0);

    	square_step(matrix, grid_split, sideLength, loRough);

    	diamond_step(matrix, grid_split, sideLength, loRough, max_index);

    	sideLength /= 2;
    	grid_split *= 2;
    	printf("Done with step: %d\n", step);
    }
    smoothing(matrix);
    return matrix;
}


int main () {
	Matrix *matrix = make_mountain(17);

    print_matrix(matrix);
    printf("Size of matrix: %d\n", matrix->num_rows);
    free_matrix(matrix);
}

/*
To make mountain:
size = 17 - ds_steps = 4
size = 37 - ds_steps = 6
size = 41 - ds_steps = 6
size = 49 - ds_steps = 6;
size = 57 - ds_steps = 6;
size = 69 - ds_steps = 7;
*/