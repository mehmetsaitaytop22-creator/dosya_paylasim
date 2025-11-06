#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>

#define KEY_SIZE 64
#define TABLE_SIZE 100 


typedef struct {
    char username[KEY_SIZE];
    char password_hash[64];
    int access_level;
} UserData;


typedef struct HashNode {
    char key[KEY_SIZE];
    UserData data;
    struct HashNode* next;
} HashNode;


typedef struct {
    HashNode* table[TABLE_SIZE];
} HashTable;


void init_hash_table(HashTable* ht);
unsigned int hash_function(const char* key);
bool insert_hash_table(HashTable* ht, const char* key, UserData* data);
bool search_hash_table(HashTable* ht, const char* key, UserData* out_data);
bool delete_hash_table(HashTable* ht, const char* key);

#endif 