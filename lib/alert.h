#ifndef ALERT_H
#define ALERT_H

#include "weather_data.h" 

#define MAX_ALERTS 100


typedef struct {
    WeatherReading reading; 
    int severity;           
} WeatherAlert;

typedef struct {
    WeatherAlert data[MAX_ALERTS];
    int size;
} AlertHeap;

extern AlertHeap alert_heap; 


void swap(WeatherAlert* a, WeatherAlert* b); 
void heapify_up(AlertHeap* heap, int index); 
void heapify_down(AlertHeap* heap, int index); 

void check_and_add_alert(WeatherReading* w);
void print_alerts();
void add_to_alert_heap(WeatherReading* w, int severity); 


WeatherAlert pop_max_alert(AlertHeap* heap); 

#endif