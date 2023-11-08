#ifndef _CZ_HUFFMANN_H_
#define _CZ_HUFFMANN_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define MAP_SIZE 256

typedef struct HNode
{
    char character;
    size_t freq;
    struct HNode *left;
    struct HNode *right;
} HNode;

HNode *hnode_new(char character, size_t freq)
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

size_t huffmann_get_frequencies(FILE *fp, size_t map[])
{
    // find the number of character
    int currentCharacter = fgetc(fp);
    size_t numberOfUniqueCharacter = 0;
    while (currentCharacter != EOF)
    {
        if (map[currentCharacter] == 0)
            numberOfUniqueCharacter++;
        map[currentCharacter] += 1;
        currentCharacter = fgetc(fp);
    }
    return numberOfUniqueCharacter;
}

void hnode_print(HNode **arrayOfNode, size_t arraySize)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        printf("%c :  %d\n", arrayOfNode[i]->character, arrayOfNode[i]->freq);
    }
}

void traverse_tree(HNode *tree, char *codes[], char *code, size_t codeIndex)
{
    assert(tree != NULL && "Corrupt huffman tree is provided");

    if (tree->left == NULL && tree->right == NULL)
    {
        code[codeIndex] = 0;
        codes[tree->character] = strdup(code);
    }
    else
    {
        code[codeIndex] = '0';
        traverse_tree(tree->left, codes, code, codeIndex + 1);

        code[codeIndex] = '1';
        traverse_tree(tree->right, codes, code, codeIndex + 1);
    }
}

void huffmann_generate_codes(HNode *tree, char *codes[])
{
    char code[MAP_SIZE] = {0};
    traverse_tree(tree, codes, code, 0);
    // code[0] = '0';
    // traverse_tree(tree->left, codes, code, 1);
    // memset(code, 0, MAP_SIZE);

    // traverse_tree(tree->right, codes, code, 1);
}

HNode *huffmann_tree_generate(size_t *map, size_t sizeOfArray)
{

    sizeOfArray++;
    HNode **arrayOfNode = (HNode **)malloc(sizeof(HNode *) * sizeOfArray);
    HNode **freeIngNode = arrayOfNode;
    HNode *result;

    arrayOfNode[0] = hnode_new(0, 0);

    size_t j = 1;

    for (size_t i = 0; i < MAP_SIZE; i++)
    {
        if (map[i] != 0)
        {
            arrayOfNode[j] = hnode_new(i, map[i]);
            j++;
        }
    }

    for (; sizeOfArray != 1;)
    {
        hnode_sort(arrayOfNode, sizeOfArray);

        // For debug Only
        // printf("=================\n");
        // hnode_print(arrayOfNode, sizeOfArray);

        HNode *new = hnode_new('*', arrayOfNode[0]->freq + arrayOfNode[1]->freq);

        new->left = arrayOfNode[0];
        new->right = arrayOfNode[1];

        arrayOfNode++;

        arrayOfNode[0] = new;

        --sizeOfArray;
    }
    result = arrayOfNode[0];

    free(freeIngNode);

    return result;
}

char huffmann_out(FILE *file, char bit)
{
    static char count;
    static char bitStream;

    bitStream = bitStream << 1;
    if (bit == '1')
        bitStream = bitStream | 1;

    count++;

    if (count == 8)
    {
        fputc(bitStream, file);
        count = 0;
        bitStream = 0;
    }
    return count;
}

#define CZ_FILENAME_MAXIMUM 100

void huffmann_encode_file(FILE *inFile, const char *inFileName, char *codes[])
{

    char outFileName[CZ_FILENAME_MAXIMUM];

    snprintf(outFileName, CZ_FILENAME_MAXIMUM, "%s.huff", inFileName);

    FILE *outFile = fopen(outFileName, "w");

    if (outFile == NULL)
    {
        fprintf(stderr, "[ERROR] Cannot create the filename %s\n", outFileName);
        exit(EXIT_FAILURE);
    }

    int data = fgetc(inFile);

    while (data != EOF)
    {
        char *codeOfChar = codes[data];

        for (; *codeOfChar != 0; codeOfChar++)
        {
            huffmann_out(outFile, *codeOfChar);
        }

        data = fgetc(inFile);
    }
    while (huffmann_out(outFile, '0'))
    {
    }
    fclose(outFile);
}

void huffmann_decode_file(FILE *file, const char *inFileName, HNode *tree)
{

    char *outputFileName = strdup(inFileName);

    outputFileName[strlen(outputFileName) - 3] = '\0';

    FILE *outFile = fopen(outputFileName, "w");

    if (outFile == NULL)
    {
        fprintf(stderr, "[ERROR] Cannot create the filename test.c\n");
        exit(EXIT_FAILURE);
    }

    int in = getc(file);
    char byteRep = 0;
    HNode *nodeIndex = tree;
    while (in != EOF)
    {
        byteRep = (char)in;

        for (size_t i = 0; i < 8; i++)
        {
            // if the anding of the character with b1000 0000 is true then,
            if (byteRep & 0x80)
                nodeIndex = nodeIndex->right; // 1 go right
            else
                nodeIndex = nodeIndex->left; // else go left

            if (nodeIndex->left == NULL || nodeIndex->right == NULL)
            {                                         // until a leaf meets
                putchar(nodeIndex->character);        // spit that character out and
                fputc(nodeIndex->character, outFile); // save the character in file
                nodeIndex = tree;                     // reset the nodeIndex
            }
            byteRep = byteRep << 1; // shift the character by 1
        }
        in = getc(file);
    }
}

#endif