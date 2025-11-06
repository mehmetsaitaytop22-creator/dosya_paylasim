#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "history.h"
#include "alert.h"
#include <stddef.h>  


void save_history_to_file(const char* filename);
void load_history_from_file(const char* filename);


void save_alerts_to_file(const char* filename);
void load_alerts_from_file(const char* filename);


char* get_safe_input(char* buffer, size_t size);

#endif 
