
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "json.h"
#include "util.h"
#include "node.h"

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

typedef struct token token;

typedef struct {
    double value;
    int decimals;
} literal_number;

struct token {
    token_type type;
    union {
        char *string;
        double number;
    } value;
    token *next;
};

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

double tokenize_number(char **input) {
    double result = 0;
    double decimalPos = 10;
    int decimalCount = 0;
    bool decimal = false;
    bool negative = false;

    if (**input == '-') {
        negative = true;
        ++*input;
    }

    while (true) {
        if ( **input >= '0' && **input <= '9' ) {
            int digit = **input - '0';
            if (decimal) {
                result += (digit / decimalPos);
                decimalPos *= 10;
                decimalCount++;
            }
            else {
                result *= 10;
                result += digit;
            }
        }
        else if ( **input == '.' ) {
            if (decimal) break;
            decimal = true;
        }
        else {
            break;
        }
        ++*input;
    }

    if (negative) result *= -1;

    return result;
}

//   input = address of address of first character
//  *input = address of first character (start of string)
// **input = first character
char *tokenize_string(char **input) {
    ++*input; // opening quote
    char *start = *input;
    do {
        ++*input;
    } while ( **input != '\0' && **input != '"' );

    if (**input == '\0') {
        return NULL;
    }
    size_t length = *input - start;
    ++*input;
    char *str = calloc(1, length + 1);
    strncpy(str, start, length);
    return str;
}

// Append token to the list and return the newly inserted tail element.
//   list = address of address of head token
//  *list = address of head token (can be null!)
// **list = head token
token *append_token(token **list, token_type type) {
    token *t;
    if (*list == NULL) {
        *list = calloc(1, sizeof(token));
        t = *list;
    }
    else {
        t = *list;
        while (t->next != NULL) {
            t = t->next;
        }
        t->next = calloc(1, sizeof(token));
        t = t->next;
    }
    t->type = type;
    return t;
}


// Append a token to the list with a string value
// value must be a valid (null-terminated) cstring
token *append_token_string(token **list, token_type type, char *value) {
    size_t length = strlen(value);
    token *t = append_token(list, type);
    t->value.string = calloc(1, length + 1);
    strncpy(t->value.string, value, length);
    return t;
}

// Append a token to the list with a numeric value
token *append_token_number(token **list, token_type type, double value) {
    token *t = append_token(list, type);
    t->value.number = value;
    return t;
}

void print_token(token t) {
    switch (t.type) {
        case TOKEN_NUMBER:
            printf("%s %f\n", token_type_name(t.type), t.value.number);
            break;
        case TOKEN_STRING:
        case TOKEN_ERROR:
            if (t.value.string != NULL) {
                printf("%s \"%s\"\n", token_type_name(t.type), t.value.string);
                break;
            }
            __attribute__((fallthrough));
        default:
            printf("%s\n", token_type_name(t.type));
    }
}

void print_token_list(token *t) {
    while (t != NULL) {
        print_token(*t);
        t = t->next;
    }
}

void free_token_list(token **list) {
    if ((*list)->next != NULL) {
        free_token_list(&((*list)->next));
    }
    if (((*list)->type == TOKEN_STRING || ((*list)->type == TOKEN_ERROR)) && (*list)->value.string != NULL) {
        free((*list)->value.string);
    }
    free(*list);
}

