#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef signed int ssize_t;

// Exceptions are bad => C doesn't support them 

typedef enum
{
	META_COMMAND_SUCCESS,
	META_COMMAND_UNRECOGNIZED_COMMAND

} MetaCommandResult; // one of that return 

typedef enum
{
	PREPARE_SUCCESS,
	PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum
{
	STATEMENT_INSERT,
	STATEMENT_SELECT
} StatementType;

typedef struct
{
	StatementType type;
} Statement;

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

MetaCommandResult do_meta_command(InputBuffer* input_buffer)
{
	if (strcmp(input_buffer->buffer, ".exit") == 0)
		exit(EXIT_SUCCESS);
	else
		return META_COMMAND_UNRECOGNIZED_COMMAND;
}

// function for understand SQL commands 
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement)
{
	// strncmp but only for n-bytes, return 0 if n-bytes 
	// identical because after insert following data
	if (strncmp(input_buffer->buffer, "insert", 6 /* number of bytes */) == 0)
	{
		statement->type = STATEMENT_INSERT;
		return PREPARE_SUCCESS;
	}
	if (strcmp(input_buffer->buffer, "select") == 0)
	{
		statement->type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}

	return PREPARE_UNRECOGNIZED_STATEMENT;
}

void
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

		// Non-SQL statements like ".exit" called meta-commands -> all start with a dot 
		if (input_buffer->buffer[0] == '.')
		{
			switch (do_meta_command(input_buffer))
			{
			case (META_COMMAND_SUCCESS):
				continue;
			case (META_COMMAND_UNRECOGNIZED_COMMAND):
				printf("Unrecongnized command '%s'\n", input_buffer->buffer);
				continue;
			}
		}

		// process of convert line of input into an internal representation 
		Statement statement;
		switch (prepare_statement(input_buffer, &statement))
		{
		case (PREPARE_SUCCESS):
			break;
		case (PREPARE_UNRECOGNIZED_STATEMENT):
			printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
			continue;
		}

		execute_statement(&statement);
		printf("Executed.\n");
	}
}