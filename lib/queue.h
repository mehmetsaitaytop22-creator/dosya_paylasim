#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>


typedef struct QNode {
    void* data;
    struct QNode* next;
} QNode;

typedef struct {
    QNode* front;
    QNode* rear;
} Queue;

void init_queue(Queue* q);
bool is_queue_empty(Queue* q);
void enqueue(Queue* q, void* data);
void* dequeue(Queue* q); 

#endif 