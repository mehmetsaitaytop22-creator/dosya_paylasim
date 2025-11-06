#include <gtest/gtest.h>
#include <cstring> // strcpy ve strcmp için

/* C++ test kodunun, sizin C kütüphanenizi (hash_table.h)
 * bulabilmesi için bu extern "C" bloğu zorunludur.
 */
extern "C" {
  #include "hash_table.h"
}

/* Test suiti için bir "fixture" (hazırlık) sınıfı oluşturabiliriz.
 * Bu, her testten önce ve sonra otomatik olarak çalışan kod sağlar.
 * Bu durumda, her testin temiz bir hash tablosuyla başlamasını garanti eder.
 */
class HashTableTest : public ::testing::Test {
protected:
  HashTable ht; // Testlerde kullanılacak hash tablosu
  UserData user1;
  UserData user2;
  UserData user_out; // Arama sonuçları için

  // Her test case'inden ÖNCE bu fonksiyon çalışır
  void SetUp() override {
    // Testin temiz bir tablo ile başlamasını garanti et
    init_hash_table(&ht);
    
    // Test verilerini hazırla
    strcpy(user1.username, "admin");
    strcpy(user1.password_hash, "pass123");
    user1.access_level = 2;

    strcpy(user2.username, "salih");
    strcpy(user2.password_hash, "gizli456");
    user2.access_level = 1;
  }

  // Her test case'inden SONRA bu fonksiyon çalışır
  // void TearDown() override {
  //   // Not: Hash tablosu stack üzerinde (statik) olduğu için
  //   // ve düğümlerini (node) delete_hash_table ile temizlediğimiz için
  //   // burada özel bir TearDown'a gerek yok.
  // }
};

// Test Case 1: Tablonun başarıyla başlatıldığını test et
// Fixture'ı kullanmak için TEST yerine TEST_F yazıyoruz
TEST_F(HashTableTest, Initialization) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    // Beklenti: Tüm indeksler NULL (C++'da nullptr) olarak başlamalı
    ASSERT_EQ(ht.table[i], nullptr);
  }
}

// Test Case 2: Yeni bir kullanıcı eklemeyi ve aramayı test et
TEST_F(HashTableTest, InsertAndSearchNew) {
  // "admin" kullanıcısını ekle
  bool insert_result = insert_hash_table(&ht, user1.username, &user1);
  
  // Beklenti: Ekleme başarılı olmalı (true dönmeli)
  ASSERT_TRUE(insert_result);

  // Şimdi "admin" kullanıcısını ara
  bool search_result = search_hash_table(&ht, "admin", &user_out);

  // Beklenti: Arama başarılı olmalı
  ASSERT_TRUE(search_result);
  // Beklenti: Bulunan veri, eklenen veri ile aynı olmalı
  ASSERT_STREQ(user_out.username, "admin");
  ASSERT_STREQ(user_out.password_hash, "pass123");
  ASSERT_EQ(user_out.access_level, 2);
}

// Test Case 3: Var olmayan bir kullanıcıyı aramayı test et
TEST_F(HashTableTest, SearchNonExistent) {
  bool search_result = search_hash_table(&ht, "kimse_yok", &user_out);
  
  // Beklenti: Arama başarısız olmalı (false dönmeli)
  ASSERT_FALSE(search_result);
}

// Test Case 4: Mevcut bir kullanıcıyı güncellemeyi test et
TEST_F(HashTableTest, InsertAndUpdate) {
  // "admin" kullanıcısını (level 2) ekle
  insert_hash_table(&ht, user1.username, &user1);

  // "admin" için yeni veri (level 5) oluştur
  UserData user_updated;
  strcpy(user_updated.username, "admin");
  strcpy(user_updated.password_hash, "yeni_sifre");
  user_updated.access_level = 5;

  // insert_hash_table'ı aynı anahtarla tekrar çağır
  bool update_result = insert_hash_table(&ht, "admin", &user_updated);

  // Beklenti: Güncelleme başarılı olmalı
  ASSERT_TRUE(update_result);

  // "admin"i ara ve verinin güncellendiğini doğrula
  bool search_result = search_hash_table(&ht, "admin", &user_out);
  ASSERT_TRUE(search_result);
  ASSERT_EQ(user_out.access_level, 5);
  ASSERT_STREQ(user_out.password_hash, "yeni_sifre");
}

// Test Case 5: Bir kullanıcıyı silmeyi test et
TEST_F(HashTableTest, DeleteExisting) {
  insert_hash_table(&ht, user1.username, &user1);

  // Silmeden önce var olduğunu doğrula
  ASSERT_TRUE(search_hash_table(&ht, "admin", &user_out));

  // Kullanıcıyı sil
  bool delete_result = delete_hash_table(&ht, "admin");
  
  // Beklenti: Silme işlemi başarılı olmalı
  ASSERT_TRUE(delete_result);

  // Sildikten sonra artık bulunamadığını doğrula
  ASSERT_FALSE(search_hash_table(&ht, "admin", &user_out));
}

// Test Case 6: Var olmayan bir kullanıcıyı silmeyi deneme
TEST_F(HashTableTest, DeleteNonExistent) {
  bool delete_result = delete_hash_table(&ht, "kimse_yok");
  
  // Beklenti: Silme işlemi başarısız olmalı
  ASSERT_FALSE(delete_result);
}

// Test Case 7: Hash çakışmasını (collision) test et
// Bu test, çakışma olsa bile (iki anahtar aynı indekse düşse bile)
// verilerin doğru şekilde eklendiğini ve arandığını doğrular.
TEST_F(HashTableTest, CollisionHandling) {
  // İki farklı kullanıcıyı da ekle
  insert_hash_table(&ht, user1.username, &user1); // "admin"
  insert_hash_table(&ht, user2.username, &user2); // "salih"

  // "admin"i ara
  bool search1 = search_hash_table(&ht, "admin", &user_out);
  ASSERT_TRUE(search1);
  ASSERT_STREQ(user_out.username, "admin");
  ASSERT_EQ(user_out.access_level, 2);

  // "salih"i ara
  bool search2 = search_hash_table(&ht, "salih", &user_out);
  ASSERT_TRUE(search2);
  ASSERT_STREQ(user_out.username, "salih");
  ASSERT_EQ(user_out.access_level, 1);
  
  // Şimdi birini sil (admin'i)
  delete_hash_table(&ht, "admin");

  // Diğerinin (salih'in) hala erişilebilir olduğunu doğrula
  bool search3 = search_hash_table(&ht, "salih", &user_out);
  ASSERT_TRUE(search3);
  ASSERT_STREQ(user_out.username, "salih");
  
  // "admin"in gerçekten silindiğini doğrula
  ASSERT_FALSE(search_hash_table(&ht, "admin", &user_out));
}