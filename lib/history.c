#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xor_linked_list.h"

Node* history_head = NULL;
extern XorNode* xor_records;

void free_weather_reading(void* data) {
    free(data);
}

void add_to_history(WeatherReading* w) {
    WeatherReading* copy = (WeatherReading*)malloc(sizeof(WeatherReading));
    if (copy == NULL) {
        perror("Bellek tahsis hatası");
        return;
    }
    *copy = *w;
    
    append_node(&history_head, copy);
    insert_xor_list(&xor_records, copy);
}

void print_history() {
    Node* current = history_head;
    printf("\n--- Geçmiş Kayıtlar (DLL) ---\n");
    while (current) {
        WeatherReading* w = (WeatherReading*)current->data;
        
        printf("Şehir: %s (Sıcaklık: %.1f°C, Nem: %d%%, Yağış İhtimali: %.0f%%, Hava: %s)\n", 
               w->city, w->temp, w->humidity, w->precipitation_prob, w->description);
        
        current = current->next;
    }
}

void print_history_by_date(const char* date) {
    Node* current = history_head;
    printf("\n--- %s Tarihli Geçmiş Kayıtlar ---\n", date);
    while (current) {
        WeatherReading* w = (WeatherReading*)current->data;
        if (strcmp(w->date, date) == 0) {
            printf("Şehir: %s, Sıcaklık: %.1f°C, Hava: %s\n", w->city, w->temp, w->description);
        }
        current = current->next;
    }
}

void clear_history() {
    free_list(&history_head, free_weather_reading);
    free_xor_list(&xor_records);
    printf("Geçmiş veriler temizlendi ve bellek serbest bırakıldı.\n");
}