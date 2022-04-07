#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "enum.h"

typedef struct json_node            json_node;
typedef struct json_array           json_array;
typedef struct json_object_entry    json_object_entry;
typedef struct json_object          json_object;

struct json_array {
    size_t length;
    json_node *nodes;
};

struct json_object_entry {
    char *key;
    json_node *value;
};

struct json_object {
    size_t length;
    json_object_entry *entries;
};

struct json_node {
    json_type type;
    union {
        char *string;
        char boolean;
        double number;
        json_array *array;
        json_object *object;
    } as;
};

char *json_node_to_string(json_node node);
char *json_node_type_name(json_node node);
void json_print_node(json_node node);
void json_pretty_print_node(json_node node);

json_node *json_create_array();
void json_array_append(json_node array, json_node node);

json_node *json_create_object();
void json_object_set(json_node node, char *key, json_node value);

json_node json_create_null();
json_node json_create_boolean(bool b);
json_node json_create_number(double d);
json_node json_create_string(char *str);
json_node json_create_error(char *str);

void json_free_node(json_node node);
