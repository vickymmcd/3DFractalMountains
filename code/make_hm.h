typedef struct {
    int num_rows, num_cols;
    double **rows;
} Matrix;

Matrix* make_mountain(int sizeM, int steps);
void print_matrix(Matrix *matrix);
