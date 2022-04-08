#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "node.h"

json_node json_create_null() {
    return (json_node){ .type = JSON_NULL };
}

json_node json_create_boolean(bool b) {
    return (json_node){ .type = JSON_BOOLEAN, .as.boolean = b };
}

json_node json_create_number(double d) {
    return (json_node){ .type = JSON_NUMBER, .as.number = d };
}

json_node json_create_string(char *str) {
    size_t len = strlen(str);
    char *t = malloc(len + 1);
    strncpy(t, str, len + 1);
    return (json_node){ .type = JSON_STRING, .as.string = t };
}

json_node json_create_error(char *msg) {
    return (json_node){ .type = JSON_ERROR, .as.string = msg };
}

json_node *json_create_array() {
    json_node *node = calloc(1, sizeof(json_node));
    node->as.array = calloc(1, sizeof(json_array));

    node->type = JSON_ARRAY;
    return node;
}

void json_array_append(json_node array, json_node node) {
    int i = array.as.array->length;
    array.as.array->length ++;
    array.as.array->nodes = realloc(array.as.array->nodes, array.as.array->length * sizeof(json_node));
    if (!array.as.array->nodes) {
        fprintf(stderr, "Error expanding array, memory probably corrupted"); // FIXME
    }
    memcpy(&array.as.array->nodes[i], &node, sizeof(json_node));
}

json_node *json_create_object() {
    json_node *node = calloc(1, sizeof(json_node));
    node->as.array = calloc(1, sizeof(json_object));

    node->type = JSON_OBJECT;
    return node;
}

void json_object_set(json_node node, char *key, json_node value) {
    int i = node.as.object->length;
    node.as.object->length++;
    node.as.object->entries = realloc(node.as.object->entries, node.as.object->length * sizeof(json_object_entry));

    node.as.object->entries[i].key = key;

    node.as.object->entries[i].value = malloc(sizeof(json_node));
    memcpy(node.as.object->entries[i].value, &value, sizeof(json_node));
}

bool json_object_get(json_node node, char *key, json_node *value) {
    size_t key_length = strlen(key);
    for (size_t i = 0; i < node.as.object->length; i++) {
        json_object_entry entry = node.as.object->entries[i];
        size_t entry_key_length = strlen(entry.key);
        if (key_length != entry_key_length)
            continue;
        if (strncmp(key, entry.key, key_length) == 0) {
            value = entry.value;
            return true;
        }
    }
    return false;
}

// TODO: Array and objects
void json_free_node(json_node node) {
    switch (node.type) {
        case JSON_STRING:
        case JSON_ERROR:
            free(node.as.string);
            break;
        default:
            break;
    }
}