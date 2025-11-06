#include "xor_linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include "weather_data.h" 


XorNode* xor_op(XorNode* a, XorNode* b) {
    
    return (XorNode*)((intptr_t)a ^ (intptr_t)b);
}


static XorNode* create_xor_node(void* data) {
    XorNode* node = (XorNode*)malloc(sizeof(XorNode));
    if(node) {
        node->data = data;
        node->xor_ptr = NULL;
    }
    return node;
}


void insert_xor_list(XorNode** head_ref, void* data) {
    XorNode* new_node = create_xor_node(data);
    if (!new_node) return;

    if (*head_ref == NULL) {
        
        *head_ref = new_node;
    } else {
        
        XorNode* current = *head_ref;
        XorNode* prev = NULL;
        XorNode* next;

        while (current != NULL) {
            next = xor_op(prev, current->xor_ptr);
            if (next == NULL) {
                
                break;
            }
            prev = current;
            current = next;
        }

        
        
        
        new_node->xor_ptr = xor_op(current, NULL);
        
        
        
        current->xor_ptr = xor_op(prev, new_node);
    }
}


void print_xor_list(XorNode* head) {
    XorNode* current = head;
    XorNode* prev = NULL;
    XorNode* next;

    printf("\n--- Bellek Verimli Kayitlar ---\n");
    if (head == NULL) {
        printf("Bellek Listesi bos.\n");
        return;
    }

    while (current != NULL) {
        
        WeatherReading* w = (WeatherReading*)current->data;
        printf("Sehir: %s, Sicaklik: %.1f\n", w->city, w->temp);

        
        next = xor_op(prev, current->xor_ptr);
        prev = current;
        current = next;
    }
}


void free_xor_list(XorNode** head_ref) {
    XorNode* current = *head_ref;
    XorNode* prev = NULL;
    XorNode* next;

    while (current != NULL) {
        

        next = xor_op(prev, current->xor_ptr);
        prev = current;
        
        free(current); 
        
        current = next;
    }
    *head_ref = NULL;
}