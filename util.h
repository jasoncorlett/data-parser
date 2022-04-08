#ifndef UTIL_H
#define UTIL_H 1

char *create_heap_string(void);
char *copy_to_heap_string(char *str);

char *string_quote(char *str);
char *double_to_string(double d);

void string_append(char **str1, char *str2);
void string_indent(char **str, char *pad, int count);

#endif