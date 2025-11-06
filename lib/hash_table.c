#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void init_hash_table(HashTable* ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->table[i] = NULL;
    }
}


unsigned int hash_function(const char* key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash % TABLE_SIZE;
}


static HashNode* create_hash_node(const char* key, UserData* data) {
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (!new_node) {
        perror("HashNode bellek tahsis hatasi");
        return NULL;
    }
    strcpy(new_node->key, key);
    new_node->data = *data;
    new_node->next = NULL;
    return new_node;
}


bool insert_hash_table(HashTable* ht, const char* key, UserData* data) {
    unsigned int index = hash_function(key);
    
    HashNode* current = ht->table[index];
    HashNode* prev = NULL;

    
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->data = *data; 
            return true;
        }
        prev = current;
        current = current->next;
    }

    
    HashNode* new_node = create_hash_node(key, data);
    if (!new_node) return false;

    if (prev == NULL) {
        
        ht->table[index] = new_node;
    } else {
        
        prev->next = new_node;
    }
    return true;
}


bool search_hash_table(HashTable* ht, const char* key, UserData* out_data) {
    unsigned int index = hash_function(key);
    HashNode* current = ht->table[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            *out_data = current->data; 
            return true;
        }
        current = current->next;
    }
    return false; 
}


bool delete_hash_table(HashTable* ht, const char* key) {
    unsigned int index = hash_function(key);
    HashNode* current = ht->table[index];
    HashNode* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                
                ht->table[index] = current->next;
            } else {
                
                prev->next = current->next;
            }
            free(current); 
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false; 
}