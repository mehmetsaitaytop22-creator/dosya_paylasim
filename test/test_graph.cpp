#include <gtest/gtest.h>
#include <cstring>  // strcpy
#include <string>   // C++ string kütüphanesi
#include <iostream> // Konsol çıktısını yakalamak için

/* C++ test kodunun, sizin C kütüphanelerinizi
 * (graph.h ve bağımlılıkları) bulabilmesi için
 * bu extern "C" bloğu zorunludur.
 */
extern "C" {
  #include "graph.h"
  #include "alert.h" // graph.c buna bağımlı
  #include "queue.h" // graph.h buna bağımlı
  #include "weather_data.h"
}

/* Her test için temiz bir graph ve temiz bir alert_heap
 * oluşturmak için "Fixture" kullanıyoruz.
 */
class GraphTest : public ::testing::Test {
protected:
  Graph g; // Testlerde kullanılacak graf

  // Her test case'inden ÖNCE bu fonksiyon çalışır
  void SetUp() override {
    // Her test temiz bir graf ile başlasın
    init_graph(&g);
    
    // graph.c, global 'alert_heap' kullandığı için
    // her testten önce onu da sıfırlamalıyız!
    alert_heap.size = 0;
  }

  // Her test case'inden SONRA bu fonksiyon çalışır
  void TearDown() override {
    // Not: Kodunuzda 'free_graph' fonksiyonu yok.
    // Bu, testler sırasında bellek sızıntısı yapacağı anlamına gelir.
    // Ancak bu, testlerin BAŞARILI olmasına engel değildir.
    // (Bu, hocanıza göstermeniz gereken bir eksikliktir:
    // graph.c'de AdjListNode'ları free eden bir fonksiyon yazmalısınız.)
  }
};

// Test 1: Grafiğin başlatılmasını test et
TEST_F(GraphTest, Initialization) {
  ASSERT_EQ(g.num_cities, 0);
  // Diğer kontroller (örn: cities dizisinin NULL olması) eklenebilir.
}

// Test 2: Şehir (node) ve komşuluk (edge) eklemeyi test et
TEST_F(GraphTest, AddCityAndEdge) {
  int rize_idx = add_city_to_graph(&g, "Rize");
  int trabzon_idx = add_city_to_graph(&g, "Trabzon");
  
  ASSERT_EQ(rize_idx, 0);
  ASSERT_EQ(trabzon_idx, 1);
  ASSERT_EQ(g.num_cities, 2);
  ASSERT_STREQ(g.cities[0].name, "Rize");

  // Komşuluk ekle (yönsüz)
  add_edge(&g, "Rize", "Trabzon");

  // Rize'nin komşularını kontrol et
  ASSERT_NE(g.cities[rize_idx].head, nullptr);
  ASSERT_EQ(g.cities[rize_idx].head->dest_index, trabzon_idx);

  // Trabzon'un komşularını kontrol et (yönsüz olduğu için)
  ASSERT_NE(g.cities[trabzon_idx].head, nullptr);
  ASSERT_EQ(g.cities[trabzon_idx].head->dest_index, rize_idx);
}

// Test 3: BFS (Riskli Komşu Analizi) - Risk Yok Durumu
TEST_F(GraphTest, BFS_NoAlerts) {
  add_city_to_graph(&g, "Rize");
  add_city_to_graph(&g, "Trabzon");
  add_edge(&g, "Rize", "Trabzon");

  // Konsol çıktısını yakalamayı başlat
  ::testing::internal::CaptureStdout();
  
  BFS_traverse(&g, "Rize");
  
  // Yakalanan çıktıyı al
  std::string output = ::testing::internal::GetCapturedStdout();

  // Beklenti: Çıktı "Riskli Komşu Analizi" başlığını içermeli
  ASSERT_NE(output.find("Riskli Komşu Analizi (BFS)"), std::string::npos);
  
  // Beklenti: 'alert_heap' boş olduğu için "Risk Yok" yazmalı
  ASSERT_NE(output.find("-> Rize (Durum: Risk Yok, Şiddet: 0)"), std::string::npos);
  ASSERT_NE(output.find("-> Trabzon (Durum: Risk Yok, Şiddet: 0)"), std::string::npos);
}

// Test 4: DFS (Risk Yolu Analizi) - Risk Var Durumu
TEST_F(GraphTest, DFS_WithAlerts) {
  add_city_to_graph(&g, "Rize");
  add_city_to_graph(&g, "Trabzon");
  add_edge(&g, "Rize", "Trabzon");
  
  // "Rize" için manuel olarak bir uyarı oluştur ve global heap'e ekle
  WeatherReading w_firtina;
  strcpy(w_firtina.city, "Rize");
  strcpy(w_firtina.description, "Firtinali");
  w_firtina.temp = 20.0;
  w_firtina.wind_speed = 60.0; // Bu Sev. 3 tetikler
  check_and_add_alert(&w_firtina); // Heap'e eklendi

  // Konsol çıktısını yakalamayı başlat
  ::testing::internal::CaptureStdout();
  
  DFS_traverse(&g, "Rize");

  // Yakalanan çıktıyı al
  std::string output = ::testing::internal::GetCapturedStdout();
  
  // Beklenti: Çıktı "Risk Yolu Analizi" başlığını içermeli
  ASSERT_NE(output.find("Risk Yolu Analizi (DFS)"), std::string::npos);

  // Beklenti: Rize için "Şiddet: 3" yazmalı (çünkü heap'e ekledik)
  ASSERT_NE(output.find("-> Rize (Durum: Firtinali, Şiddet: 3)"), std::string::npos);
  
  // Beklenti: Trabzon için "Risk Yok" yazmalı
  ASSERT_NE(output.find("-> Trabzon (Durum: Risk Yok, Şiddet: 0)"), std::string::npos);
}