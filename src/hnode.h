#ifndef _CZ_HNODE_H_
#define _CZ_HNODE_H_

#include<stdlib.h>


typedef struct HNode
{
    int character;
    size_t freq;
    struct HNode *left;
    struct HNode *right;
} HNode;

HNode *hnode_new(int character, size_t freq)
{
    HNode *newNode = (HNode *)malloc(sizeof(HNode));
    newNode->character = character;
    newNode->freq = freq;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

void hnode_sort(HNode **arrayOfNode, size_t arraySize)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        for (size_t j = i; j < arraySize; j++)
        {
            if (arrayOfNode[i]->freq > arrayOfNode[j]->freq)
            {
                HNode *tmp = arrayOfNode[i];
                arrayOfNode[i] = arrayOfNode[j];
                arrayOfNode[j] = tmp;
            }
        }
    }
}

#endif
