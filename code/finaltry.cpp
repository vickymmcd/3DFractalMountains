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


/* Perform row reduction.
Subtract a multiple of row j from row i so that the first element
of row i is 0.
*/
void reduce_matrix_rows(Matrix *matrix, int i, int j) {
    double *row1 = matrix->rows[i];
    double *row2 = matrix->rows[j];
    double factor = row1[0] / row2[0];

    for (int j=0; j<matrix->num_cols; j++) {
        row1[j] -= row2[j] * factor;
    }
}

int random_int(int min, int max)
{
   return (rand() + min) % (max + 1);
   //return min + rand() % (max+1 - min);
}

int oneDiamond(Matrix *matrix, int x, int y, int halfSide, int N, int even){
	printf("X: %d, Y: %d, halfSide: %d\n", x, y, halfSide);
	double sum = 0;
	double nSquare = 0;
	int size = matrix->num_cols;
	/*for (int l = -1; l< 2; l+=1){
		int x = i+(l*step);
		for (int k = -1; k<2; k+=1){
			
			int y = j+(k*step);;
			if ((l == 0 && y >=0) || (x >= 0 && k ==0)){
				nSquare += 1;
				sum += matrix->rows[x][y];
			}
		}	
	}*/
	if (y >= 0 && y<=size-1){
		if (((x - halfSide)) >=0 && (x - halfSide)<=size-1){
			printf("Xcorner: %d, Ycorner: %d\n", (x - halfSide), y);
			nSquare++;
			sum += matrix->rows[y][(x - halfSide)];//left of center
		}	
		if (((x + halfSide))>=0  && (x + halfSide)<=size-1) {
			printf("Xcorner: %d, Ycorner: %d\n", (x + halfSide), y);
			nSquare++;
			sum += matrix->rows[y][(x + halfSide)]; //right of center
		}
	}
	if (x >= 0 && x<=size-1)	{	
		if (((y + halfSide)) >=0  && (y + halfSide)<=size-1){
			printf("Xcorner: %d, Ycorner: %d\n", x, y+halfSide);
			nSquare++;
			sum += matrix->rows[(y + halfSide)][x]; //below center
		}
		if (((y - halfSide)) >=0  && (y - halfSide)<=size-1){	
			printf("Xcorner: %d, Ycorner: %d\n", x, y-halfSide);
			nSquare++;
			sum += matrix->rows[(y - halfSide)][x];
		}
	}		
	matrix->rows[y][x]= (int) sum/nSquare + random_int(-N,N);
	return (int) sum/nSquare ;
}	

void diamond(Matrix *matrix, int i, int j, int step, int N){
	int length = matrix->num_cols;
	int middle = (int)((double) length/2.0);

	for (int l = -1; l< 2; l+=1){
		int x = i+(l*step);
		for (int k = -1; k<2; k+=1){
			
			int y = j+(k*step);;
			if ((l == 0 && y >=0) || (x >= 0 && k ==0)){
				matrix->rows[x][y] = oneDiamond(matrix, 0, 2, 2, N, 1) + random_int(-N,N);
			}
		}	
	}
	

}

void square(Matrix *matrix, int x_max, int y_max, int x_min, int y_min, int N, int even){
	
	int length = x_max-x_min;

	int middle = (int)((double) length/2.0);

	int sum = matrix->rows[y_min][x_min]+matrix->rows[y_min][x_max]+
	matrix->rows[y_max][x_min]+matrix->rows[y_max][x_max];
	/*for (int i=x_min; i<=x_max; i+=middle){
		for (int j=y_min; j<=y_max; j+=middle){
			sum += matrix->rows[i][j];
		}
	}*/


	int randomN = random_int(-N,N);
	double value = (double) sum/4.0;
	matrix->rows[y_min+middle][x_min+middle] = (int) value + randomN;
	//square(matrix, x_min+middle, y_min+middle, middle, N);
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
	matrix->rows[size/2][size/2] = 500;
	/*for (int x = 0; x < matrix->num_cols-1; x += sideLength) {
        for (int y = 0; y < matrix->num_cols-1; y += sideLength) {
        	printf("Before:\n");
        	print_matrix(matrix);
        	square(matrix, x+sideLength, y+sideLength, x, y, roughness, even);
        	printf("After:\n");
        	print_matrix(matrix);
        	printf("\n\n");
            //x, y is upper left corner of square
            //calculate average of existing corners
            /*double avg = data[x, y] + //top left
            data[x + sideLength, y] +//top right
            data[x, y + sideLength] + //lower left
            data[x + sideLength, y + sideLength];//lower right
            avg /= 4.0;

            //center is average plus random offset
            data[x + halfSide, y + halfSide] =
          //We calculate random value in range of 2h
          //and then subtract h so the end value is
          //in the range (-h, +h)
          avg + (r.NextDouble() * 2 * h) - h;
        }
	}*/
}

