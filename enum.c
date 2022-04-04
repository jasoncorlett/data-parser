#include "enum.h"
// Genrated code, see bin/gen-enum.pl
char *token_type_name(token_type val) {
	switch (val) {
	case TOKEN_ERROR: return "ERROR";
	case TOKEN_NULL: return "NULL";
	case TOKEN_TRUE: return "TRUE";
	case TOKEN_FALSE: return "FALSE";
	case TOKEN_STRING: return "STRING";
	case TOKEN_NUMBER: return "NUMBER";
	case TOKEN_COMMA: return "COMMA";
	case TOKEN_COLON: return "COLON";
	case TOKEN_EOF: return "EOF";
	case TOKEN_LEFT_BRACE: return "LEFT_BRACE";
	case TOKEN_RIGHT_BRACE: return "RIGHT_BRACE";
	case TOKEN_LEFT_BRACKET: return "LEFT_BRACKET";
	case TOKEN_RIGHT_BRACKET: return "RIGHT_BRACKET";
	default: return "<UNKNOWN token_type>";
	}
}

char *json_type_name(json_type val) {
	switch (val) {
	case JSON_ERROR: return "ERROR";
	case JSON_NULL: return "NULL";
	case JSON_STRING: return "STRING";
	case JSON_BOOLEAN: return "BOOLEAN";
	case JSON_NUMBER: return "NUMBER";
	case JSON_ARRAY: return "ARRAY";
	case JSON_OBJECT: return "OBJECT";
	default: return "<UNKNOWN json_type>";
	}
}
