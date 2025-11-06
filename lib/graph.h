#ifndef GRAPH_H
#define GRAPH_H

#include "queue.h" 
#include <stdbool.h>

#define MAX_CITIES 50
#define CITY_NAME_MAX_LEN 64


typedef struct AdjListNode {
    int dest_index; 
    struct AdjListNode* next;
} AdjListNode;


typedef struct CityNode {
    char name[CITY_NAME_MAX_LEN];
    AdjListNode* head; 
} CityNode;


typedef struct {
    CityNode cities[MAX_CITIES];
    int num_cities;
} Graph;


void init_graph(Graph* g);
int add_city_to_graph(Graph* g, const char* city_name);
void add_edge(Graph* g, const char* src_city, const char* dest_city);
void BFS_traverse(Graph* g, const char* start_city);
void DFS_traverse(Graph* g, const char* start_city);

#endif 