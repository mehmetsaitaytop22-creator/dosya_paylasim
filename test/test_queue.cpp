#include <gtest/gtest.h>

/* C++ test kodunun, sizin C kütüphanenizi (queue.h)
 * bulabilmesi için bu extern "C" bloğu zorunludur.
 */
extern "C" {
  #include "queue.h"
}

// Test 1: Kuyruğun başlatılmasını ve boş olup olmadığını kontrol et
TEST(QueueTest, Initialization) {
  Queue q;
  init_queue(&q);
  
  // Beklenti: Kuyruk boş başlamalı
  ASSERT_TRUE(is_queue_empty(&q));
  // Beklenti: front ve rear göstericileri NULL (C++'da nullptr) olmalı
  ASSERT_EQ(q.front, nullptr);
  ASSERT_EQ(q.rear, nullptr);
}

// Test 2: Tek bir eleman ekleme (enqueue)
TEST(QueueTest, EnqueueSingleItem) {
  Queue q;
  init_queue(&q);
  
  // Test verisi (int'in adresini void* olarak atıyoruz)
  int data1 = 10;
  enqueue(&q, &data1);

  // Beklenti: Kuyruk artık boş olmamalı
  ASSERT_FALSE(is_queue_empty(&q));
  // Beklenti: front ve rear göstericileri dolu olmalı
  ASSERT_NE(q.front, nullptr);
  ASSERT_NE(q.rear, nullptr);
  // Beklenti: Sadece bir eleman olduğunda front ve rear aynı düğümü göstermeli
  ASSERT_EQ(q.front, q.rear);
  
  // Beklenti: Geriye int'e cast et ve değeri kontrol et
  ASSERT_EQ(*(static_cast<int*>(q.front->data)), 10);
}

// Test 3: Ekleme ve çıkarma (enqueue/dequeue)
TEST(QueueTest, EnqueueAndDequeue) {
  Queue q;
  init_queue(&q);

  int data1 = 20;
  enqueue(&q, &data1);

  // Kuyruktan çıkar
  void* data_out_ptr = dequeue(&q);

  // Beklenti: Çıkan veri NULL olmamalı
  ASSERT_NE(data_out_ptr, nullptr);
  // Beklenti: Çıkan verinin değeri 20 olmalı
  ASSERT_EQ(*(static_cast<int*>(data_out_ptr)), 20);

  // Beklenti: Kuyruğun tekrar boş olduğunu kontrol et
  ASSERT_TRUE(is_queue_empty(&q));
  ASSERT_EQ(q.front, nullptr);
  ASSERT_EQ(q.rear, nullptr);
}

// Test 4: FIFO (First-In-First-Out) sırasını test et
TEST(QueueTest, FIFOOrder) {
  Queue q;
  init_queue(&q);

  int data1 = 10;
  int data2 = 20;
  int data3 = 30;

  enqueue(&q, &data1); // Kuyruk: [10]
  enqueue(&q, &data2); // Kuyruk: [10, 20]
  enqueue(&q, &data3); // Kuyruk: [10, 20, 30]

  // Beklenti: İlk giren (10) ilk çıkmalı
  void* data_out1 = dequeue(&q);
  ASSERT_EQ(*(static_cast<int*>(data_out1)), 10);

  // Beklenti: İkinci giren (20) ikinci çıkmalı
  void* data_out2 = dequeue(&q);
  ASSERT_EQ(*(static_cast<int*>(data_out2)), 20);
  
  // Beklenti: Son giren (30) son çıkmalı
  void* data_out3 = dequeue(&q);
  ASSERT_EQ(*(static_cast<int*>(data_out3)), 30);

  // Beklenti: Kuyruk boş olmalı
  ASSERT_TRUE(is_queue_empty(&q));
}

// Test 5: Boş kuyruktan çıkarma (dequeue)
TEST(QueueTest, DequeueFromEmpty) {
  Queue q;
  init_queue(&q);

  // Boş kuyruktan çıkarmayı dene
  void* data_out = dequeue(&q);

  // Beklenti: Kodunuza göre NULL (nullptr) dönmeli
  ASSERT_EQ(data_out, nullptr);
  ASSERT_TRUE(is_queue_empty(&q));
}