token *tokenize(char *input) {
    token *list = NULL;

    while (*input != '\0') {
        switch (*input) {
        case '{': append_token(&list, TOKEN_LEFT_BRACE); ++input; break;
        case '}': append_token(&list, TOKEN_RIGHT_BRACE); ++input; break;
        case '[': append_token(&list, TOKEN_LEFT_BRACKET); ++input; break;
        case ']': append_token(&list, TOKEN_RIGHT_BRACKET); ++input; break;
        case ':': append_token(&list, TOKEN_COLON); ++input; break;
        case ',': append_token(&list, TOKEN_COMMA); ++input; break;
        case '"': {
            char *str = tokenize_string(&input);
            append_token_string(&list, TOKEN_STRING, str);
            break;
        }
        case 't':
            if (input[1] == 'r' && input[2] == 'u' && input[3] == 'e') {
                append_token(&list, TOKEN_TRUE);
                input += 4;
            }
            break;
        case 'f':
            if (input[1] == 'a' && input[2] == 'l' && input[3] == 's' && input[4] == 'e') {
                append_token(&list, TOKEN_FALSE);
                input += 5;
            }
            break;
        case 'n':
            if (input[1] == 'u' && input[2] == 'l' && input[3] == 'l') {
                append_token(&list, TOKEN_NULL);
                input += 4;
            }
            break;
        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': case '0': case '-': {
            double value = tokenize_number(&input);
            append_token_number(&list, TOKEN_NUMBER, value);
            break;
        }
        case '\r': case '\n': case ' ': ++input; continue;
        default: {
            free_token_list(&list);
            #define MSG "Invalid character at "
            #define LEN (sizeof(MSG) + 3)
            char *str = malloc(LEN);
            snprintf(str, LEN, MSG "'%c'", *input);
            return append_token_string(&list, TOKEN_ERROR, str);
        }
            
        }
    }

    return list;
}

Node parse_next_token(token *token);

Node parse_error(char *message, token at) {
    char *str = malloc(1);
    str[0] = '\0';
    if (message != NULL) {
        string_append(&str, message);
    }
    else if (at.type != TOKEN_ERROR) {
        string_append(&str, "ERROR: ");
    }

    if (at.type == TOKEN_ERROR && at.value.string != NULL) {
        if (message != NULL) string_append(&str, " - ");
        string_append(&str, at.value.string);
    }
    else {
        string_append(&str, " (");
        string_append(&str, token_type_name(at.type));
        if (at.type == TOKEN_STRING) {
            string_append(&str, ": ");
            if (at.value.string != NULL) string_append(&str, at.value.string);
            else string_append(&str, "<NULL STRING>");
        }
        else if (at.type == TOKEN_NUMBER) {
            string_append(&str, ": ");
            string_append(&str, double_to_string(at.value.number));
        }
        string_append(&str, ")");
    }
    return node_create_error(str);
}

Node parse_array(token *token) {
    Node array = *node_create_array();

    token = token->next; // left bracket
    while (token != NULL && token->type != TOKEN_RIGHT_BRACKET) {
        Node node = parse_next_token(token);
        node_array_append(array, node);
        token = token->next;

        if (token != NULL && token->type == TOKEN_COMMA) {
            token = token->next;
        }
        else {
            break;
        }
    }

    if (token == NULL) return node_create_error("Expected ] at EOF");
    if (token->type != TOKEN_RIGHT_BRACKET) return node_create_error("Expected ]");
    token = token->next; // right bracket

    return array;
}

Node parse_object(token *token) {
    Node object = *node_create_map();

    token = token->next; // left brace
    
    while (token != NULL && token->type != TOKEN_RIGHT_BRACE) {
        if (token->type != TOKEN_STRING) {
            return parse_error("Expected key", *token);
        }

        size_t key_size = strlen(token->value.string) + 1;
        char *key = malloc(key_size);
        strncpy(key, token->value.string, key_size);

        token = token->next;
        if (token == NULL || token->type != TOKEN_COLON) {
            return parse_error("Expected colon", *token);
        }

        token = token->next;
        if (token == NULL) {
            return parse_error("Expected object value", *token);
        }
        Node value = parse_next_token(token);
        node_map_set(object, key, value);

        token = token->next;
        if (token != NULL && token->type == TOKEN_COMMA) {
            token = token->next;
        }
        else {
            break;
        }
    }

    if (token == NULL) return parse_error("Expected } at EOF", *token);
    if (token->type != TOKEN_RIGHT_BRACE) return parse_error("Expected }", *token);

    token = token->next;

    return object;
}

