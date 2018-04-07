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

int main () {
    Matrix *matrix = make_matrix(5, 5);
    for (int i=0; i<matrix->num_rows; i++) {
        for (int j=0; j<matrix->num_cols; j++) {
            matrix->rows[i][j] = 0;
        }
    }
    matrix->rows[0][0] = 10;
    matrix->rows[0][4] = 10;
    matrix->rows[4][0] = 10;
    matrix->rows[4][4] = 10;

    print_matrix(matrix);

    free_matrix(matrix);
}