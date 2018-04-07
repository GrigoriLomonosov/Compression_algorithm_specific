#pragma warning(disable:4996)
#include "Modulo.h"

//Helper-function to write a long long value, followed by a comma in a char-array
void write_long_in_buffer(int* write_index, char* write_buffer, long long value) {
	unsigned int length;
	if (value == 0) {
		length = 1;
	}
	else {
		length = (int)floor(log10(value)+1);
	}
	for (int i = length - 1; i >= 0; i--) {
		write_buffer[*write_index + i] = (value % 10) + '0';
		value = value / 10;
	}
	write_buffer[*write_index + length] = ',';
	*write_index = *write_index + length + 1;
}

//Helper-function that writes the original numbers to a file, based on a delta and the previous number
long long write_result_from_delta(FILE* output, long long* input, int arr_length, long long from_previous_buffer) {
	char* write_buffer = malloc(sizeof(char)*BUFFER_SIZE * 20);
	int* write_index = malloc(sizeof(int));
	*write_index = 0;

	//Calculate the original numbers
	long long result[BUFFER_SIZE];
	result[0] = llabs(from_previous_buffer + input[0]);
	write_long_in_buffer(write_index, write_buffer, result[0]);
	for (int i = 1; i < arr_length; i++) {
		result[i] = (input[i] + result[i - 1]);
		write_long_in_buffer(write_index, write_buffer, result[i]);
	}

	//Write the results
	fwrite(write_buffer, 1, *write_index, output);

	//Free the memory
	free(write_buffer);
	free(write_index);

	//Return the last value, so the first element of the next buffer can use it to calculate the correct number based on its delta
	return result[arr_length - 1];
}

//Helper function that calculates the number of needed buffers to read all the chars in the file
int calc_number_of_buffers(FILE* input) {
	fseek(input, 0, SEEK_END);
	int filelen = ftell(input);
	rewind(input);
	return (filelen + BUFFER_SIZE - 1) / BUFFER_SIZE;
}

void get_long_values_and_compress(FILE* input, FILE* output) {
	//Calculate the number of needed buffers
	int number_of_buffers = calc_number_of_buffers(input);
	char* read_buffer = malloc(sizeof(char)*BUFFER_SIZE);
	char* write_buffer = malloc(sizeof(char)*BUFFER_SIZE);

	//The array of long longs to store what is in the file
	long long* long_arr = malloc(sizeof(long long)*BUFFER_SIZE);

	//The actual compression
	int arr_counter = 0;
	long long temp_long = 0;
	long long to_next_buffer = 0;
	for (int i = 0; i < number_of_buffers; i++) {
		int read = fread(read_buffer, 1, BUFFER_SIZE, input);
		for (int j = 0; j < read; j++) {
			char current_char = read_buffer[j];
			if (current_char == ',' || current_char == ']') {
				long_arr[arr_counter] = temp_long;
				arr_counter++;
				temp_long = 0;
				if (arr_counter == BUFFER_SIZE || (i==(number_of_buffers-1) && j == (read-1))) {
					//process the longs
					delta_encoding_and_modulo(long_arr, arr_counter, output, to_next_buffer);
					//delta_encoding(long_arr, arr_counter, output, to_next_buffer);
					if (i != 0) {
						to_next_buffer = long_arr[arr_counter-1];
					}
					//reset array
					arr_counter = 0;
				}
			}
			else if (isdigit(current_char)) {
				temp_long = (temp_long * 10) + (current_char - '0');
			}
		}
	}

	//Free the memory
	free(read_buffer);
	free(write_buffer);
	free(long_arr);
}

void decompress(FILE* input, FILE* output) {
	//Write the first char
	fwrite("[", 1, 1, output);

	//Calculate the number of needed buffers
	int number_of_buffers = calc_number_of_buffers(input);
	unsigned char* read_buffer = malloc(sizeof(char)*BUFFER_SIZE);
	unsigned char* write_buffer = malloc(sizeof(char)*BUFFER_SIZE);

	//The array of longs to store the result
	long long* long_arr = malloc(sizeof(long long)*BUFFER_SIZE);

	//The actual decompression
	int arr_counter = 0;
	long long temp_long = 0;
	long long to_next_buffer = 0;
	for (int i = 0; i < number_of_buffers; i++) {
		int read = fread(read_buffer, 1, BUFFER_SIZE, input);
		for (int j = 0; j < read; j++) {
			int value = (int) read_buffer[j];
			if (value == DIVISOR) {
				long_arr[arr_counter] = temp_long;
				arr_counter++;
				if (arr_counter == BUFFER_SIZE || (i == (number_of_buffers - 1) && j == (read - 1))) {
					to_next_buffer = write_result_from_delta(output, long_arr, arr_counter, to_next_buffer);
					arr_counter = 0;
				}
				temp_long = 0;
			}
			else {
				temp_long = ((temp_long * DIVISOR) + value);
			}
		}
	}

	//Free the memory
	free(read_buffer);
	free(write_buffer);

	//Remove last comma and replace it by a bracket
	fseek(output, -1, SEEK_END);
	fwrite("]", 1, 1, output);
}

