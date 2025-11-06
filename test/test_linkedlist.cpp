#include <gtest/gtest.h>
#include <stdlib.h> // malloc ve free kullanmak için
#include <cstring>  // (Gelecekte string gerekirse diye)

/* C++ test kodunun, sizin C kütüphanenizi (linkedlist.h)
 * bulabilmesi için bu extern "C" bloğu zorunludur.
 */
extern "C" {
  #include "linkedlist.h"
}

/* Her test için temiz bir liste oluşturmak ve test sonrası
 * belleği temizlemek için bir "Fixture" (test altyapısı) kullanıyoruz.
 */
class LinkedListTest : public ::testing::Test {
protected:
  // Her testin kullanabileceği ortak değişkenler
  Node* head;
  int data1;
  int data2;
  int data3;

  // Her test case'inden ÖNCE bu fonksiyon çalışır
  void SetUp() override {
    // Her testin temiz bir 'head' (NULL) ile başlamasını garanti et
    head = NULL;
    
    // Test verilerini hazırla (adreslerini kullanacağız)
    data1 = 10;
    data2 = 20;
    data3 = 30;
  }

  // Her test case'inden SONRA bu fonksiyon çalışır
  void TearDown() override {
    // Her testten sonra listeyi temizle.
    // Veriler (data1, data2) stack'te olduğu için (malloc'lanmadığı için)
    // free_data_func'a NULL yolluyoruz.
    free_list(&head, NULL);
  }
};

// Test 1: Liste oluşturma (Boş liste)
// Fixture kullandığımız için TEST_F yazıyoruz
TEST_F(LinkedListTest, Initialization) {
  // SetUp() zaten head = NULL yaptı
  ASSERT_EQ(head, nullptr);
}

// Test 2: Tek bir düğüm ekleme (append_node)
TEST_F(LinkedListTest, AppendSingleNode) {
  append_node(&head, &data1);

  // Beklenti: head artık NULL olmamalı
  ASSERT_NE(head, nullptr);
  // Beklenti: Veri doğru olmalı
  ASSERT_EQ(*(static_cast<int*>(head->data)), 10);
  // Beklenti: Tek düğüm olduğu için next ve prev NULL olmalı
  ASSERT_EQ(head->next, nullptr);
  ASSERT_EQ(head->prev, nullptr);
}

// Test 3: Birden fazla düğüm ekleme (append_node) ve yapı kontrolü
TEST_F(LinkedListTest, AppendMultipleNodes) {
  append_node(&head, &data1); // [10]
  append_node(&head, &data2); // [10] -> [20]
  append_node(&head, &data3); // [10] -> [20] -> [30]

  Node* node1 = head;
  Node* node2 = head->next;
  Node* node3 = head->next->next;

  // Beklenti: head (node1) doğru olmalı
  ASSERT_NE(node1, nullptr);
  ASSERT_EQ(*(static_cast<int*>(node1->data)), 10);
  ASSERT_EQ(node1->prev, nullptr); // İlk düğümün 'prev'i NULL olmalı

  // Beklenti: node2 doğru olmalı
  ASSERT_NE(node2, nullptr);
  ASSERT_EQ(*(static_cast<int*>(node2->data)), 20);
  ASSERT_EQ(node2->prev, node1); // node2'nin 'prev'i node1 olmalı

  // Beklenti: node3 doğru olmalı
  ASSERT_NE(node3, nullptr);
  ASSERT_EQ(*(static_cast<int*>(node3->data)), 30);
  ASSERT_EQ(node3->prev, node2); // node3'ün 'prev'i node2 olmalı
  ASSERT_EQ(node3->next, nullptr); // Son düğümün 'next'i NULL olmalı
}

// Test 4: İleri/Geri Navigasyon (Şartnamede istenen use-case)
TEST_F(LinkedListTest, NavigationForwardBackward) {
  append_node(&head, &data1);
  append_node(&head, &data2);
  append_node(&head, &data3);

  Node* current = head->next; // Ortadaki düğümdeyiz (20)
  ASSERT_EQ(*(static_cast<int*>(current->data)), 20);

  // İleri git (go_forward)
  current = go_forward(current);
  ASSERT_NE(current, nullptr);
  ASSERT_EQ(*(static_cast<int*>(current->data)), 30);

  // Geri git (go_backward)
  current = go_backward(current);
  ASSERT_NE(current, nullptr);
  ASSERT_EQ(*(static_cast<int*>(current->data)), 20);
  
  // Tekrar geri git
  current = go_backward(current);
  ASSERT_NE(current, nullptr);
  ASSERT_EQ(*(static_cast<int*>(current->data)), 10);
}

// Test 5: Navigasyon Kenar Durumları (Listenin sonundan/başından gidememe)
TEST_F(LinkedListTest, NavigationEdgeCases) {
  append_node(&head, &data1); // Sadece 1 düğüm var

  // Beklenti: Listenin başındayken geri gidememeli
  Node* new_pos_back = go_backward(head);
  ASSERT_EQ(new_pos_back, head); // Pozisyon değişmemeli

  // Beklenti: Listenin sonundayken ileri gidememeli
  Node* new_pos_fwd = go_forward(head);
  ASSERT_EQ(new_pos_fwd, head); // Pozisyon değişmemeli
}

// C-stili free fonksiyonu (test 6 için gerekli)
static void free_int_ptr(void* data) {
    free(data);
}

// Test 6: Bellek temizleme (free_list) ve veri temizleme
TEST_F(LinkedListTest, FreeListWithData) {
  // Bu sefer verileri heap'e (malloc ile) koyuyoruz
  int* p_data1 = (int*)malloc(sizeof(int)); *p_data1 = 100;
  int* p_data2 = (int*)malloc(sizeof(int)); *p_data2 = 200;
  
  append_node(&head, p_data1);
  append_node(&head, p_data2);
  
  // Beklenti: Düğüm verisi doğru olmalı
  ASSERT_EQ(*(static_cast<int*>(head->next->data)), 200);

  // Şimdi listeyi ve içindeki verileri (int*) temizle
  free_list(&head, free_int_ptr);

  // Beklenti: Head artık NULL olmalı
  ASSERT_EQ(head, nullptr);
}