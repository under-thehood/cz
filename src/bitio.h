#ifndef _CZ_BITIO_H_
#define _CZ_BITIO_H_
#include <stdint.h>
#include <stdio.h>

uint8_t write_bit_to_file(FILE *fp, uint8_t bit)
{

    static uint8_t count;
    static uint8_t bitStream;

    bitStream = bitStream << 1;
    if (bit)
        bitStream = bitStream | 1;

    count++;

    if (count == 8)
    {
        fputc(bitStream, fp);
        count = 0;
        bitStream = 0;
    }
    return count;
}

void write_byte_to_file(FILE *fp, uint8_t byte)
{
    for (size_t i = 0; i < 8; i++)
    {
        write_bit_to_file(fp, byte & 0x80);
        byte = byte << 1;
    }
}

uint8_t read_bit_from_file(FILE *fp)
{
    static uint8_t data = 0;
    static uint8_t count = 8;
    if (count == 8)
    {

        data = (uint8_t)fgetc(fp);
        count = 0;
    }

    uint8_t bit = data & 0x80;

    data = data << 1;
    count++;
    return bit == 0 ? 0 : 1;
}

uint8_t read_byte_from_file(FILE *fp)
{
    uint8_t byte = 0;
    for (size_t i = 0; i < 7; i++)
    {
        byte = byte | read_bit_from_file(fp);
        byte = byte << 1;
    }
    byte = byte | read_bit_from_file(fp);
    return byte;
}

#endif
