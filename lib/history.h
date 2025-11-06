#ifndef HISTORY_H
#define HISTORY_H

#include "weather_data.h"
#include "linkedlist.h"


extern Node* history_head;


void add_to_history(WeatherReading* w);


void print_history();


void print_history_by_date(const char* date);
void clear_history();
#endif
