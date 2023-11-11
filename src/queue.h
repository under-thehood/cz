#ifndef _CZ_QUEUE_H_
#define _CZ_QUEUE_H_

#include "hnode.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
    HNode **array;
    int size;
    int pointer;
    size_t capacity;
} HQueue;

bool hqueue_init(HQueue *queue, size_t capacity)
{
    // Initializw the size and the pointer to -1
    queue->size = -1;
    queue->pointer = -1;
    queue->capacity = capacity;

    queue->array = (typeof(queue->array))malloc(sizeof(HNode *) * capacity);

    if (queue->array == NULL)
    {
        perror("malloc");
        queue->capacity = 0;
        return false;
    }

    return true;
}

bool hqueue_enqueue(HQueue *queue, HNode *node)
{
    queue->size = (queue->size + 1) % queue->capacity;

    if (queue->size == queue->pointer) // Check for the full
    {
        fprintf(stderr, "[ERROR] Unable to enqueue the data ! Queue is full\n");
        return false;
    }
    if (queue->pointer == -1)
        queue->pointer = 0;
    queue->array[queue->size] = node;
    return true;
}
bool hqueue_is_empty(HQueue *queue)
{
    return queue->pointer == -1;
}
HNode *hqueue_dequeue(HQueue *queue)
{

    HNode *node;
    if (hqueue_is_empty(queue)) // Check for the empty
    {
        fprintf(stderr, "[ERROR] Unable to dequeue the data ! Queue is Empty\n");
        return NULL;
    }

    node = queue->array[queue->pointer];

    if (queue->pointer == queue->size)
    {
        queue->pointer = -1;
        queue->size = -1;
    }
    else
    {
        queue->pointer = (queue->pointer + 1) % queue->capacity;
    }
    return node;
}

void hqueue_free(HQueue *queue)
{
    free(queue->array);
    queue->size = -1;
    queue->capacity = 0;
    queue->pointer = -1;
}
#endif
