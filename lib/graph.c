#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "alert.h" 

extern AlertHeap alert_heap;



void init_graph(Graph* g) {
    g->num_cities = 0;
    for (int i = 0; i < MAX_CITIES; i++) {
        g->cities[i].head = NULL;
        g->cities[i].name[0] = '\0';
    }
}


static int get_city_index(Graph* g, const char* city_name) {
    for (int i = 0; i < g->num_cities; i++) {
        if (strcmp(g->cities[i].name, city_name) == 0) {
            return i;
        }
    }
    return -1;
}


int add_city_to_graph(Graph* g, const char* city_name) {
    if (g->num_cities >= MAX_CITIES) {
        printf("Graf dolu, yeni şehir eklenemiyor.\n");
        return -1;
    }
    if (get_city_index(g, city_name) != -1) {
        return get_city_index(g, city_name);
    }
    
    int new_index = g->num_cities;
    strcpy(g->cities[new_index].name, city_name);
    g->cities[new_index].head = NULL;
    g->num_cities++;
    return new_index;
}


static AdjListNode* new_adj_list_node(int dest_index) {
    AdjListNode* new_node = (AdjListNode*)malloc(sizeof(AdjListNode));
    new_node->dest_index = dest_index;
    new_node->next = NULL;
    return new_node;
}


void add_edge(Graph* g, const char* src_city, const char* dest_city) {
    int src_index = get_city_index(g, src_city);
    int dest_index = get_city_index(g, dest_city);

    if (src_index == -1) src_index = add_city_to_graph(g, src_city);
    if (dest_index == -1) dest_index = add_city_to_graph(g, dest_city);

    AdjListNode* new_node = new_adj_list_node(dest_index);
    new_node->next = g->cities[src_index].head;
    g->cities[src_index].head = new_node;

    new_node = new_adj_list_node(src_index);
    new_node->next = g->cities[dest_index].head;
    g->cities[dest_index].head = new_node;
}




static bool get_alert_status_for_city(const char* city_name, char* out_description, int* out_severity) {
    
    for (int i = 0; i < alert_heap.size; i++) {
        
        if (strcmp(alert_heap.data[i].reading.city, city_name) == 0) {
            
            strcpy(out_description, alert_heap.data[i].reading.description);
            *out_severity = alert_heap.data[i].severity;
            return true;
        }
    }
    
    strcpy(out_description, "Risk Yok");
    *out_severity = 0;
    return false;
}




void BFS_traverse(Graph* g, const char* start_city) {
    int start_index = get_city_index(g, start_city);
    if (start_index == -1) {
        printf("BFS baslangic sehri '%s' haritada bulunamadı.\n", start_city);
        return;
    }

    bool visited[MAX_CITIES] = {false};
    Queue q;
    init_queue(&q);

    visited[start_index] = true;
    enqueue(&q, (void*)(intptr_t)start_index); 

    printf("Riskli Komşu Analizi (BFS) - Başlangıç: %s\n", start_city);

    while (!is_queue_empty(&q)) {
        int u = (intptr_t)dequeue(&q);
        const char* current_city_name = g->cities[u].name;

        
        char alert_desc[128];
        int alert_severity = 0;
        get_alert_status_for_city(current_city_name, alert_desc, &alert_severity);
        
        printf(" -> %s (Durum: %s, Şiddet: %d)\n", current_city_name, alert_desc, alert_severity);
        

        AdjListNode* temp = g->cities[u].head;
        while (temp) {
            int v = temp->dest_index;
            if (!visited[v]) {
                visited[v] = true;
                enqueue(&q, (void*)(intptr_t)v);
            }
            temp = temp->next;
        }
    }
    printf("\n");
}




static void DFS_util(Graph* g, int v, bool visited[]) {
    visited[v] = true;
    const char* current_city_name = g->cities[v].name;

    
    char alert_desc[128];
    int alert_severity = 0;
    get_alert_status_for_city(current_city_name, alert_desc, &alert_severity);
        
    printf(" -> %s (Durum: %s, Şiddet: %d)\n", current_city_name, alert_desc, alert_severity);
    

    AdjListNode* temp = g->cities[v].head;
    while (temp) {
        int u = temp->dest_index;
        if (!visited[u]) {
            DFS_util(g, u, visited);
        }
        temp = temp->next;
    }
}


void DFS_traverse(Graph* g, const char* start_city) {
    int start_index = get_city_index(g, start_city);
    if (start_index == -1) {
        printf("DFS baslangic sehri '%s' haritada bulunamadı.\n", start_city);
        return;
    }

    bool visited[MAX_CITIES] = {false};
    
    printf("Risk Yolu Analizi (DFS) - Başlangıç: %s\n", start_city);
    DFS_util(g, start_index, visited);
    printf("\n");
}