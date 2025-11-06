#include "file_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_safe_input(char* buffer, size_t size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n')
            buffer[len-1] = '\0'; 
        return buffer;
    }
    return NULL;
}


void save_history_to_file(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if(!fp) { perror("History dosyası açılamadı"); return; }
    
    Node* current = history_head;
    while(current) {
        
        WeatherReading* w = (WeatherReading*)current->data;
        fwrite(w, sizeof(WeatherReading), 1, fp);
        current = current->next;
    }
    fclose(fp);
    printf("Geçmiş veriler başarıyla kaydedildi.\n");
}

void load_history_from_file(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if(!fp) { 
        printf("Geçmiş dosyası bulunamadı, yeni bir geçmiş oluşturuluyor.\n");
        
        if (history_head != NULL) {
            clear_history(); 
        }
        history_head = NULL; 
        return; 
    }

    
    clear_history();

    WeatherReading w;
    while(fread(&w, sizeof(WeatherReading), 1, fp) == 1) {
        
        
        add_to_history(&w);
    }
    fclose(fp);
    printf("Geçmiş veriler başarıyla yüklendi.\n");
}


void save_alerts_to_file(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if(!fp) { perror("Alert dosyası açılamadı"); return; }
    
    
    
    if (alert_heap.size > 0) {
        fwrite(alert_heap.data, sizeof(WeatherAlert), alert_heap.size, fp);
    }
    
    fclose(fp);
    printf("Uyarılar başarıyla kaydedildi.\n");
}

void load_alerts_from_file(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if(!fp) { 
        alert_heap.size = 0; 
        printf("Uyarı dosyası bulunamadı, Heap boşaltıldı.\n");
        return; 
    }

    
    alert_heap.size = 0;
    
    
    size_t count = fread(alert_heap.data, sizeof(WeatherAlert), MAX_ALERTS, fp);
    
    
    alert_heap.size = (int)count;
    
    fclose(fp);
    printf("%d adet uyarı başarıyla yüklendi.\n", alert_heap.size);
    
    
    
    
    
}