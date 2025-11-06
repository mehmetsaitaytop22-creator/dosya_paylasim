#include "stack.h"
#include <stdio.h>
#include <string.h> 
#include <stdbool.h> 

void init_stack(Stack* s) { 
    s->top = 0; 
}

void push_stack(Stack* s, WeatherReading* w) {
    if(s->top < MAX_STACK_SIZE) {
        
        s->data[s->top] = *w; 
        s->top++;
    } else {
        printf("Stack dolu. Undo işlemi kaydedilemedi.\n");
    }
}


int pop_stack(Stack* s, WeatherReading* out) {
    if (s->top > 0) {
        s->top--;
        
        *out = s->data[s->top];
        return 1; 
    }
    return 0; 
}


void print_stack(Stack* s) {
    printf("--- Stack İçeriği (Son Sorgular) ---\n");
    for(int i = s->top - 1; i >= 0; i--) { 
        WeatherReading* w = &s->data[i];
        printf("[%d] Şehir: %s, Sıcaklık: %.1f°C\n", i, w->city, w->temp);
    }
}