#ifndef XOR_LINKED_LIST_H
#define XOR_LINKED_LIST_H

#include <stdint.h> 


typedef struct XorNode {
    void* data; 
    struct XorNode* xor_ptr; 
} XorNode;


XorNode* xor_op(XorNode* a, XorNode* b);

void insert_xor_list(XorNode** head_ref, void* data);

void print_xor_list(XorNode* head);

void free_xor_list(XorNode** head_ref);

#endif 