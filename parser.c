#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "json.h"
#include "parser.h"

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

void free_token_list(token *list) { // FIXME: Memory leak
    // if (list == NULL) return;
    // if (list->next != NULL)
    //     free_token_list(list->next);
    // free(list);
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
            free_token_list(list);
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

json_node parse_next_token(token *token);

json_node parse_error(char *message, token at) {
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
    return json_create_error(str);
}

json_node parse_array(token *token) {
    json_node array = *json_create_array();

    token = token->next; // left bracket
    while (token != NULL && token->type != TOKEN_RIGHT_BRACKET) {
        json_node node = parse_next_token(token);
        json_array_append(array, node);
        token = token->next;

        if (token != NULL && token->type == TOKEN_COMMA) {
            token = token->next;
        }
        else {
            break;
        }
    }

    if (token == NULL) return json_create_error("Expected ] at EOF");
    if (token->type != TOKEN_RIGHT_BRACKET) return json_create_error("Expected ]");
    token = token->next; // right bracket

    return array;
}

json_node parse_object(token *token) {
    json_node object = *json_create_object();

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
        json_node value = parse_next_token(token);
        json_object_set(object, key, value);

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

json_node parse_next_token(token *token) {
    switch (token->type) {
    case TOKEN_LEFT_BRACE:
        return parse_object(token);
    case TOKEN_LEFT_BRACKET:
        return parse_array(token);
    case TOKEN_NUMBER:
        return json_create_number(token->value.number);
    case TOKEN_STRING:
        return json_create_string(token->value.string);
    case TOKEN_TRUE:
        return json_create_boolean(true);
    case TOKEN_FALSE:
        return json_create_boolean(false);
    case TOKEN_NULL:
        return json_create_null();
    case TOKEN_ERROR:
        return parse_error(NULL, *token);
    default:
        return parse_error("Unknown token type!", *token);
    }
        
}

json_node json_parse(char *input) {
    token *token = tokenize(input);
    return parse_next_token(token);
}