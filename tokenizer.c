#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tokenizer.h"

#define TRUE 1
#define FALSE 0

#define FAIL 0
#define SUCCESS 1

#define WHITESPACE_CHAR " "
#define EOS_CHAR '\0'

static int find_first_char(const char* str);
static void print_tokens(char** tokens);
static int save_token(char** tokens, const char* token, int position);
/*
 * Function to split a string separated by spaces into free_tokens
 */

char** str_tokenizer(char* str)
{
	const char* auxStr = NULL;
	char** tokens = NULL;
	char* token = NULL;
	int items = 0;
	int pos = 0;

	pos = find_first_char(str);

	auxStr = str + pos;
	printf("> line: %s\n", str);
	printf("> first character found at position: %d\n", pos);
	token = strtok(auxStr, WHITESPACE_CHAR);
	
	while (token != NULL) {
		printf("> token found: %s\n", token);
		items++;
		tokens = (char**)realloc(tokens, sizeof(char*)*items);
		save_token(tokens, token, items-1);
		token = strtok(NULL, WHITESPACE_CHAR);
	}
	save_token(tokens, NULL, items);
	print_tokens(tokens);
	return tokens;
}

static int save_token(char** tokens, const char* token, int position)
{
	int length;
	
	if (position < 0 || tokens == NULL) {
		return FAIL;
	}
	
	if (token == NULL) {
		tokens[position] = NULL;
	} else {
		length = strlen(token);
		tokens[position] = (char*)malloc(sizeof(char)*length+1);
		strncpy(tokens[position], token, length);
		tokens[position][length] = '\0';
	}

	return SUCCESS;
}

static void print_tokens(char** tokens)
{
	int pos = 0;

	while (tokens[pos] != NULL) {
		printf("[%d] > %s\n", pos, tokens[pos]);
		pos++;
	}
}

static int find_first_char(const char* str) 
{
	int i;

	for (i = 0; i < strlen(str); i++) {
		if (str[i] != ' ') {
			return i;
		}
	}

	return FALSE;
}

void free_tokens(char** strings)
{
	int i = 0;

	while (!strings[i]) {
		free(strings[i]);
	}

	free(strings);
} 
