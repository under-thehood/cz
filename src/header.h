#ifndef _CZ_HEADER_H_
#define _CZ_HEADER_H_

// #include <stdint.h>
// #include <stddef.h>
// #include <string.h>
// #include <stdio.h>
// #include <assert.h>
#include "queue.h"
#include "bitio.h"

#define MAGIC_NUMBER (uint8_t)0x10101010

void encode_tree_to_file(HNode *tree, FILE *file)
{
    HQueue queue;

    hqueue_init(&queue, 28);

    hqueue_enqueue(&queue, tree);

    while (!hqueue_is_empty(&queue))
    {
        HNode *tree = hqueue_dequeue(&queue);

        assert(tree != NULL);

        if (tree->left == NULL && tree->right == NULL) // Leaf node gives the 10 bit and give character byte
        {
            if (tree->character == -1) // It represent the end of the file give 11 bit
            {
                write_bit_to_file(file, 1);
                write_bit_to_file(file, 1);
            }
            else
            {
                write_bit_to_file(file, 1);
                write_bit_to_file(file, 0);
                write_byte_to_file(file, tree->character);
            }
        }
        else
        {

            write_bit_to_file(file, 0);
            hqueue_enqueue(&queue, tree->left);
            hqueue_enqueue(&queue, tree->right);
        }
    }
    while (write_bit_to_file(file, 0))
    {
    }

    hqueue_free(&queue);
}

#define NONLEAFNODE_FREQ 23 // ANYTHING
#define LEAFNODE_FREQ 22    // ANYTHING

HNode *decode_tree_from_file(FILE *file)
{

    HQueue queue;
    HNode *root;
    hqueue_init(&queue, 28);

    if (read_bit_from_file(file) == 1)
    {

        /*
         Its is case of leaf node and also leaf node is also of two type
         10 := represent normal leaf node
         11 := represent EOF leaf node
          */
        if (read_bit_from_file(file) == 1)
        {
            root = hnode_new(-1, LEAFNODE_FREQ);
        }
        else
        {
            // NOTE: It wll never happen
        }
    }
    else
    {

        root = hnode_new('*', NONLEAFNODE_FREQ);
        hqueue_enqueue(&queue, root);
        HNode *rootP, *node;
        rootP = hqueue_dequeue(&queue);

        enum mode
        {
            NODE_SIDE_LEFT_MODE,
            NODE_SIDE_RIGHT_MODE
        };
        enum mode mod = NODE_SIDE_LEFT_MODE;

        while (1)
        {

            if (read_bit_from_file(file) == 1)
            {

                if (read_bit_from_file(file) == 1)
                {
                    // printf("leaf node 11\n");
                    node = hnode_new(-1, LEAFNODE_FREQ);
                }
                else
                {
                    uint8_t byte = read_byte_from_file(file);
                    // printf("leaf node %c\n", byte);

                    node = hnode_new(byte, LEAFNODE_FREQ);
                }
            }
            else
            {
                // printf("Non leaf node 0\n");

                node = hnode_new('*', NONLEAFNODE_FREQ);
                hqueue_enqueue(&queue, node);
            }

            if (mod == NODE_SIDE_LEFT_MODE)
            {
                rootP->left = node;
                mod = NODE_SIDE_RIGHT_MODE;
            }
            else
            {

                rootP->right = node;
                if (!hqueue_is_empty(&queue))
                    rootP = hqueue_dequeue(&queue);
                else
                    return root;
                mod = NODE_SIDE_LEFT_MODE;
            }
        }
    }
    hqueue_free(&queue);
    return root;
}

void header_write_to_file(FILE *file, HNode *tree)
{
    fseek(file, 0, SEEK_SET);
    fputc(MAGIC_NUMBER, file);

    encode_tree_to_file(tree, file);
}

HNode *header_read_from_file(FILE *file)
{
    fseek(file, 0, SEEK_SET);
    uint8_t magicNumber = (uint8_t)getc(file);

    // Check the magic number for validation
    if (magicNumber != MAGIC_NUMBER)
    {
        fprintf(stderr, "[ERROR]  file is Invalid compressed ! (Magic number doesnot match)\n Provide valid compressed file\n");
        exit(EXIT_FAILURE);
    }

    HNode *tree = decode_tree_from_file(file);

    return tree;
}
#endif