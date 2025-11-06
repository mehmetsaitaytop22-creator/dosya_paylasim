#include "sparse_matrix.h"
#include <stdlib.h>
#include <stdio.h>

SparseMatrix* create_sparse_matrix(int rows, int cols) {
    SparseMatrix* sm = (SparseMatrix*)malloc(sizeof(SparseMatrix));
    if (!sm) return NULL;

    sm->max_rows = rows;
    sm->max_cols = cols;

    
    sm->row_heads = (SM_Node**)calloc(rows, sizeof(SM_Node*));
    sm->col_heads = (SM_Node**)calloc(cols, sizeof(SM_Node*));

    if (!sm->row_heads || !sm->col_heads) {
        free(sm->row_heads);
        free(sm->col_heads);
        free(sm);
        return NULL;
    }
    return sm;
}


static SM_Node* create_sm_node(int r, int c, double val) {
    SM_Node* node = (SM_Node*)malloc(sizeof(SM_Node));
    node->row = r;
    node->col = c;
    node->value = val;
    node->next_in_row = NULL;
    node->next_in_col = NULL;
    return node;
}

void insert_sparse_matrix(SparseMatrix* sm, int row, int col, double value) {
    if (row >= sm->max_rows || col >= sm->max_cols) {
        fprintf(stderr, "HATA! Geçersiz konum.\n");
        return;
    }

    
    
    if (value == 0.0) return; 

    SM_Node* new_node = create_sm_node(row, col, value);

    
    SM_Node** p_row = &sm->row_heads[row];
    while (*p_row && (*p_row)->col < col) {
        p_row = &(*p_row)->next_in_row;
    }
    
    if (*p_row && (*p_row)->col == col) {
        (*p_row)->value = value;
        free(new_node); 
    } else { 
        new_node->next_in_row = *p_row;
        *p_row = new_node;
    }

    
    SM_Node** p_col = &sm->col_heads[col];
    while (*p_col && (*p_col)->row < row) {
        p_col = &(*p_col)->next_in_col;
    }
    
    
    if (!(*p_col) || (*p_col)->row != row) {
         new_node->next_in_col = *p_col;
        *p_col = new_node;
    }
}

double get_sparse_matrix_value(SparseMatrix* sm, int row, int col) {
    if (row >= sm->max_rows || col >= sm->max_cols) {
        return 0.0; 
    }
    SM_Node* current = sm->row_heads[row];
    while (current) {
        if (current->col == col) {
            return current->value;
        }
        if (current->col > col) {
            break; 
        }
        current = current->next_in_row;
    }
    return 0.0; 
}

void free_sparse_matrix(SparseMatrix* sm) {
    if (!sm) return;

    for (int i = 0; i < sm->max_rows; i++) {
        SM_Node* current = sm->row_heads[i];
        while (current) {
            SM_Node* temp = current;
            current = current->next_in_row;
            free(temp); 
        }
    }
    free(sm->row_heads);
    free(sm->col_heads);
    free(sm);
}