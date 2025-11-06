#include "weather_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

const char* API_KEY = "7f85aaf07314ecbbdb31c5778b820a08";

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    printf("Bellek yetersiz (realloc hatası)\n");
    return 0;
  }
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

int fetch_weather_from_api(const char* city, WeatherReading* out) {
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  
    chunk.size = 0;    
    
    CURL *esc = curl_easy_init();
    char *escaped_city = curl_easy_escape(esc, city, 0);
    if (!escaped_city) {
        fprintf(stderr, "HATA: '%s' şehir adı internet (URL) formatına dönüştürülemedi.\n", city);
        free(chunk.memory);
        curl_easy_cleanup(esc);
        return 0;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if(curl_handle) {
        char url[512];
        
        snprintf(url, sizeof(url), 
                 "https://api.openweathermap.org/data/2.5/forecast?q=%s&appid=%s&units=metric&lang=tr", 
                 escaped_city, API_KEY);
        
        curl_free(escaped_city);
        curl_easy_cleanup(esc);

        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
        res = curl_easy_perform(curl_handle);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() başarısız: %s\n", curl_easy_strerror(res));
            free(chunk.memory);
            curl_easy_cleanup(curl_handle);
            return 0; 
        }

        cJSON *json = cJSON_Parse(chunk.memory);
        if (json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                fprintf(stderr, "JSON parse hatası: %s\n", error_ptr);
            }
            free(chunk.memory);
            curl_easy_cleanup(curl_handle);
            return 0; 
        }

        const cJSON *city_obj = cJSON_GetObjectItemCaseSensitive(json, "city");
        if (!city_obj) {
             printf("[API] Hata: '%s' için veri bulunamadı (city objesi eksik).\n", city);
             cJSON_Delete(json);
             free(chunk.memory);
             curl_easy_cleanup(curl_handle);
             return 0; 
        }
        const cJSON *name = cJSON_GetObjectItemCaseSensitive(city_obj, "name");
        
        const cJSON *list = cJSON_GetObjectItemCaseSensitive(json, "list");
        if (!list || !cJSON_IsArray(list) || cJSON_GetArraySize(list) == 0) {
             printf("[API] Hata: '%s' için tahmin listesi bulunamadı.\n", city);
             cJSON_Delete(json);
             free(chunk.memory);
             curl_easy_cleanup(curl_handle);
             return 0; 
        }

        const cJSON *first_forecast = cJSON_GetArrayItem(list, 0);
        const cJSON *main_obj = cJSON_GetObjectItemCaseSensitive(first_forecast, "main");
        const cJSON *weather_array = cJSON_GetObjectItemCaseSensitive(first_forecast, "weather");
        const cJSON *wind = cJSON_GetObjectItemCaseSensitive(first_forecast, "wind");
        const cJSON *pop = cJSON_GetObjectItemCaseSensitive(first_forecast, "pop"); 

        const cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
        const cJSON *description = cJSON_GetObjectItemCaseSensitive(weather, "description");
        const cJSON *temp = cJSON_GetObjectItemCaseSensitive(main_obj, "temp");
        
        
        const cJSON *humidity = cJSON_GetObjectItemCaseSensitive(main_obj, "humidity");
        
        
        const cJSON *wind_speed = cJSON_GetObjectItemCaseSensitive(wind, "speed");

        strncpy(out->city, name->valuestring, CITY_NAME_LEN - 1);
        out->city[CITY_NAME_LEN - 1] = '\0';
        
        if (description) {
            strncpy(out->description, description->valuestring, sizeof(out->description) - 1);
            out->description[sizeof(out->description) - 1] = '\0';
        } else {
            strcpy(out->description, "N/A");
        }
        
        out->temp = temp->valuedouble;
        out->humidity = humidity->valueint; 
        out->wind_speed = wind_speed->valuedouble;
        out->precipitation_prob = pop->valuedouble * 100.0; 
        
        strcpy(out->date, "TAHMİN"); 

        cJSON_Delete(json); 
        free(chunk.memory); 
        curl_easy_cleanup(curl_handle); 

        return 1; 
    }
    
    curl_free(escaped_city);
    curl_easy_cleanup(esc);
    
    return 0; 
}