Node parse_next_token(token *token) {
    switch (token->type) {
    case TOKEN_LEFT_BRACE:
        return parse_object(token);
    case TOKEN_LEFT_BRACKET:
        return parse_array(token);
    case TOKEN_NUMBER:
        return node_create_number(token->value.number);
    case TOKEN_STRING:
        return node_create_string(token->value.string);
    case TOKEN_TRUE:
        return node_create_boolean(true);
    case TOKEN_FALSE:
        return node_create_boolean(false);
    case TOKEN_NULL:
        return node_create_null();
    case TOKEN_ERROR:
        return parse_error(NULL, *token);
    default:
        return parse_error("Unknown token type!", *token);
    }
        
}


char *json_node_to_string_internal(Node node, char *pad, int indent) {
    bool pretty = pad != NULL;
    
    switch (node.type) {
    case NODE_TYPE_NULL:    return copy_to_heap_string("null");
    case NODE_TYPE_STRING:  return string_quote(node.as.string);
    case NODE_TYPE_BOOLEAN: return copy_to_heap_string(node.as.boolean ? "true" : "false");
    case NODE_TYPE_NUMBER:  return copy_to_heap_string(double_to_string(node.as.number)); // FIXME: Null result?
    case NODE_TYPE_ERROR: {
        char *str = malloc(1);
        str[0] = '\0';
        string_append(&str, "[ERROR] ");
        if (node.as.string != NULL) {
            string_append(&str, node.as.string);
        }
        return str;
    }
    case NODE_TYPE_ARRAY: {
        char *str = malloc(1);
        str[0] = '\0';
        string_append(&str, "[");
        if (pretty) {
            indent++;
            string_append(&str, "\n");
        }
        if (node.as.array->length > 0) {
            if (pretty) string_indent(&str, "  ", indent);
            string_append(&str, json_node_to_string_internal(node.as.array->nodes[0], pad, indent));
            for (size_t i = 1; i < node.as.array->length; i++) {
                if (pretty) {
                    string_append(&str, ",\n");
                    string_indent(&str, pad, indent);
                }
                else {
                    string_append(&str, ",");
                }
                string_append(&str, json_node_to_string_internal(node.as.array->nodes[i], pad, indent));
            }
        }
        if (pretty) {
            string_append(&str, "\n");
            indent--;
            string_indent(&str, pad, indent);
        }
        string_append(&str, "]");
        return str;
    }
    case NODE_TYPE_MAP: {
        char *str = malloc(1);
        str[0] = '\0';

        string_append(&str, "{");
        indent++;
        if (pretty) string_append(&str, "\n");
        
        for (size_t i = 0; i < node.as.map->length; i++) {
            if (i > 0) {
                string_append(&str, ",");
                if (pretty) string_append(&str, "\n");
            }
            if (pretty) string_indent(&str, pad, indent);
            string_append(&str, string_quote(node.as.map->entries[i].key));
            string_append(&str, ":");
            string_append(&str, json_node_to_string_internal(*node.as.map->entries[i].value, pad, indent));
        }

        if (pretty) {
            string_append(&str, "\n");
            indent--;
            string_indent(&str, "  ", indent);
        }
        string_append(&str, "}");
        return str;
    };
    }
    return NULL;
}

char *json_node_to_string(Node node) {
    return json_node_to_string_internal(node, NULL, 0);
}

char *json_node_to_pretty_string(Node node) {
    return json_node_to_string_internal(node, "  ", 0);
}

void json_print_node(Node node) {
    char *str = json_node_to_string(node);
    printf("%s\n", str);
    free(str);
}

void json_pretty_print_node(Node node) {
    char *str = json_node_to_pretty_string(node);
    printf("%s\n", str);
    free(str);
}

Node json_parse(char *input) {
    token *token = tokenize(input);
    Node node = parse_next_token(token);
    free_token_list(&token);
    return node;
}
