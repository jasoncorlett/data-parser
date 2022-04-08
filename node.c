#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "node.h"

char *node_type_name(NodeType type) {
	switch (type) {
	case NODE_TYPE_ERROR: return "ERROR";
	case NODE_TYPE_NULL: return "NULL";
	case NODE_TYPE_STRING: return "STRING";
	case NODE_TYPE_BOOLEAN: return "BOOLEAN";
	case NODE_TYPE_NUMBER: return "NUMBER";
	case NODE_TYPE_ARRAY: return "ARRAY";
	case NODE_TYPE_MAP: return "MAP";
	default: return "<UNKNOWN NODE TYPE>";
	}
}

Node node_create_null() {
    return (Node){ .type = NODE_TYPE_NULL };
}

Node node_create_boolean(bool b) {
    return (Node){ .type = NODE_TYPE_BOOLEAN, .as.boolean = b };
}

Node node_create_number(double d) {
    return (Node){ .type = NODE_TYPE_NUMBER, .as.number = d };
}

Node node_create_string(char *str) {
    size_t len = strlen(str);
    char *t = malloc(len + 1);
    strncpy(t, str, len + 1);
    return (Node){ .type = NODE_TYPE_STRING, .as.string = t };
}

Node node_create_error(char *msg) {
    return (Node){ .type = NODE_TYPE_ERROR, .as.string = msg };
}

Node *node_create_array() {
    Node *node = calloc(1, sizeof(Node));
    node->as.array = calloc(1, sizeof(NodeArray));

    node->type = NODE_TYPE_ARRAY;
    return node;
}

void node_array_append(Node array, Node node) {
    int i = array.as.array->length;
    array.as.array->length ++;
    array.as.array->nodes = realloc(array.as.array->nodes, array.as.array->length * sizeof(Node));
    if (!array.as.array->nodes) {
        fprintf(stderr, "Error expanding array, memory probably corrupted"); // FIXME: Only dependency on stdio
    }
    memcpy(&array.as.array->nodes[i], &node, sizeof(Node));
}

Node *node_create_map() {
    Node *node = calloc(1, sizeof(Node));
    node->as.array = calloc(1, sizeof(NodeMap));

    node->type = NODE_TYPE_MAP;
    return node;
}

void node_map_set(Node node, char *key, Node value) {
    int i = node.as.map->length;
    node.as.map->length++;
    node.as.map->entries = realloc(node.as.map->entries, node.as.map->length * sizeof(NodeMapEntry));

    node.as.map->entries[i].key = key;

    node.as.map->entries[i].value = malloc(sizeof(Node));
    memcpy(node.as.map->entries[i].value, &value, sizeof(Node));
}

bool node_map_get(Node node, char *key, Node *value) {
    size_t key_length = strlen(key);
    for (size_t i = 0; i < node.as.map->length; i++) {
        NodeMapEntry entry = node.as.map->entries[i];
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
void node_free(Node node) {
    switch (node.type) {
        case NODE_TYPE_STRING:
        case NODE_TYPE_ERROR:
            free(node.as.string);
            break;
        default:
            break;
    }
}