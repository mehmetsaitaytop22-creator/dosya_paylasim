#include <gtest/gtest.h>
#include <stdlib.h> // calloc/malloc/free için

/* C++ test kodunun, sizin C kütüphanenizi (sparse_matrix.h)
 * bulabilmesi için bu extern "C" bloğu zorunludur.
 */
extern "C" {
  #include "sparse_matrix.h"
}

/* Her test için 10x10'luk temiz bir matris oluşturmak ve test sonrası
 * belleği temizlemek için bir "Fixture" (test altyapısı) kullanıyoruz.
 */
class SparseMatrixTest : public ::testing::Test {
protected:
  SparseMatrix* sm;

  // Her test case'inden ÖNCE bu fonksiyon çalışır
  void SetUp() override {
    // Her test için 10x10'luk temiz bir matris oluştur
    sm = create_sparse_matrix(10, 10);
  }

  // Her test case'inden SONRA bu fonksiyon çalışır
  void TearDown() override {
    // Her testten sonra matrisin belleğini serbest bırak
    free_sparse_matrix(sm);
  }
};

// Test 1: Matrisin başarıyla başlatıldığını test et
TEST_F(SparseMatrixTest, Initialization) {
  // Beklenti: Boyutlar doğru ayarlanmış olmalı
  ASSERT_NE(sm, nullptr);
  ASSERT_EQ(sm->max_rows, 10);
  ASSERT_EQ(sm->max_cols, 10);

  // Beklenti: create_sparse_matrix 'calloc' kullandığı için
  // tüm satır ve sütun başlıkları NULL (nullptr) olmalı.
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(sm->row_heads[i], nullptr);
    ASSERT_EQ(sm->col_heads[i], nullptr);
  }
}

// Test 2: Değer ekleme (Insert) ve Değer alma (Get)
TEST_F(SparseMatrixTest, InsertAndRetrieve) {
  insert_sparse_matrix(sm, 2, 3, 42.5);
  
  // Beklenti: Eklenen değeri doğru almalıyız
  double val = get_sparse_matrix_value(sm, 2, 3);
  ASSERT_DOUBLE_EQ(val, 42.5);
}

// Test 3: Boş bir hücreden değer alma
TEST_F(SparseMatrixTest, RetrieveEmptyCell) {
  insert_sparse_matrix(sm, 5, 5, 99.0);
  
  // Beklenti: Hiç değer atanmamış [0, 0] hücresi 0.0 dönmeli
  double val = get_sparse_matrix_value(sm, 0, 0);
  ASSERT_DOUBLE_EQ(val, 0.0);
  
  // Beklenti: Değer olan satırdaki boş bir hücre (5, 6) 0.0 dönmeli
  double val2 = get_sparse_matrix_value(sm, 5, 6);
  ASSERT_DOUBLE_EQ(val2, 0.0);
}

// Test 4: Matris sınırları dışından değer alma
TEST_F(SparseMatrixTest, RetrieveOutOfBounds) {
  // Beklenti: Sınırların dışındaki bir hücre 0.0 dönmeli
  double val = get_sparse_matrix_value(sm, 20, 20); // 10x10 matris
  ASSERT_DOUBLE_EQ(val, 0.0);
}

// Test 5: Mevcut bir değeri güncelleme
TEST_F(SparseMatrixTest, InsertAndUpdateValue) {
  insert_sparse_matrix(sm, 4, 4, 1.0);
  
  // Beklenti: İlk değer doğru olmalı
  ASSERT_DOUBLE_EQ(get_sparse_matrix_value(sm, 4, 4), 1.0);

  // Şimdi aynı hücreye yeni bir değer ekle
  insert_sparse_matrix(sm, 4, 4, 2.0);
  
  // Beklenti: Değer güncellenmiş olmalı
  ASSERT_DOUBLE_EQ(get_sparse_matrix_value(sm, 4, 4), 2.0);
}

// Test 6: Sıfır (0.0) değeri eklemeyi deneme (Şartname için kritik)
TEST_F(SparseMatrixTest, InsertZeroValue) {
  // Beklenti: Kodunuz 0.0 değerini eklemeyi ignore etmeli
  insert_sparse_matrix(sm, 7, 7, 0.0);
  
  // Beklenti: Değer 0.0 olarak okunmalı
  ASSERT_DOUBLE_EQ(get_sparse_matrix_value(sm, 7, 7), 0.0);
  
  // Beklenti (Daha önemlisi): 0.0 eklendiğinde yeni bir düğüm OLUŞTURULMAMALI.
  // Bu yüzden o satırın başı hala NULL (nullptr) olmalı.
  ASSERT_EQ(sm->row_heads[7], nullptr);
  ASSERT_EQ(sm->col_heads[7], nullptr);
}

// Test 7: Satır ve Sütun bağlantılarının bütünlüğünü test etme
TEST_F(SparseMatrixTest, CheckRowAndColLinks) {
  // Aynı satıra iki eleman ekle
  insert_sparse_matrix(sm, 3, 1, 10.0); // [3, 1]
  insert_sparse_matrix(sm, 3, 5, 20.0); // [3, 5] (Sırayla eklenmeli)

  // Aynı sütuna iki eleman ekle
  insert_sparse_matrix(sm, 1, 5, 30.0); // [1, 5]
  
  // Düğümleri bulalım
  // <--- DÜZELTME BURADA: 'Node*' yerine 'SM_Node*' kullanıldı
  SM_Node* node_3_1 = sm->row_heads[3];
  SM_Node* node_3_5 = sm->row_heads[3]->next_in_row;
  SM_Node* node_1_5 = sm->col_heads[5];

  // --- Satır (Row) Kontrolü ---
  // Beklenti: Satır 3'ün başı [3, 1] olmalı
  ASSERT_NE(node_3_1, nullptr);
  ASSERT_DOUBLE_EQ(node_3_1->value, 10.0);
  
  // Beklenti: [3, 1]'in satırdaki devamı [3, 5] olmalı
  ASSERT_NE(node_3_5, nullptr);
  ASSERT_EQ(node_3_5, node_3_1->next_in_row);
  ASSERT_DOUBLE_EQ(node_3_5->value, 20.0);
  ASSERT_EQ(node_3_5->next_in_row, nullptr); // Satırın sonu

  // --- Sütun (Column) Kontrolü ---
  // Beklenti: Sütun 5'in başı [1, 5] olmalı
  ASSERT_NE(node_1_5, nullptr);
  ASSERT_DOUBLE_EQ(node_1_5->value, 30.0);
  
  // Beklenti: [1, 5]'in sütundaki devamı [3, 5] olmalı
  ASSERT_NE(node_1_5->next_in_col, nullptr);
  ASSERT_EQ(node_1_5->next_in_col, node_3_5);
  ASSERT_EQ(node_3_5->next_in_col, nullptr); // Sütunun sonu
}