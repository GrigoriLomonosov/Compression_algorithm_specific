#pragma warning(disable:4996)
//Code to check for memory leaks
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include <time.h>
#include <math.h>
#include <float.h>

#include "Modulo.h"
#include "TestSuite.h"

int main(int argc, char *argv[]) {
	printf("Running: Specific algorithm\n");
	if (argc == 4) {
		//Check if files can be opened
		FILE* input;
		input = fopen(argv[2], "rb");
		if (!input) {
			printf("ERROR: Could not open file: %s!\n", argv[2]);
			exit(1);
		}
		FILE* output;
		output = fopen(argv[3], "wb");
		if (!output) {
			printf("ERROR: Could not open file: %s!\n", argv[3]);
			exit(1);
		}

		//Compression
		if (strcmp("-c", argv[1]) == 0) {
			//Start the timing
			clock_t begin = clock();

			int original_size = get_file_size(input);
			printf("Original file size: %d\n", original_size);

			//COMPRESS HERE
			get_long_values_and_compress(input, output);
			printf("Compresseed\n");

			int compressed_size = get_file_size(output);
			printf("Compressed file size: %d\n", compressed_size);
			printf("Compression-factor: %lf\n", (double)compressed_size / original_size);

			//Calculate the time needed to compress
			clock_t end = clock();
			double cpu_time = (double)(end - begin) / CLOCKS_PER_SEC;
			printf("Used CPU-time: %lf", cpu_time);
		}

		//Decompression
		else if (strcmp("-d", argv[1]) == 0) {
			//Start the timing
			clock_t begin = clock();

			//DECOMPRESS HERE
			decompress2(input, output);
			printf("decompressed\n");

			//Calculate the time needed to dcompress
			clock_t end = clock();
			double cpu_time = (double)(end - begin) / CLOCKS_PER_SEC;
			printf("Used CPU-time: %lf", cpu_time);
		}

		//Test
		else if (strcmp("-t", argv[1]) == 0) {
			test();
		}

		//Clean up
		fclose(input);
		fclose(output);
	}
	else {
		printf("3 arguments should be given in the following order: [-c or -d] [inputfile] [outputfile]");
	}
	getchar();
	return 0;
#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
#endif
}