void diamond_step(Matrix *matrix, int grid_split, int sideLength, int N, int max_index){
	printf("Doing good\n");
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
            printf("Step 1\n");
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
            printf("Step 2\n");
            //Left Diamond - wraps if at edge.
            if (j_min == 0){
                temp = max_index - halfSide;
            } else {
                temp = j_min - halfSide;
            }
            printf("Step 2.5\n");
            //If Left value exists then skip else compute.
            if (matrix->rows[j_min][i_mid] == 0){
            	printf("Temp: %d, I_mid: %d\n", temp, i_mid);
            	sum = center+north_west+south_west+matrix->rows[temp][i_mid];
            	printf("Working fine\n");
            	avg = (double) sum/4.0;
                matrix->rows[j_min][i_mid] = avg + random_int(-N,N);
            }
            printf("Step 3\n");
            
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
            printf("Row\n");
		}

	}		
	printf("Finished\n");
	//halfSide /= 2;
	/*for (int x = 0; x < matrix->num_cols; x += halfSide) {
        //and y is x offset by half a side, but moved by
        //the full side length
        //NOTE: if the data shouldn't wrap then y < DATA_SIZE
        //to generate the far edge values
        for (int y = (x + halfSide) % sideLength; y < matrix->num_cols; y += sideLength) {
            printf("Before:\n");
        	print_matrix(matrix);
            oneDiamond(matrix, x, y, halfSide, roughness, even);
            printf("After:\n");
        	print_matrix(matrix);
        	printf("\n\n");
            //x, y is center of diamond
            //note we must use mod  and add DATA_SIZE for subtraction 
            //so that we can wrap around the array to find the corners
            double avg =
              data[(x - halfSide + DATA_SIZE) % DATA_SIZE, y] + //left of center
              data[(x + halfSide) % DATA_SIZE, y] + //right of center
              data[x, (y + halfSide) % DATA_SIZE] + //below center
              data[x, (y - halfSide + DATA_SIZE) % DATA_SIZE]; //above center
            avg /= 4.0;

            //new value = average plus random offset
            //We calculate random value in range of 2h
            //and then subtract h so the end value is
            //in the range (-h, +h)
            avg = avg + (r.NextDouble() * 2 * h) - h;
            //update value for center of diamond
            data[x, y] = avg;

            //wrap values on the edges, remove
            //this and adjust loop condition above
            //for non-wrapping values.
            if (x == 0) data[DATA_SIZE - 1, y] = avg;
            if (y == 0) data[x, DATA_SIZE - 1] = avg;
        }
	}*/
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
    matrix->rows[size/2][size/2] = 500;
    matrix->rows[size-1][0] = 10;
    matrix->rows[size-1][size-1] = 10;
    return matrix;
}


int main () {
	srand(time(NULL));
    
	Matrix *matrix = init_matrix(17);
    int size = matrix->num_cols-1;
    int ds_steps = 4;
    int num_quad = 1;
    double roughness = 30;
    int grid_split = 1;

    #include <math.h>

    int max_index = size;//(int) pow(2.0, ds_steps);

    int sideLength = size;
    for (int step = 0; step < ds_steps; step++){
    	int loRough = roughness/(step +1.0);
    	//printf("Before Square\n");
    	//print_matrix(matrix);
    	square_step(matrix, grid_split, sideLength, loRough);
    	printf("After square, before diamond\n");
    	//print_matrix(matrix);
    	printf("Doing good 5\n");
    	diamond_step(matrix, grid_split, sideLength, loRough, max_index);
    	//printf("after diamond, before math\n");
    	//print_matrix(matrix);
    	sideLength /= 2;
    	grid_split *= 2;
    	//printf("At the end of the loop\n");
    	//print_matrix(matrix);
    }
    print_matrix(matrix);
    /*for (int sideLength = matrix->num_cols-1; sideLength>=2; sideLength /= 2, roughness /= 2.0){
    	int even = sideLength %2;
    	int halfSide = sideLength / 2;
    	//printf("sideLength: %d, halfSide: %d\n", sideLength, halfSide);
    	//generate the new square values
        printf("Squaring ----------------------------------------\n");
        square_step(matrix, grid_split, shapeLength, roughness);
		printf("Diamonding ----------------------------------------\n");
		diamondStep(matrix,grid_split, shapeLength, roughness);
		
    }*/

    /*diamond(matrix, 0, 0,4,4,30);
    //square(matrix, 2, 2, 2, 30);

    //diamond(matrix, 0,0,2,2,15);
    //diamond(matrix, 2,0,4,2,15);
    //diamond(matrix, 0,2,2,4,15);
    //diamond(matrix, 2,2,4,4,15);

    for (int u = 0; u<=3; u+=2){
    	for (int y = 0; y<= 3; y+=2){
    		diamond(matrix, u, y, u+2, y+2, 15);
    	}
    }*/
    //square()
   // print_matrix(matrix);

    free_matrix(matrix);
}