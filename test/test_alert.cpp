#include <gtest/gtest.h>
#include <cstring> // strcpy için

/* C++ test kodunun, sizin C kütüphanelerinizi
 * (weather_data.h ve alert.h) bulabilmesi için
 * bu extern "C" bloğu zorunludur.
 */
extern "C" {
  #include "weather_data.h" 
  #include "alert.h"
}

/* * DİKKAT: 'alert_heap' global bir değişken.
 * Testlerin birbirinden etkilenmemesi için her testten ÖNCE
 * 'alert_heap.size = 0;' yaparak sıfırlamalıyız.
 * Bunun için "Fixture" (Test Altyapısı) kullanıyoruz.
 */
class AlertTest : public ::testing::Test {
protected:
  // Testlerde kullanılacak örnek hava durumu verileri
  WeatherReading w_normal;
  WeatherReading w_yagis; // Severity 1
  WeatherReading w_soguk; // Severity 2
  WeatherReading w_firtina; // Severity 3

  // Her test case'inden ÖNCE bu fonksiyon çalışır
  void SetUp() override {
    // Global heap'i her testten önce sıfırla
    alert_heap.size = 0;

    // Test verilerini hazırla
    strcpy(w_normal.city, "Normal");
    strcpy(w_normal.description, "Sunny");
    w_normal.temp = 25.0;
    w_normal.wind_speed = 10.0;

    strcpy(w_yagis.city, "Yagmurlu");
    strcpy(w_yagis.description, "Rainy");
    w_yagis.temp = 15.0;
    w_yagis.wind_speed = 20.0;

    strcpy(w_soguk.city, "Soguk");
    strcpy(w_soguk.description, "Sunny");
    w_soguk.temp = -5.0; // Sev. 2 tetikler
    w_soguk.wind_speed = 10.0;

    strcpy(w_firtina.city, "Firtinali");
    strcpy(w_firtina.description, "Cloudy");
    w_firtina.temp = 20.0;
    w_firtina.wind_speed = 60.0; // Sev. 3 tetikler
  }
};

// Test 1: Heap'in (global alert_heap) boş başladığını test et
TEST_F(AlertTest, Initialization) {
  // SetUp() zaten size'ı 0 yaptı
  ASSERT_EQ(alert_heap.size, 0);
}

// Test 2: 'check_and_add_alert' fonksiyonunun kural mantığını test et
TEST_F(AlertTest, CheckAlertLogic) {
  // Kural 1: Fırtına (Sev 3)
  check_and_add_alert(&w_firtina);
  ASSERT_EQ(alert_heap.size, 1);
  ASSERT_EQ(alert_heap.data[0].severity, 3);
  ASSERT_STREQ(alert_heap.data[0].reading.city, "Firtinali");

  // Kural 2: Aşırı Soğuk (Sev 2)
  alert_heap.size = 0; // Testi izole et
  check_and_add_alert(&w_soguk);
  ASSERT_EQ(alert_heap.size, 1);
  ASSERT_EQ(alert_heap.data[0].severity, 2);

  // Kural 3: Yağış (Sev 1)
  alert_heap.size = 0; // Testi izole et
  check_and_add_alert(&w_yagis);
  ASSERT_EQ(alert_heap.size, 1);
  ASSERT_EQ(alert_heap.data[0].severity, 1);
  
  // Kural 4: Alert Yok (Sev 0)
  alert_heap.size = 0; // Testi izole et
  check_and_add_alert(&w_normal);
  ASSERT_EQ(alert_heap.size, 0); // Heap'e eklenmemeli
}

// Test 3: heapify_up fonksiyonunun Max-Heap özelliğini koruduğunu test et
TEST_F(AlertTest, PriorityHeapOrder) {
  // Düşük öncelik ekle
  add_to_alert_heap(&w_yagis, 1); // [1]
  ASSERT_EQ(alert_heap.data[0].severity, 1);
  ASSERT_EQ(alert_heap.size, 1);

  // Yüksek öncelik ekle (heapify_up çalışmalı)
  add_to_alert_heap(&w_firtina, 3); // [3, 1] olmalı
  ASSERT_EQ(alert_heap.data[0].severity, 3); // Kök (en üst) değişmeli
  ASSERT_EQ(alert_heap.size, 2);

  // Orta öncelik ekle
  add_to_alert_heap(&w_soguk, 2); // [3, 1, 2] -> [3, 2, 1] olmalı
  ASSERT_EQ(alert_heap.data[0].severity, 3); // Kök değişmemeli
  ASSERT_EQ(alert_heap.size, 3);
  
  // Kökün altındakilerin de doğru olduğunu kontrol edebiliriz
  // (Not: Max-Heap'te sadece data[0] > data[1] ve data[0] > data[2] garantidir)
  ASSERT_TRUE(alert_heap.data[0].severity >= alert_heap.data[1].severity);
  ASSERT_TRUE(alert_heap.data[0].severity >= alert_heap.data[2].severity);
}

// Test 4: pop_max_alert (Öncelik sırasını) test et
TEST_F(AlertTest, PopMaxAlertOrder) {
  // Karışık sırada ekle
  add_to_alert_heap(&w_soguk, 2);  // [2]
  add_to_alert_heap(&w_yagis, 1);  // [2, 1]
  add_to_alert_heap(&w_firtina, 3); // [3, 1, 2] -> [3, 2, 1]
  
  ASSERT_EQ(alert_heap.size, 3);

  // 1. Pop (En yüksek öncelik, Sev 3 gelmeli)
  WeatherAlert alert1 = pop_max_alert(&alert_heap);
  ASSERT_EQ(alert_heap.size, 2);
  ASSERT_EQ(alert1.severity, 3);
  ASSERT_STREQ(alert1.reading.city, "Firtinali");

  // 2. Pop (Kalanlardan en yüksek, Sev 2 gelmeli)
  WeatherAlert alert2 = pop_max_alert(&alert_heap);
  ASSERT_EQ(alert_heap.size, 1);
  ASSERT_EQ(alert2.severity, 2);
  ASSERT_STREQ(alert2.reading.city, "Soguk");

  // 3. Pop (Son kalan, Sev 1 gelmeli)
  WeatherAlert alert3 = pop_max_alert(&alert_heap);
  ASSERT_EQ(alert_heap.size, 0);
  ASSERT_EQ(alert3.severity, 1);
  ASSERT_STREQ(alert3.reading.city, "Yagmurlu");
}

// Test 5: Boş heap'ten pop yapmayı test et
TEST_F(AlertTest, PopFromEmptyHeap) {
  ASSERT_EQ(alert_heap.size, 0); // Garanti

  WeatherAlert alert = pop_max_alert(&alert_heap);

  // Beklenti: Kodunuza göre severity = -1 dönmeli
  ASSERT_EQ(alert.severity, -1);
  ASSERT_EQ(alert_heap.size, 0); // Boyut 0'da kalmalı
}