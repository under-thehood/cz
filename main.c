// #include "src/fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "src/huffmann.h"

void display_usage(const char *appName)
{
    fprintf(stderr, "[Usage]: %s <filename>\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
    size_t map[MAP_SIZE] = {0};
    char *codes[MAP_SIZE] = {0};

    if (argc < 2)
        display_usage(argv[0]);

    // Open the file
    FILE *fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        perror(argv[0]);
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
    huffmann_encode_file(fp, argv[1], codes);

    FILE *fp1 = fopen("test.txt.huff", "r");
    if (fp1 == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    huffmann_decode_file(fp1, argv[1], tree);

    return 0;
}
