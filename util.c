#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

#define MAX_DECIMALS 15
#define NUMBER_FORMAT "%.*lf"

char *double_to_string(double d) {
    char* str = NULL;
    int size = 0;
    int decimal_count = MAX_DECIMALS;

    // 1. Determine number of decimal places
    size = snprintf(NULL, 0, NUMBER_FORMAT, MAX_DECIMALS, d);
    if (size < 0) return NULL;
    
    size++;
    str = malloc(size);
    if (str == NULL) return NULL;

    snprintf(str, size, NUMBER_FORMAT, MAX_DECIMALS, d);
    
    for (int i = 2; str[size - i] == '0'; i++) {
        decimal_count--;
    }
    free(str);

    size = snprintf(NULL, 0, NUMBER_FORMAT, decimal_count, d);
    
    size++;
    str = malloc(size); // FIXME: I think this is being leaked :)
    if (str == NULL) return NULL;

    snprintf(str, size, NUMBER_FORMAT, decimal_count, d);
    return str;
}

char *create_heap_string(void) {
    char *str = malloc(1);
    str[0] = '\0';
    return str;
}

char *copy_to_heap_string(char *str) {
    size_t len = strlen(str) + 1;
    char *heap = malloc(len);
    strncpy(heap, str, len);
    return heap;
}

void string_append(char **str1, char *str2) {
    size_t len1 = strlen(*str1);
    size_t len2 = strlen(str2);

    *str1 = realloc(*str1, len1 + len2 + 1);
    if (*str1 == NULL) {
        fprintf(stderr, "Could not append to string!");
        exit(1);
    }

    strncat(*str1, str2, len2);
}

char *string_quote(char *str) {
    if (str == NULL) return NULL;
    char *quoted = create_heap_string();
    string_append(&quoted, "\"");
    string_append(&quoted, str);
    string_append(&quoted, "\"");
    return quoted;
}

void string_indent(char **str, char *pad, int count) {
    for (int i = 0; i < count; i++) {
        string_append(str, pad);
    }
}
