#pragma warning(disable:4996)
#include "Modulo.h"

#include <time.h>

/*
To test delta-encoding, change 'decompress' in this function to 'decompress2' AND in Modulo.c change 'delta_encoding_and_modulo' to 'delta_encoding' in
'get_long_values_and_compress'.
*/
void test() {
	int number_of_files = 14;
	
	//Files to compress
	char *files[14];
	files[0] = "small_consecutive";
	files[1] = "medium_consecutive";
	files[2] = "large_consecutive";
	files[3] = "small_big_jumps";
	files[4] = "medium_big_jumps";
	files[5] = "big_big_jumps";
	files[6] = "small_small_jumps";
	files[7] = "medium_small_jumps";
	files[8] = "big_small_jumps";
	files[9] = "small_random";
	files[10] = "medium_random";
	files[11] = "big_random";
	files[12] = "small_numbers";
	files[13] = "large_numbers";

	//Results from compression
	char *output[14];
	output[0] = "small_consecutive_o";
	output[1] = "medium_consecutive_o";
	output[2] = "large_consecutive_o";
	output[3] = "small_big_jumps_o";
	output[4] = "medium_big_jumps_o";
	output[5] = "big_big_jumps_o";
	output[6] = "small_small_jumps_o";
	output[7] = "medium_small_jumps_o";
	output[8] = "big_small_jumps_o";
	output[9] = "small_random_o";
	output[10] = "medium_random_o";
	output[11] = "big_random_o";
	output[12] = "small_numbers_o";
	output[13] = "large_numbers_o";

	//Results from decompression
	char *decomp[14];
	decomp[0] = "small_consecutive_d";
	decomp[1] = "medium_consecutive_d";
	decomp[2] = "large_consecutive_d";
	decomp[3] = "small_big_jumps_d";
	decomp[4] = "medium_big_jumps_d";
	decomp[5] = "big_big_jumps_d";
	decomp[6] = "small_small_jumps_d";
	decomp[7] = "medium_small_jumps_d";
	decomp[8] = "big_small_jumps_d";
	decomp[9] = "small_random_d";
	decomp[10] = "medium_random_d";
	decomp[11] = "big_random_d";
	decomp[12] = "small_numbers_d";
	decomp[13] = "large_numbers_d";

	double compr_times[14][10];
	double decompr_times[14][10];

	//Compress each file 10 times and count the time the cpu needs
	for (int i = 0; i < number_of_files; i++) {
		for (int j = 0; j < 10; j++) {
			clock_t begin = clock();
			FILE* in = fopen(files[i], "rb");
			FILE* out = fopen(output[i], "wb");
			get_long_values_and_compress(in, out);
			clock_t end = clock();
			double cpu_time = (double)(end - begin) / CLOCKS_PER_SEC;
			compr_times[i][j] = cpu_time;
			if (j == 0) {
				int original_size = get_file_size(in);
				int compressed_size = get_file_size(out);
				printf("Compression-factor for file%d: %lf\n", i, (double)compressed_size / original_size);
			}
			fclose(in);
			fclose(out);
		}
	}
	
	//Print the average cpu_time for each file for compression
	for (int i = 0; i < number_of_files; i++) {
		double total = compr_times[i][0];
		for (int j = 1; j < 10; j++) {
			total += compr_times[i][j];
		}
		printf("File %d average compression time: %lf\n", i, total/10.0);
	}

	//Decompress each file 10 times and count the time the cpu needs
	for (int i = 0; i < number_of_files; i++) {
		for (int j = 0; j < 10; j++) {
			clock_t begin = clock();
			FILE* in = fopen(output[i], "rb");
			FILE* out = fopen(decomp[i], "wb");
			decompress(in, out);
			clock_t end = clock();
			double cpu_time = (double)(end - begin) / CLOCKS_PER_SEC;
			decompr_times[i][j] = cpu_time;
			int original_size = get_file_size(in);
			int compressed_size = get_file_size(out);
			fclose(in);
			fclose(out);
		}
		printf("Finished file%d\n", i);
	}

	//Print the average cpu_time for each file for decompression
	for (int i = 0; i < number_of_files; i++) {
		double total = decompr_times[i][0];
		for (int j = 1; j < 10; j++) {
			total += decompr_times[i][j];
		}
		printf("File %d average decompression time: %lf\n", i, total / 10.0);
	}
}

