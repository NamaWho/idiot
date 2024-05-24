#include "dynamic_queue.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 24 


void initQueue(DynamicQueue *queue) {
    queue->buffer = (double *)malloc(MAX_SIZE * sizeof(double));

    if (queue->buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    queue->max_capacity = MAX_SIZE;
    queue->size = 0;
}


void enqueue(DynamicQueue *queue, double item) {
    
    if (queue->size == queue->max_capacity) {

        freeQueue(queue); //free the queue
        
        initQueue(queue); //reinitialize the queue
    }

    // Insert the element in the queue
    queue->buffer[queue->size++] = item;
}


void freeQueue(DynamicQueue *queue) {
    free(queue->buffer);
}
