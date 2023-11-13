#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "src/huffmann.h"
#include "src/header.h"

#define COUNT 5

void print2DUtil(HNode *root, int space)
{
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += COUNT;

    // Process right child first
    print2DUtil(root->right, space);

    // Print current node after space
    // count
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%c\n", root->character);

    // Process left child
    print2DUtil(root->left, space);
}

void hnode_print_P(HNode *tree)
{
    print2DUtil(tree, 0);
}

void display_usage(const char *appName)
{
    fprintf(stderr,
            "[Usage]: %s [Options] <filename>\n\n"
            " Options:\n"
            "       -c        Compress the file\n"
            "       -d        Decompress the file\n"
            "       -h        Display this help message\n",
            appName);

    exit(EXIT_FAILURE);
}

void cz_compress_file(const char *fileName)
{
    size_t map[MAP_SIZE] = {0};
    char *codes[MAP_SIZE] = {0};

    FILE *fp = fopen(fileName, "r");

    if (fp == NULL)
    {
        perror(fileName);
        exit(EXIT_FAILURE);
    }

    // find the number of character
    size_t unS = huffmann_get_frequencies(fp, map);

    fseek(fp, SEEK_SET, 0); // Reset file pointer to start of file

    // For DEBUG purpose
    // for (size_t i = 0; i < currentCharacter; i++)
    // {
    //     printf("%c:%d\n", filteredCharacterMap[i], filteredDataMap[i]);
    // }

    HNode *tree = huffmann_tree_generate(map, unS);

    huffmann_generate_codes(tree, codes);

    // For Debug of codes
    // for (size_t i = 0; i < MAP_SIZE; i++)
    // {
    //     if (codes[i] != NULL)
    //         printf("%d  : %s\n", i, codes[i]);
    // }

    FILE *outFile = huffmann_get_compress_output_file(fileName); // Get the output file handler

    header_write_to_file(outFile, tree);

    huffmann_encode_file(fp, outFile, codes);
}

void cz_decompress_file(const char *fileName)
{
    printf("[INFO] Decompressing file %s\n", fileName);
    FILE *fp = fopen(fileName, "r");

    if (fp == NULL)
    {
        perror(fileName);
        exit(EXIT_FAILURE);
    }

    uint8_t magicNumber = (uint8_t)getc(fp);

    // Check the magic number for validation
    if (magicNumber != MAGIC_NUMBER)
    {
        fprintf(stderr, "[ERROR] %s file is Invalid compressed ! (Magic number doesnot match)\n Provide valid compressed file\n", fileName);
        exit(EXIT_FAILURE);
    }

    HNode *tree = header_read_from_file(fp);
    // hnode_print_P(tree);
    FILE *outfile = huffmann_get_decompress_output_file(fileName);

    huffmann_decode_file(fp, outfile, tree);
}

int main(int argc, char const *argv[])
{

    if (argc < 3 || argv[1][0] != '-')
        display_usage(argv[0]);

    if (strcmp(argv[1], "-c") == 0)
        cz_compress_file(argv[2]);
    else if (strcmp(argv[1], "-d") == 0)
        cz_decompress_file(argv[2]);
    else if (strcmp(argv[1], "-h") == 0)
        display_usage(argv[0]);
    else
    {
        fprintf(stderr, "%s options is not supported !!\n", argv[1]);
        display_usage(argv[0]);
    }

    // FILE *fp = fopen(argv[1], "r");

    // if (fp == NULL)
    // {
    //     perror(argv[0]);
    //     exit(EXIT_FAILURE);
    // }

    // FILE *fp1 = fopen("test.txt.huff", "r");
    // if (fp1 == NULL)
    // {
    //     perror("fopen");
    //     exit(EXIT_FAILURE);
    // }

    // huffmann_decode_file(fp1, argv[1], tree);

    return 0;
}
