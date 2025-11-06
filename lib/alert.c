#include "alert.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

AlertHeap alert_heap = { .size = 0 }; 


void swap(WeatherAlert* a, WeatherAlert* b) {
    WeatherAlert temp = *a;
    *a = *b;
    *b = temp;
}


void heapify_up(AlertHeap* heap, int index) {
    int parent_index = (index - 1) / 2;
    
    while (index > 0 && heap->data[index].severity > heap->data[parent_index].severity) {
        swap(&heap->data[index], &heap->data[parent_index]);
        index = parent_index;
        parent_index = (index - 1) / 2;
    }
}


void heapify_down(AlertHeap* heap, int index) {
    int max_index = index;
    int left_child = 2 * index + 1;
    int right_child = 2 * index + 2;

    
    if (left_child < heap->size && heap->data[left_child].severity > heap->data[max_index].severity) {
        max_index = left_child;
    }

    
    if (right_child < heap->size && heap->data[right_child].severity > heap->data[max_index].severity) {
        max_index = right_child;
    }

    if (max_index != index) {
        swap(&heap->data[index], &heap->data[max_index]);
        heapify_down(heap, max_index);
    }
}


WeatherAlert pop_max_alert(AlertHeap* heap) {
    if (heap->size == 0) {
        WeatherAlert empty_alert = { .severity = -1 };
        return empty_alert;
    }
    
    
    WeatherAlert max_alert = heap->data[0];
    
    
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    
    
    heapify_down(heap, 0);
    
    return max_alert;
}

void add_to_alert_heap(WeatherReading* w, int severity) {
    if (alert_heap.size < MAX_ALERTS) {
        
        alert_heap.data[alert_heap.size].reading = *w; 
        alert_heap.data[alert_heap.size].severity = severity;
        
        alert_heap.size++;
        
        
        heapify_up(&alert_heap, alert_heap.size - 1); 
    }
}


void check_and_add_alert(WeatherReading* w) {
    int severity = 0;
    
    if (w->wind_speed > 50) {
        severity = 3; 
    } else if (w->temp > 35.0 || w->temp < 0.0) {
        severity = 2; 
    } else if (strcmp(w->description, "Rainy") == 0 || strcmp(w->description, "Snowy") == 0) {
        severity = 1; 
    }

    if (severity > 0) {
        add_to_alert_heap(w, severity);
    }
}

void print_alerts() {
    if(alert_heap.size == 0) {
        printf("Şiddetli hava uyarısı yok.\n");
        return;
    }

    printf("--- ŞİDDETLİ HAVA UYARILARI (Öncelik Sırasına Göre) ---\n");
    
    
    
    
    AlertHeap temp_heap = alert_heap; 
    
    while(temp_heap.size > 0) {
        
        WeatherAlert alert = pop_max_alert(&temp_heap); 

        printf("⚠️ Şiddet: %d | Şehir: %s, Hava: %s, Sıcaklık: %.1f°C\n", 
               alert.severity, alert.reading.city, alert.reading.description, alert.reading.temp);
        
    }
    
}