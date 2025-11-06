#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h>


#include "weather_data.h"
#include "history.h"      
#include "alert.h"        
#include "hash_table.h"   
#include "queue.h"        
#include "sparse_matrix.h"
#include "graph.h"        
#include "xor_linked_list.h" 
#include "file_ops.h"     
#include "stack.h"


HashTable user_db; 
XorNode* xor_records = NULL; 
SparseMatrix* geo_grid = NULL; 
Graph city_graph; 
Stack undo_stack; 


WeatherReading temp_reading; 


void display_main_menu();
void handle_history_menu();
void handle_analysis_menu();
void handle_user_management_menu();
void initialize_app_data();
void cleanup_app_data();



void initialize_app_data() {
    
    printf("Uygulama verileri başlatılıyor...\n");
    
    
    init_hash_table(&user_db);
    
    init_stack(&undo_stack);
    
    init_graph(&city_graph);

    
    geo_grid = create_sparse_matrix(10, 10);
    
    
    load_history_from_file("history.bin"); 
    
   
    UserData admin = {"admin", "hash123", 2};
    insert_hash_table(&user_db, "admin", &admin);

    printf("Coğrafi komşuluk haritası yükleniyor...\n");
    add_city_to_graph(&city_graph, "Rize");
    add_city_to_graph(&city_graph, "Trabzon");
    add_city_to_graph(&city_graph, "Artvin");
    add_city_to_graph(&city_graph, "Erzurum");
    add_city_to_graph(&city_graph, "Bayburt");
    add_city_to_graph(&city_graph, "Giresun");
    add_edge(&city_graph, "Rize", "Trabzon");
    add_edge(&city_graph, "Rize", "Artvin");
    add_edge(&city_graph, "Rize", "Erzurum");
    add_edge(&city_graph, "Rize", "Bayburt");
    add_edge(&city_graph, "Trabzon", "Giresun");
    add_edge(&city_graph, "Trabzon", "Bayburt");
    add_edge(&city_graph, "Artvin", "Erzurum");
    
}

void cleanup_app_data() {
    printf("\nUygulama sonlandırılıyor, veriler kaydediliyor...\n");
    
    
    save_history_to_file("history.bin");
    
    
    clear_history();          
    free_xor_list(&xor_records); 
    free_sparse_matrix(geo_grid); 
    
    printf("Bellek temizlendi. Güle güle.\n");
}




void display_main_menu() {
    printf("\n======================================================\n");
    printf("     🚀 BASİT HAVA DURUMU İSTASYONU  🚀\n");
    printf("======================================================\n");
    printf("1. Hava Durumu Sorgula ve Kaydet \n");
    printf("2. Geçmiş ve Navigasyon İşlemleri \n");
    printf("3. Uyarı ve Analiz İşlemleri \n");
    printf("4. Kullanıcı Yönetimi \n");
    printf("5. Çıkış ve Kaydet\n");
    printf("------------------------------------------------------\n");
    printf("Seçiminiz (1-5): ");
}


void handle_weather_query() {
    char city[CITY_NAME_LEN];
    printf("\n--- Hava Durumu Sorgula ---\n");
    printf("Şehir Adı: ");
    get_safe_input(city, sizeof(city));
    WeatherReading current;
    
    
    if (fetch_weather_from_api(city, &current)) {
        printf("✅ %s için güncel hava durumu: %.1f°C, Hava: %s\n", 
               current.city, current.temp, current.description);

        
        add_to_history(&current);
        push_stack(&undo_stack, &current);
        check_and_add_alert(&current);
        
        
    } else {
        printf("❌Veri bulunamadı. Şehri kontrol edin.\n");
    }
}


