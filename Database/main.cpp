#include<iostream> 
#include<stdio.h>
#include<stdlib.h>

typedef signed int ssize_t;

typedef struct 
{
	char* buffer;
	size_t buffer_length;
	ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer()
{
	InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
	input_buffer->buffer = NULL;
	input_buffer->buffer_length = 0;
	input_buffer->input_length = 0;
	
	return input_buffer;
}

void print_prompt()
{
	printf("db >");
}

// to read a line of input use 
// getline(char **lineptr, size_t *n, FILE *stream);
// **lineptr -> a pointer to the var that point to the buffer containing read line
// *n -> a pointer to the var that keep size of allocated buffer
// *stream -> input stream to read from (will be use a stdin) 

void read_input(InputBuffer* input_buffer)
{
	ssize_t bytes_read =
		getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

	if (bytes_read <= 0)
	{
		printf("Error reading input");
		exit(EXIT_FAILURE);
	}
	
	//ignore trailing newline
	input_buffer->input_length = bytes_read - 1; // last byte always '\0'
	input_buffer->buffer[bytes_read - 1] = 0;
}

// proper to do a fucntion that frees memory allocated for an
// instance of  InputBuffer*
void close_input_buffer(InputBuffer* input_buffer)
{
	free(input_buffer->buffer);
	free(input_buffer);
}
// sqlite read-execute-print loop 
int main(int argc, char **argv)
{
	InputBuffer* input_buffer = new_input_buffer();

	while (true)
	{
		print_prompt();
		read_input(input_buffer);
		
		if (strcmp(input_buffer->buffer, "exit") == 0)
		{
			close_input_buffer(input_buffer);
			exit(EXIT_SUCCESS);
		}
		else
		{
			printf("Unrecognized command %s", input_buffer->buffer);
		}
	}
}