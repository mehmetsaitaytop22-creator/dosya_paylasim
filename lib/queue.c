#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

void init_queue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}

bool is_queue_empty(Queue* q) {
    return q->front == NULL;
}


void enqueue(Queue* q, void* data) {
    QNode* new_node = (QNode*)malloc(sizeof(QNode));
    if (!new_node) {
        perror("Queue node bellek tahsis hatasi");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    if (q->rear == NULL) { 
        q->front = new_node;
        q->rear = new_node;
    } else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
}


void* dequeue(Queue* q) {
    if (is_queue_empty(q)) {
        return NULL;
    }

    QNode* temp = q->front;
    void* data = temp->data;

    q->front = q->front->next;

    if (q->front == NULL) { 
        q->rear = NULL;
    }

    free(temp);
    return data;
}