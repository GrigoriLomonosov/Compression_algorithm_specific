#ifndef MODULO_H
#define MODULO_H

#include <ctype.h>

#include "Common.h"

/*
Gets the long long values from a file and compresses these values based on delta_encoding_and_modulo (see below). The results are written to the output file.
*/
void get_long_values_and_compress(FILE* input, FILE* output);

/*
Delta-encodes an array of long longs and writes the results to the file output. The last long long value from a previous buffer is given as parameter.
*/
void delta_encoding(unsigned long long* input, int arr_length, FILE* output, long long from_previous_buffer);

/*
First the array of long longs is delta-encoded. Secondly each long long is divided bij 255 until zero is reached. The modulo of each division is stored in an unsigned char.
These chars then form a string that represents the number. Finally these strings are written to the file output.
*/
void delta_encoding_and_modulo(unsigned long long* input, int arr_length, FILE* output, long long from_previous_buffer);

/*
Decompresses the data in the input file based on the delta_encoding_and_modulo algorithm. The results are written to the output file.
*/
void decompress(FILE* input, FILE* output);

/*
Decompresses the data in the input file based on simple delta_encoding. The results are written to the output file.
*/
void decompress2(FILE* input, FILE* output);

#endif