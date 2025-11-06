#include "linkedlist.h"
#include <stdlib.h> 


Node* create_node(void* data) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (n == NULL) {
        
        return NULL;
    }
    n->data = data;
    n->next = NULL;
    n->prev = NULL; 
    return n;
}


void append_node(Node** head, void* data) {
    Node* n = create_node(data);
    if (n == NULL) return; 

    if (*head == NULL) {
        *head = n;
        return;
    }

    Node* cur = *head;
    while(cur->next) {
        cur = cur->next;
    }
    
    
    cur->next = n;
    n->prev = cur; 
}


Node* go_backward(Node* current) {
    if (current && current->prev) {
        return current->prev;
    }
    return current; 
}


Node* go_forward(Node* current) {
    if (current && current->next) {
        return current->next;
    }
    return current; 
}


void free_list(Node** head, void (*free_data_func)(void*)) {
    Node* current = *head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        if (free_data_func != NULL && current->data != NULL) {
            free_data_func(current->data); 
        }
        free(current); 
        current = next;
    }
    *head = NULL;
}