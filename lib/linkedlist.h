#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct Node {
    void* data;
    struct Node* next;
    struct Node* prev; 
} Node;


Node* create_node(void* data);


void append_node(Node** head, void* data);


void free_list(Node** head, void (*free_data_func)(void*));


Node* go_backward(Node* current);
Node* go_forward(Node* current);

#endif