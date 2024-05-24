#ifndef DYNAMIC_QUEUE_H
#define DYNAMIC_QUEUE_H

// data structure for the dynamic queue
typedef struct {
    double *buffer;   // dynamic array to store the elements
    int max_capacity;   // maximum capacity of the array
    int size;         // current size of the array

} DynamicQueue;

// Init function for the dynamic queue
void initQueue(DynamicQueue *queue);

// Insert an element in the dynamic queue
void enqueue(DynamicQueue *queue, double item);

// Empty the dynamic queue
void freeQueue(DynamicQueue *queue);

#endif 