#include "Common.h"

void set_bit(unsigned char* arr, int k) {
	arr[k / CHAR_BIT] = arr[k / CHAR_BIT] | (0x01 << (CHAR_BIT - 1 - (k%CHAR_BIT)));
}

void clear_bit(unsigned char* arr, int k) {
	arr[k / CHAR_BIT] = arr[k / CHAR_BIT] & ~(0x01 << (CHAR_BIT - 1 - (k%CHAR_BIT)));
}

int test_bit(unsigned char* arr, int k) {
	return (arr[k / CHAR_BIT] & (0x01 << (CHAR_BIT - 1 - (k%CHAR_BIT)))) != 0;
}

int test_bit_in_char(unsigned char c, int k) {
	return (c & (0x01 << (CHAR_BIT - 1 - k))) != 0;
}

void write_bit(unsigned char* buffer, int index, int binary_number) {
	if (binary_number == 1) {
		set_bit(buffer, index);
	}
	else {
		clear_bit(buffer, index);
	}
}

int get_file_size(FILE* file) {
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	rewind(file);
	return size;
}