void delta_encoding_and_modulo(unsigned long long* input, int arr_length, FILE* output, long long from_previous_buffer) {
	//Calculate the deltas
	long long result[BUFFER_SIZE];
	result[0] = llabs(from_previous_buffer - input[0]);
	for (int i = 1; i < arr_length; i++) {
		result[i] = (input[i] - input[i - 1]);
	}

	//The length of 1 string is maximum 8 (+1 for '\0') because LLONG_MAX can only be divided 8 times before reaching zero
	unsigned char* write_buffer = malloc(sizeof(unsigned char)*BUFFER_SIZE * 9);
	int write_index = 0;

	//Calculate the modulo-strings
	unsigned char temp;
	for (int i = 0; i < arr_length; i++) {
		int counter = 0;
		unsigned char temp_buffer[9];
		while (result[i] > 0) {
			temp = (unsigned char)(result[i] % DIVISOR);
			temp_buffer[counter] = temp;
			counter++;
			result[i] = result[i] / DIVISOR;
		}
		for (int j = 1; j <= counter; j++) {
			write_buffer[write_index] = temp_buffer[counter - j];
			write_index++;
		}
		write_buffer[write_index] = (unsigned char)DIVISOR;
		write_index++;
	}
	
	//Write the results
	fwrite(write_buffer, 1, write_index, output);

	//Free the memory
	free(write_buffer);
}

void delta_encoding(unsigned long long* input, int arr_length, FILE* output, long long from_previous_buffer) {
	char* write_buffer = malloc(sizeof(char)*BUFFER_SIZE * 20);
	int* write_index = malloc(sizeof(int));
	*write_index = 0;

	//Calculate the differences
	long long result[BUFFER_SIZE];
	result[0] = llabs(from_previous_buffer - input[0]);
	write_long_in_buffer(write_index, write_buffer, result[0]);
	for (int i = 1; i < arr_length; i++) {
		result[i] = (input[i] - input[i-1]);
		write_long_in_buffer(write_index, write_buffer, result[i]);
	}
	
	//Write the results
	fwrite(write_buffer, 1, *write_index, output);

	//Free the memory
	free(write_buffer);
	free(write_index);
}

void decompress2(FILE* input, FILE* output) {
	//Write the first char
	fwrite("[", 1, 1, output);

	//Calculate the number of needed buffers
	int number_of_buffers = calc_number_of_buffers(input);
	unsigned char* read_buffer = malloc(sizeof(char)*BUFFER_SIZE);
	unsigned char* write_buffer = malloc(sizeof(char)*BUFFER_SIZE);

	//The array of longs to store the result
	long long* long_arr = malloc(sizeof(long long)*BUFFER_SIZE);

	//The actual decompression
	int arr_counter = 0;
	long long temp_long = 0;
	long long to_next_buffer = 0;
	for (int i = 0; i < number_of_buffers; i++) {
		int read = fread(read_buffer, 1, BUFFER_SIZE, input);
		for (int j = 0; j < read; j++) {
			unsigned char c = read_buffer[j];
			if (c == ',') {
				long_arr[arr_counter] = temp_long;
				arr_counter++;
				if (arr_counter == BUFFER_SIZE || (i == (number_of_buffers - 1) && j == (read - 1))) {
					to_next_buffer = write_result_from_delta(output, long_arr, arr_counter, to_next_buffer);
					arr_counter = 0;
				}
				temp_long = 0;
			}
			else {
				temp_long = ((temp_long * 10) + c - '0');
			}
		}
	}

	//Free the memory
	free(read_buffer);
	free(write_buffer);

	//Remove last comma and replace it by a bracket
	fseek(output, -1, SEEK_END);
	fwrite("]", 1, 1, output);
}