void handle_history_menu() {
    int choice;
    WeatherReading undo_data;
    
    printf("\n--- Geçmiş ve Navigasyon Menüsü ---\n");
    printf("1. Geçmiş'i Görüntüle \n");
    printf("2. Son Aratılan Şehir \n");
    printf("3. Bellek Verimli Kayıtları Görüntüle \n");
    printf("Seçim: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            print_history(); 
            
            printf("\n(İleri/Geri navigasyon simülasyonu yapılır)\n");
            break;
        case 2:
           
            if (pop_stack(&undo_stack, &undo_data)) {
                printf("✅ Son işlem geri alındı: %s için %.1f°C\n", undo_data.city, undo_data.temp);
                
            } else {
                printf("Geri alınacak işlem yok.\n");
            }
            break;
        case 3:
           
            print_xor_list(xor_records);
            
            break;
        default:
            printf("Geçersiz seçim.\n");
    }
}


void handle_analysis_menu() {
    int choice;
    char start_city[CITY_NAME_LEN]; 
    printf("\n--- Uyarı ve Analiz Menüsü ---\n");
    printf("1. Öncelikli Uyarıları Görüntüle \n");
    printf("2. Riskli Komşu Analizi \n");
    printf("3. Risk Yolu Analizi \n");
    printf("Seçim: ");

    if (scanf("%d", &choice) != 1) {
        while(getchar() != '\n'); 
        choice = 0;
        return; 
    }
   
    while(getchar() != '\n');


    switch(choice) {
        case 1:
            
            print_alerts(); 
            break;
        case 2:
        printf("Risk analizi için başlangıç şehri girin (örn: Rize):");
        get_safe_input(start_city, sizeof(start_city));
        BFS_traverse(&city_graph, start_city);
        break;
        case 3:
        printf("Risk analizi için başlangıç şehri girin (örn: Rize):");
        get_safe_input(start_city, sizeof(start_city));
        DFS_traverse(&city_graph, start_city);
        break;
        default:
            printf("Geçersiz seçim.\n");
    }
}


void handle_user_management_menu() {
    int choice;
    char user[KEY_SIZE];
    UserData data;
    
    printf("\n--- Kullanıcı Yönetimi Menüsü ---\n");
    printf("1. Kullanıcı Ara \n");
    printf("2. Yeni Kullanıcı Ekle \n");
    printf("3. Kullanıcı Sil \n");
    printf("Seçim: ");
    scanf("%d", &choice);

    printf("Kullanıcı Adı: ");
    scanf("%s", user);

    switch(choice) {
        case 1:
            
            if (search_hash_table(&user_db, user, &data)) {
                printf("✅ Kullanıcı bulundu: %s, Erişim Seviyesi: %d\n", data.username, data.access_level);
            } else {
                printf("❌ Kullanıcı bulunamadı.\n");
            }
            break;
        case 2:
            
            strncpy(data.username, user, KEY_SIZE);
            strncpy(data.password_hash, "newhash", 64);
            data.access_level = 1;
            if (insert_hash_table(&user_db, user, &data)) {
                printf("✅ Kullanıcı başarıyla eklendi/güncellendi.\n");
            } else {
                printf("❌ Kullanıcı listesi dolu.\n");
            }
            break;
        case 3:
            
            if (delete_hash_table(&user_db, user)) {
                printf("✅ Kullanıcı silindi.\n");
            } else {
                printf("❌ Kullanıcı bulunamadı.\n");
            }
            break;
        default:
            printf("Geçersiz seçim.\n");
    }
}



int main() {
    int choice;
    
    initialize_app_data(); 

    do {
        display_main_menu();
        if (scanf("%d", &choice) != 1) {
            
            while(getchar() != '\n'); 
            choice = 0;
            continue;
        }
        while(getchar() != '\n');

        switch (choice) {
            case 1:
                handle_weather_query();
                break;
            case 2:
                handle_history_menu();
                break;
            case 3:
                handle_analysis_menu();
                break;
            case 4:
                handle_user_management_menu();
                break;
            case 5:
                printf("Uygulamadan çıkılıyor...\n");
                break;
            default:
                printf("Geçersiz seçim. Lütfen 1 ile 5 arasında bir sayı girin.\n");
        }
    } while (choice != 5);

    cleanup_app_data(); 
    
    return 0;
}