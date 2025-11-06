#include <gtest/gtest.h>
#include <cstring> // strcpy için C++ 'da bu kütüphane gerekir

/* * C++ test kodunun, sizin C kütüphanelerinizi
 * (weather_data.h ve stack.h) bulabilmesi için 
 * bu extern "C" bloğu zorunludur.
 */
extern "C" {
  #include "weather_data.h" 
  #include "stack.h"
  // Diğer kütüphanelerinizi test ettikçe buraya ekleyebilirsiniz:
  // #include "linkedlist.h"
  // #include "hash_table.h"
  // #include "alert.h"
}

/* * Test Suit Adı: StackTest
 * Test Case Adı: Initialization
 * Bu test, init_stack fonksiyonunun stack'i doğru başlattığını kontrol eder.
 */
TEST(StackTest, Initialization) {
  Stack s;
  init_stack(&s);
  
  // Beklenti: top 0 olmalı
  ASSERT_EQ(s.top, 0); 
}

/* * Test Case Adı: PushAndPop
 * Bu test, stack'e veri eklemeyi (push) ve çıkarmayı (pop) test eder.
 */
TEST(StackTest, PushAndPop) {
  Stack s;
  init_stack(&s);

  // Bir WeatherReading objesi oluştur ve stack'e "push" et
  WeatherReading w1;
  strcpy(w1.city, "Rize"); // C struct'ı olduğu için strcpy kullanmalıyız
  w1.temp = 15.0;

  push_stack(&s, &w1);

  // Beklenti: Stack'in top'u (eleman sayısı) 1 olmalı
  ASSERT_EQ(s.top, 1);

  // Şimdi "pop" ile veriyi geri al
  WeatherReading w_out;
  int result = pop_stack(&s, &w_out);

  // Beklenti: Pop işleminin başarılı olması (kodunuza göre 1 döndürmeli)
  ASSERT_EQ(result, 1);
  // Beklenti: Stack'in top'u tekrar 0 olmalı
  ASSERT_EQ(s.top, 0);
  // Beklenti: Çıkan veri, giren veri ile aynı olmalı
  ASSERT_STREQ(w_out.city, "Rize"); // STREQ, string karşılaştırması yapar
  ASSERT_DOUBLE_EQ(w_out.temp, 15.0); // DOUBLE_EQ, ondalıklı sayı karşılaştırması yapar
}

/* * Test Case Adı: PopFromEmptyStack
 * Bu test, boş bir stack'ten pop yapmaya çalışınca ne olduğunu kontrol eder.
 */
TEST(StackTest, PopFromEmptyStack) {
  Stack s;
  init_stack(&s);

  WeatherReading w_out;
  int result = pop_stack(&s, &w_out);

  // Beklenti: Boş stack'ten pop 0 döndürmeli (kodunuza göre)
  ASSERT_EQ(result, 0);
  // Beklenti: Top hala 0 olmalı
  ASSERT_EQ(s.top, 0);
}

/* * Test Case Adı: StackOverflow
 * Bu test, stack'i tamamen doldurup bir daha eklemeye çalışmayı test eder.
 */
TEST(StackTest, StackOverflow) {
  Stack s;
  init_stack(&s);

  WeatherReading w;
  strcpy(w.city, "Test");
  w.temp = 1.0;

  // Stack'i MAX_STACK_SIZE (100) kadar doldur
  for (int i = 0; i < MAX_STACK_SIZE; i++) {
    push_stack(&s, &w);
  }

  // Beklenti: Stack tam dolu olmalı
  ASSERT_EQ(s.top, MAX_STACK_SIZE);

  // Şimdi bir tane daha eklemeyi dene (taşmalı)
  push_stack(&s, &w);

  // Beklenti: Top hala MAX_STACK_SIZE olmalı (artmamalı)
  ASSERT_EQ(s.top, MAX_STACK_SIZE);
}