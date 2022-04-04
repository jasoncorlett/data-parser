#pragma once

// Genrated code, see bin/gen-enum.pl
typedef enum {
	TOKEN_ERROR = -1,
	TOKEN_NULL,
	TOKEN_TRUE,
	TOKEN_FALSE,
	TOKEN_STRING,
	TOKEN_NUMBER,
	TOKEN_COMMA,
	TOKEN_COLON,
	TOKEN_EOF,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_LEFT_BRACKET,
	TOKEN_RIGHT_BRACKET,
} token_type;

char *token_type_name(token_type val);

typedef enum {
	JSON_ERROR = -1,
	JSON_NULL,
	JSON_STRING,
	JSON_BOOLEAN,
	JSON_NUMBER,
	JSON_ARRAY,
	JSON_OBJECT,
} json_type;

char *json_type_name(json_type val);

