#ifndef WEATHER_DATA_H
#define WEATHER_DATA_H

#define CITY_NAME_LEN 64
#define DATE_LEN 11

typedef struct {
    char city[CITY_NAME_LEN];
    char date[DATE_LEN]; 
    double temp;
    int humidity;
    double wind_speed;
    char description[128];
    double precipitation_prob;
} WeatherReading;


int fetch_weather_from_api(const char* city, WeatherReading* out);








#endif 