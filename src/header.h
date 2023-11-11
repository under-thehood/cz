#ifndef _CZ_HEADER_H_
#define _CZ_HEADER_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "queue.h"

#define MAGIC_NUMBER (uint8_t)0x10101010

typedef struct
{
    uint8_t magicNumber;
    uint8_t *bitStream;
    size_t arraySize;
    size_t arrayCapacity;
} Header;

#define INITIAL_HEADER_SIZE 64

uint8_t header_put_bit(Header *header, uint8_t bit)
{

    static uint8_t count;
    static uint8_t bitStream;

    bitStream = bitStream << 1;
    if (bit == 1)
        bitStream = bitStream | 1;

    count++;

    if (count == 8)
    {
        if (header->arraySize + 1 >= header->arrayCapacity)
        {
            header->arrayCapacity = header->arrayCapacity + 10; // Only increate the capacity by ten
            header->bitStream = (uint8_t *)realloc(header->bitStream, header->arrayCapacity);

            if (header->bitStream == NULL)
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
        }
        header->bitStream[header->arraySize++] = bitStream;
        count = 0;
        bitStream = 0;
    }
    return count;
}

void header_put_byte(Header *header, uint8_t byte)
{
    for (size_t i = 0; i < 8; i++)
    {
        header_put_bit(header, byte & 0x80);
        byte = byte << 1;
    }
}

void header_generate(Header *header, HQueue *queue)
{
    while (!hqueue_is_empty(queue))
    {
        HNode *tree = hqueue_dequeue(queue);

        assert(tree != NULL);

        if (tree->left == NULL && tree->right == NULL) // Leaf node gives the 10 bit and give character byte
        {
            if (tree->character == -1) // It represent the end of the file give 11 bit
            {
                // printf("EOF\n");

                header_put_bit(header, 1);
                header_put_bit(header, 1);
            }
            else
            {
                // printf("%c\n", tree->character);

                header_put_bit(header, 1);
                header_put_bit(header, 0);

                header_put_byte(header, tree->character);
            }
        }
        else
        {

            header_put_bit(header, 0);
            hqueue_enqueue(queue, tree->right);
            hqueue_enqueue(queue, tree->left);
        }
    }
    while (header_put_bit(header, 0))
    {
    }
}

bool header_init(Header *header)
{

    header->magicNumber = MAGIC_NUMBER;
    header->arraySize = 0;
    header->bitStream = (uint8_t *)malloc(sizeof(uint8_t) * INITIAL_HEADER_SIZE);
    if (header->bitStream == NULL)
    {
        perror("malloc");
        return false;
    }
    header->arrayCapacity = INITIAL_HEADER_SIZE;
    return true;
}

bool header_from_tree(Header *header, HNode *tree)
{

    HQueue queue;
    hqueue_init(&queue, 28);

    hqueue_enqueue(&queue, tree);

    header_generate(header, &queue);

    hqueue_free(&queue);

    return true;
}

void header_free(Header *header)
{
    free(header->bitStream);
    header->arrayCapacity = 0;
    header->arraySize = 0;
}

void header_write_to_file(Header *header, FILE *file)
{
    fseek(file, 0, SEEK_SET);
    fputc(header->magicNumber, file);

    size_t writeByte = fwrite(header->bitStream, sizeof(*(header->bitStream)), header->arraySize, file);

    assert(writeByte + 1 != header->arraySize && "Cannot write the header to the file");
}

#endif