void generate_random_numbers(FILE* output, long long size, long long min_jump, long long start) {
	long long counter = 0;
	long long current = start;
	fprintf(output, "%lld,", current);
	while (counter < size) {
		long long jump = 0;
		while (jump < min_jump) {
			jump += rand();
		}
		current = current + jump;
		if (current > LLONG_MAX) {
			printf("number too large!!");
		}
		fprintf(output, "%lld,", current);
		counter++;
	}
}

void generate_files() {
	FILE* f1 = fopen("small_consecutive", "wb");
	fprintf(f1, "[");
	generate_random_numbers(f1, 100000, 10, 1);
	fseek(f1, -1, SEEK_END);
	fprintf(f1, "]");
	fclose(f1);
	printf("f1 ok\n");

	FILE* f2 = fopen("medium_consecutive", "wb");
	fprintf(f2, "[");
	generate_random_numbers(f2, 1000000, 10, 1);
	fseek(f2, -1, SEEK_END);
	fprintf(f2, "]");
	fclose(f2);
	printf("f2 ok\n");

	FILE* f3 = fopen("large_consecutive", "wb");
	fprintf(f3, "[");
	generate_random_numbers(f3, 10000000, 10, 1);
	fseek(f3, -1, SEEK_END);
	fprintf(f3, "]");
	fclose(f3);
	printf("f3 ok\n");

	FILE* f4 = fopen("small_big_jumps", "wb");
	fprintf(f4, "[");
	generate_random_numbers(f4, 100000, 100000, 1);
	fseek(f4, -1, SEEK_END);
	fprintf(f4, "]");
	fclose(f4);
	printf("f4 ok\n");

	FILE* f5 = fopen("medium_big_jumps", "wb");
	fprintf(f5, "[");
	generate_random_numbers(f5, 1000000, 100000, 1);
	fseek(f5, -1, SEEK_END);
	fprintf(f5, "]");
	fclose(f5);
	printf("f5 ok\n");

	FILE* f6 = fopen("big_big_jumps", "wb");
	fprintf(f6, "[");
	generate_random_numbers(f6, 10000000, 100000, 1);
	fseek(f6, -1, SEEK_END);
	fprintf(f6, "]");
	fclose(f6);
	printf("f6 ok\n");

	FILE* f7 = fopen("small_small_jumps", "wb");
	fprintf(f7, "[");
	generate_random_numbers(f7, 100000, 1000, 1);
	fseek(f7, -1, SEEK_END);
	fprintf(f7, "]");
	fclose(f7);
	printf("f7 ok\n");

	FILE* f8 = fopen("medium_small_jumps", "wb");
	fprintf(f8, "[");
	generate_random_numbers(f8, 1000000, 1000, 1);
	fseek(f8, -1, SEEK_END);
	fprintf(f8, "]");
	fclose(f8);
	printf("f8 ok\n");

	FILE* f9 = fopen("big_small_jumps", "wb");
	fprintf(f9, "[");
	generate_random_numbers(f9, 10000000, 1000, 1);
	fseek(f9, -1, SEEK_END);
	fprintf(f9, "]");
	fclose(f9);
	printf("f9 ok\n");

	FILE* f10 = fopen("small_random", "wb");
	fprintf(f10, "[");
	generate_random_numbers(f10, 100000, 1, 1000000);
	fseek(f10, -1, SEEK_END);
	fprintf(f10, "]");
	fclose(f10);
	printf("f10 ok\n");

	FILE* f11 = fopen("medium_random", "wb");
	fprintf(f11, "[");
	generate_random_numbers(f11, 1000000, 1, 1000000);
	fseek(f11, -1, SEEK_END);
	fprintf(f11, "]");
	fclose(f11);
	printf("f11 ok\n");

	FILE* f12 = fopen("big_random", "wb");
	fprintf(f12, "[");
	generate_random_numbers(f12, 10000000, 1, 1000000);
	fseek(f12, -1, SEEK_END);
	fprintf(f12, "]");
	fclose(f12);
	printf("f12 ok\n");

	FILE* f13 = fopen("small_numbers", "wb");
	fprintf(f13, "[");
	generate_random_numbers(f13, 10000000, 1, 1);
	fseek(f13, -1, SEEK_END);
	fprintf(f13, "]");
	fclose(f13);
	printf("f13 ok\n");

	FILE* f14 = fopen("large_numbers", "wb");
	fprintf(f14, "[");
	generate_random_numbers(f14, 10000000, 1, 100000000000);
	fseek(f14, -1, SEEK_END);
	fprintf(f14, "]");
	fclose(f14);
	printf("f14 ok\n");
}