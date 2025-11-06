#ifndef STACK_H
#define STACK_H

#include "weather_data.h"

#define MAX_STACK_SIZE 100 

typedef struct {
    
    WeatherReading data[MAX_STACK_SIZE]; 
    int top;
} Stack;

void init_stack(Stack* s);
void push_stack(Stack* s, WeatherReading* w);

int pop_stack(Stack* s, WeatherReading* out); 
void print_stack(Stack* s); 

#endif