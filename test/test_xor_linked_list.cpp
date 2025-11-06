#include <gtest/gtest.h>
#include <stdint.h> // intptr_t için
#include <stdlib.h> // malloc/free için

/* C++ test kodunun, sizin C kütüphanenizi (xor_linked_list.h)
 * bulabilmesi için bu extern "C" bloğu zorunludur.
 */
extern "C" {
  #include "xor_linked_list.h"
  #include "weather_data.h" // Veri yapısı olarak bunu kullanacağız
}

/* Her test için temiz bir liste oluşturmak ve test sonrası
 * belleği temizlemek için bir "Fixture" (test altyapısı) kullanıyoruz.
 */
class XorListTest : public ::testing::Test {
protected:
  XorNode* head;
  
  // Test verileri (stack'te)
  WeatherReading data1, data2, data3;

  // Her test case'inden ÖNCE bu fonksiyon çalışır
  void SetUp() override {
    head = NULL;
    
    // Test verilerini hazırla (adreslerini kullanacağız)
    strcpy(data1.city, "Birinci"); data1.temp = 10;
    strcpy(data2.city, "Ikinci");  data2.temp = 20;
    strcpy(data3.city, "Ucuncu");  data3.temp = 30;
  }

  // Her test case'inden SONRA bu fonksiyon çalışır
  void TearDown() override {
    // Her testten sonra listeyi temizle.
    // free_xor_list sadece düğümleri (node) free eder, 
    // içindeki veriye dokunmaz. Veriler (data1, 2, 3) stack'te olduğu için
    // bu, bellek sızıntısına yol açmaz.
    free_xor_list(&head);
  }
};

// Test 1: Liste oluşturma (Boş liste)
TEST_F(XorListTest, Initialization) {
  ASSERT_EQ(head, nullptr);
}

// Test 2: Tek bir düğüm ekleme (insert_xor_list)
TEST_F(XorListTest, InsertSingleNode) {
  insert_xor_list(&head, &data1);

  // Beklenti: head artık NULL olmamalı
  ASSERT_NE(head, nullptr);
  // Beklenti: Veri doğru olmalı
  WeatherReading* w = static_cast<WeatherReading*>(head->data);
  ASSERT_STREQ(w->city, "Birinci");
  
  // Beklenti: Tek düğümün xor_ptr'ı (NULL ^ NULL) NULL olmalı
  ASSERT_EQ(head->xor_ptr, xor_op(NULL, NULL));
  ASSERT_EQ(head->xor_ptr, nullptr);
}

// Test 3: Birden fazla düğüm ekleme ve düğüm pointer'larını doğrulama
TEST_F(XorListTest, InsertMultipleNodesAndCheckPointers) {
  insert_xor_list(&head, &data1); // [1]
  insert_xor_list(&head, &data2); // [1] -> [2]
  insert_xor_list(&head, &data3); // [1] -> [2] -> [3]

  XorNode* node1 = head;
  ASSERT_NE(node1, nullptr);
  
  // İleri doğru 2. düğümü bul
  XorNode* node2 = xor_op(NULL, node1->xor_ptr);
  ASSERT_NE(node2, nullptr);

  // İleri doğru 3. düğümü bul
  XorNode* node3 = xor_op(node1, node2->xor_ptr);
  ASSERT_NE(node3, nullptr);

  // Verileri doğrula
  ASSERT_STREQ(static_cast<WeatherReading*>(node1->data)->city, "Birinci");
  ASSERT_STREQ(static_cast<WeatherReading*>(node2->data)->city, "Ikinci");
  ASSERT_STREQ(static_cast<WeatherReading*>(node3->data)->city, "Ucuncu");
  
  // Pointer'ları doğrula
  // node1->xor_ptr = NULL ^ node2
  ASSERT_EQ(node1->xor_ptr, xor_op(NULL, node2));
  // node2->xor_ptr = node1 ^ node3
  ASSERT_EQ(node2->xor_ptr, xor_op(node1, node3));
  // node3->xor_ptr = node2 ^ NULL
  ASSERT_EQ(node3->xor_ptr, xor_op(node2, NULL));
}


// Test 4: İleri Yönlü Gezinme (Forward Traversal)
TEST_F(XorListTest, ForwardTraversal) {
  insert_xor_list(&head, &data1);
  insert_xor_list(&head, &data2);
  insert_xor_list(&head, &data3);

  XorNode* curr = head;
  XorNode* prev = NULL;
  XorNode* next;

  // 1. Düğüm
  ASSERT_NE(curr, nullptr);
  ASSERT_STREQ(static_cast<WeatherReading*>(curr->data)->city, "Birinci");
  next = xor_op(prev, curr->xor_ptr);
  prev = curr;
  curr = next;

  // 2. Düğüm
  ASSERT_NE(curr, nullptr);
  ASSERT_STREQ(static_cast<WeatherReading*>(curr->data)->city, "Ikinci");
  next = xor_op(prev, curr->xor_ptr);
  prev = curr;
  curr = next;

  // 3. Düğüm
  ASSERT_NE(curr, nullptr);
  ASSERT_STREQ(static_cast<WeatherReading*>(curr->data)->city, "Ucuncu");
  next = xor_op(prev, curr->xor_ptr);
  prev = curr;
  curr = next;

  // Liste Sonu
  ASSERT_EQ(curr, nullptr);
}

// Test 5: Geri Yönlü Gezinme (Backward Traversal) - Şartname  için kritik
TEST_F(XorListTest, BackwardTraversal) {
  insert_xor_list(&head, &data1);
  insert_xor_list(&head, &data2);
  insert_xor_list(&head, &data3);

  XorNode* curr = head;
  XorNode* prev = NULL;
  XorNode* next;

  // Önce listenin sonuna gitmeliyiz
  while (curr != NULL) {
    next = xor_op(prev, curr->xor_ptr);
    if (next == NULL) {
      break; // 'curr' şu an son düğüm, 'prev' ondan bir önceki
    }
    prev = curr;
    curr = next;
  }

  // Şimdi 'curr' son düğüm (data3), 'prev' ise bir önceki (data2)
  // Geriye doğru gezinmeye başla
  
  // 3. Düğüm (Sondan başla)
  ASSERT_NE(curr, nullptr);
  ASSERT_STREQ(static_cast<WeatherReading*>(curr->data)->city, "Ucuncu");
  next = xor_op(curr->xor_ptr, NULL); // Bu, geriye doğru ilk adımı atar (prev'i bulur)
  prev = curr;
  curr = next;

  // 2. Düğüm
  ASSERT_NE(curr, nullptr);
  ASSERT_STREQ(static_cast<WeatherReading*>(curr->data)->city, "Ikinci");
  next = xor_op(prev, curr->xor_ptr); // (prev=node3, curr=node2, curr->xor_ptr = node1^node3) -> node1
  prev = curr;
  curr = next;
  
  // 1. Düğüm
  ASSERT_NE(curr, nullptr);
  ASSERT_STREQ(static_cast<WeatherReading*>(curr->data)->city, "Birinci");
  next = xor_op(prev, curr->xor_ptr); // (prev=node2, curr=node1, curr->xor_ptr = NULL^node2) -> NULL
  prev = curr;
  curr = next;

  // Liste Başı
  ASSERT_EQ(curr, nullptr);
}