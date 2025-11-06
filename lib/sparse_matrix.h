#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H


typedef struct SM_Node {
    int row;
    int col;
    double value; 
    struct SM_Node* next_in_row;
    struct SM_Node* next_in_col;
} SM_Node;


typedef struct {
    int max_rows;
    int max_cols;
    SM_Node** row_heads; 
    SM_Node** col_heads; 
} SparseMatrix;


SparseMatrix* create_sparse_matrix(int rows, int cols);
void insert_sparse_matrix(SparseMatrix* sm, int row, int col, double value);
double get_sparse_matrix_value(SparseMatrix* sm, int row, int col);
void free_sparse_matrix(SparseMatrix* sm);

#endif 