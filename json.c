#include "enum.h"
#include "json.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char *json_node_to_string_internal(json_node node, char *pad, int indent) {
    bool pretty = pad != NULL;
    
    switch (node.type) {
    case JSON_NULL:    return create_heap_string("null");
    case JSON_STRING:  return string_quote(node.as.string);
    case JSON_BOOLEAN: return create_heap_string(node.as.boolean ? "true" : "false");
    case JSON_NUMBER:  return create_heap_string(double_to_string(node.as.number)); // FIXME: Null result?
    case JSON_ERROR: {
        char *str = malloc(1);
        str[0] = '\0';
        string_append(&str, "[ERROR] ");
        if (node.as.string != NULL) {
            string_append(&str, node.as.string);
        }
        return str;
    }
    case JSON_ARRAY: {
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
    case JSON_OBJECT: {
        char *str = malloc(1);
        str[0] = '\0';

        string_append(&str, "{");
        indent++;
        if (pretty) string_append(&str, "\n");
        
        for (size_t i = 0; i < node.as.object->length; i++) {
            if (i > 0) {
                string_append(&str, ",");
                if (pretty) string_append(&str, "\n");
            }
            if (pretty) string_indent(&str, pad, indent);
            string_append(&str, string_quote(node.as.object->entries[i].key));
            string_append(&str, ":");
            string_append(&str, json_node_to_string_internal(*node.as.object->entries[i].value, pad, indent));
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

char *json_node_to_string(json_node node) {
    return json_node_to_string_internal(node, NULL, 0);
}

char *json_node_to_pretty_string(json_node node) {
    return json_node_to_string_internal(node, "  ", 0);
}

void json_print_node(json_node node) {
    char *str = json_node_to_string(node);
    printf("%s\n", str);
    free(str);
}

void json_pretty_print_node(json_node node) {
    char *str = json_node_to_pretty_string(node);
    printf("%s\n", str);
    free(str);
}

char* json_node_type_name(json_node node) {
    return json_type_name(node.type);
}

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
    }}
